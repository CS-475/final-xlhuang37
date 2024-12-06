/*
 *  Copyright 2024 <me>
 */

#include "include/GPath.h"
#include "switch_blit.h"
#include "starter_canvas.h"
#include "utils_blend_poly.h"
#include "utils_blend_nonconvex.h"
#include "shader_combined.h"
#include "shader_mesh_color.h"
#include "shader_proxy.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <list>

void MyCanvas::clear(const GColor& color) {

    const unsigned a = (unsigned)(color.a * 255 + 0.5f);
    const unsigned r = (unsigned)(color.a * color.r * 255 + 0.5f);
    const unsigned g = (unsigned)(color.a * color.g * 255 + 0.5f);
    const unsigned b = (unsigned)(color.a * color.b * 255 + 0.5f);

    const int size = fDevice.width() * fDevice.height();

    GPixel* original_pixel = fDevice.getAddr(0, 0);

    const int src = GPixel_PackARGB(a,r,g,b);

    for(int i = 0; i < size; i++) {
            *(original_pixel + i) = src;
    }
}

void MyCanvas::drawRect(const GRect& rect, const GPaint& paint) {

    const GMatrix& mat = this->mat_stack.top();
    if((mat.e0()).y != 0 || (mat.e1()).x != 0) {
        std::vector<GPoint> four_points;
        four_points.push_back({rect.left, rect.top});
        four_points.push_back({rect.right, rect.top});
        four_points.push_back({rect.right, rect.bottom});
        four_points.push_back({rect.left, rect.bottom});
        drawConvexPolygon(&four_points[0], 4, paint);
        return;
    }


    GBlendMode mode = paint.getBlendMode();
    GShader* shader = paint.peekShader();

    // CONSTANTS
    GIRect giRect = rect.round();
    const GColor color = paint.getColor();
    const int width = fDevice.width();
    const int height = fDevice.height();

    float left_f = giRect.left * mat.e0().x + mat.origin().x;
    int left = clamping(GRoundToIntNonNegative(left_f), 0, width);
    float right_f = giRect.right * mat.e0().x + mat.origin().x;
    int right = clamping(GRoundToIntNonNegative(right_f), 0, width);
    if(left > right) {
        int tmp = left;
        left = right;
        right = tmp;
    }
    
    float top_f = giRect.top * mat.e1().y + mat.origin().y;
    int top = clamping(GRoundToIntNonNegative(top_f), 0, height);
    float bottom_f = giRect.bottom * mat.e1().y + mat.origin().y;
    int bottom = clamping(GRoundToIntNonNegative(bottom_f), 0, height); 
    if(top > bottom) {
        int tmp = top;
        top = bottom;
        bottom = tmp;
    }

    const unsigned int a = (int)(color.a * 255 + 0.5f);
    const unsigned int r = (int)(color.a * color.r * 255 + 0.5f);
    const unsigned int g = (int)(color.a * color.g * 255 + 0.5f);
    const unsigned int b = (int)(color.a * color.b * 255 + 0.5f);

    const unsigned int src = GPixel_PackARGB(a,r,g,b);

    // defining function pointer
    void (*blit)(GPixel src, int Sa, int left, int right, GPixel* row) = nullptr;
    void (*blit_shader)(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) = nullptr;
    // Setting up Function Pointers
    int err_no = switchMode(a, shader, mode, blit, blit_shader);
    if(err_no) {
        return;
    }
    assert(blit != nullptr || blit_shader != nullptr);
    
    if(shader == nullptr) {
        for(int j = top; j < bottom; j++) {
            GPixel* row = this->fDevice.getAddr(0, j);
            blit(src, a, left, right, row);
        }
    } else {
        if(!shader->setContext(this->mat_stack.top()))
            return;
        int length = right - left;
        if(blit_shader == kSrcBlitShader) {
            for(int j = top; j < bottom; j++) {
                GPixel* row = this->fDevice.getAddr(0, j);
                shader->shadeRow(left, j, length, &row[left]);
            }
        } else {
            std::vector<GPixel> src_list(length, 0);
            for(int j = top; j < bottom; j++) {
                GPixel* row = this->fDevice.getAddr(0, j);
                shader->shadeRow(left, j, length, &src_list[0]);
                blit_shader(shader, left, right, row, src_list);
            }
        }
    }
}


void MyCanvas::drawConvexPolygon(const GPoint points[], int count, const GPaint& paint) {
     
    if(count < 2) {
        return;
    }
    std::vector<GPoint> tm_points;
    tm_points.reserve(count);

    GMatrix& ctm = this->mat_stack.top();
    ctm.mapPoints(&tm_points[0], points, count);
 


    GBlendMode mode = paint.getBlendMode();
    GShader* shader = paint.peekShader();

    // Constants
    const int width = fDevice.width();
    const int height = fDevice.height();
    const GColor color = paint.getColor();
   

    // Color
    const int a = (int)(color.a * 255 + 0.5f);
    const int r = (int)(color.a * color.r * 255 + 0.5f);
    const int g = (int)(color.a * color.g * 255 + 0.5f);
    const int b = (int)(color.a * color.b * 255 + 0.5f);

    const int src = GPixel_PackARGB(a,r,g,b);
    // defining function pointer
    void (*blit)(GPixel src, int Sa, int left, int right, GPixel* row) = nullptr;
    void (*blit_shader)(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) = nullptr;

    // Setting up Function Pointers
    int err_no = switchMode(a, shader, mode, blit, blit_shader);
    if(err_no < 0) {
#if _g_starter_canvas_h_debug_ == 1
        std::cout << "Debugging: shade mode dst, returning" << std::endl;
#endif
        return;
    }
    assert(blit != nullptr || blit_shader != nullptr);

    // Initialize the edges
    std::vector<Edge> GEdges = std::vector<Edge>();
    GEdges.reserve(count * 3);

    processEdgeInPlace(GEdges, &tm_points[0], width, height, count);

    if(GEdges.size() < 2) {
        return;
    }

    // sort edges
    std::sort(GEdges.begin(), GEdges.end());


    // Prepare Args
    int top = GEdges[0].top;
    int bottom = GEdges[GEdges.size() - 1].bottom;
    // Scanline Bland of Choice
    if(shader == nullptr) {
        assert(blit != nullptr);
        
        assert(GEdges.size() > 1);
        Edge active_edge_0 = GEdges[0];
        Edge active_edge_1 = GEdges[1];
        int edge_idx = 2;
        
        for(int y = top; y < bottom; y++) {
            // Edges moving down
            if(y == active_edge_0.bottom && edge_idx < GEdges.size()) {
                active_edge_0 = GEdges[edge_idx];
                edge_idx++;
            }
            if(y == active_edge_1.bottom && edge_idx < GEdges.size()) {
                active_edge_1 = GEdges[edge_idx];
                edge_idx++;
            }

            int x0 = GRoundToIntNonNegative(active_edge_0.slope * (y + 0.5f) + active_edge_0.offset);
            int x1 = GRoundToIntNonNegative(active_edge_1.slope * (y + 0.5f) + active_edge_1.offset);

            int left, right;
            if(x0 < x1) {
                left = x0;
                right = x1;
            } else {
                left = x1;
                right = x0;
            }

            GPixel* row = this->fDevice.getAddr(0, y);
            blit(src, a, left, right, row);
        }
    } else {
        if(!shader->setContext(this->mat_stack.top()))
            return;

        assert(GEdges.size() > 1);
        Edge active_edge_0 = GEdges[0];
        Edge active_edge_1 = GEdges[1];
        int edge_idx = 2;
        std::vector<GPixel> src_list(width, 0);
        for(int y = top; y < bottom; y++) {
            // Edges moving down
            if(y == active_edge_0.bottom && edge_idx < GEdges.size()) {
                active_edge_0 = GEdges[edge_idx];
                edge_idx++;
            }
            if(y == active_edge_1.bottom && edge_idx < GEdges.size()) {
                active_edge_1 = GEdges[edge_idx];
                edge_idx++;
            }

            int x0 = GRoundToIntNonNegative(active_edge_0.slope * (y + 0.5f) + active_edge_0.offset);
            int x1 = GRoundToIntNonNegative(active_edge_1.slope * (y + 0.5f) + active_edge_1.offset);
            int left, right;
            if(x0 < x1) {
                left = x0;
                right = x1;
            } else {
                left = x1;
                right = x0;
            }

            int length = right - left; 
            shader->shadeRow(left, y, length, &src_list[0]);

            GPixel* row = this->fDevice.getAddr(0, y);
            blit_shader(shader, left, right, row, src_list);
        }
    }
}

void MyCanvas::drawPath(const GPath& path, const GPaint& paint) {
    GBlendMode mode = paint.getBlendMode();
    GShader* shader = paint.peekShader();

    GMatrix& ctm = this->mat_stack.top();

    // Constants
    const int width = fDevice.width();
    const int height = fDevice.height();
    const GColor color = paint.getColor();

    // Color
    const int a = (int)(color.a * 255 + 0.5f);
    const int r = (int)(color.a * color.r * 255 + 0.5f);
    const int g = (int)(color.a * color.g * 255 + 0.5f);
    const int b = (int)(color.a * color.b * 255 + 0.5f);

    const int src = GPixel_PackARGB(a,r,g,b);
    // defining function pointer
    void (*blit)(GPixel src, int Sa, int left, int right, GPixel* row) = nullptr;
    void (*blit_shader)(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) = nullptr;
    // Setting up Function Pointers
    int err_no = switchMode(a, shader, mode, blit, blit_shader);
    if(err_no) 
        return;
    assert(blit != nullptr || blit_shader != nullptr);

    // Initialize the edges
    std::vector<EdgeWithDirection> GEdges = std::vector<EdgeWithDirection>();
    GEdges.reserve(path.countPoints() * 4);

    GPoint pts[GPath::kMaxNextPoints];
    GPath::Edger edger(path);
    bool hasPrevSegment = true;
    while(auto v = edger.next(pts)) {
        if(v == GPathVerb::kCubic) {
            ctm.mapPoints(&pts[0], &pts[0], 4);
            GPoint& A = pts[0];
            GPoint& B = pts[1];
            GPoint& C = pts[2];
            GPoint& D = pts[3];
            float tolerance_inv = 1.0f;
            float error_x_0 = A.x - 2 * B.x + C.x;
            float error_y_0 = A.y - 2 * B.y + C.y;
            float error_x_1 = B.x - 2 * C.x + D.x;
            float error_y_1 = B.y - 2 * C.y + D.y;
            float error_x =  std::max(error_x_0, error_x_1);
            float error_y = std::max(error_y_0, error_y_1);
            float total_error = error_x * error_x + error_y * error_y;
            int num_segs = (int)ceil(sqrt((3*total_error)*(0.25 * tolerance_inv)));
            float step = 1.0f / num_segs;
            float start = step;
            GPoint prev_point = A;
            GPoint curr_point;
            for(int i = 1; i < num_segs; i++) {
                curr_point = eval_cubic_GPoint(start, A, B, C, D);
                connectTwoPoints(GEdges, prev_point, curr_point, width, height, hasPrevSegment);
                prev_point = curr_point;
                start += step;
            }
            curr_point = D;
            connectTwoPoints(GEdges, prev_point, curr_point, width, height, hasPrevSegment);
            hasPrevSegment = true;


            // int num_segs_pow2 = 1;
            // while(num_segs_pow2 < num_segs) {
            //     num_segs_pow2 <<= 1;
            // }
            
            // recursion_cubic(A, B, C, D, num_segs_pow2, GEdges, width, height);
        } 
        else if (v == GPathVerb::kQuad){
            ctm.mapPoints(&pts[0], &pts[0], 3);
            GPoint& A = pts[0];
            GPoint& B = pts[1];
            GPoint& C = pts[2];
            float tolerance_inv = 1.0f;
            float error_x = (A.x - 2 * B.x + C.x) * 0.25f;
            float error_y = (A.y - 2 * B.y + C.y) * 0.25f;
            float total_error = error_x * error_x + error_y * error_y;
            int num_segs = (int)ceil(sqrt(total_error * tolerance_inv));
            float step = 1.0f / num_segs;
            float start = step;
            GPoint prev_point = A;
            GPoint curr_point;
            for(int i = 1; i < num_segs; i++) {
                curr_point = eval_quad_GPoint(start, A, B, C);
                connectTwoPoints(GEdges, prev_point, curr_point, width, height, hasPrevSegment);
                prev_point = curr_point;
                start += step;
            }
            curr_point = C;
            connectTwoPoints(GEdges, prev_point, curr_point, width, height, hasPrevSegment);
            hasPrevSegment = true;
            // int num_segs_pow2 = 1;
            // while(num_segs_pow2 < num_segs) {
            //     num_segs_pow2 <<= 1;
            // }
            
            // recursion_quad(A, B, C, num_segs_pow2, GEdges, width, height);
        }
        else {
            ctm.mapPoints(&pts[0], &pts[0], 2);

            connectTwoPoints(GEdges, pts[0], pts[1], width, height, hasPrevSegment);
            if(v == GPathVerb::kMove) {
                hasPrevSegment = false;
            } else {
                hasPrevSegment = true;
            }
        
        } 
    }
    if(GEdges.size() < 2) {
        return;
    }

    int top = GEdges[0].top;
    int bottom = GEdges[0].bottom;
    for(int i = 1; i < GEdges.size(); i++) {
        if(GEdges[i].top < top)
            top = GEdges[i].top;
        if(GEdges[i].bottom > bottom)
            bottom = GEdges[i].bottom;
    }
    assert(top <= bottom);
    // for(int i = 0; i < GEdges.size(); i++) {
    //     std::cout << GEdges[i].top << "  "<< GEdges[i].bottom << "  "<< GEdges[i].goingUp << std::endl;
    // }

    // sort edges
    std::sort(GEdges.begin(), GEdges.end());

    if(shader == nullptr) {
        int size = GEdges.size();
        int actual_start = 0;
        std::vector<int> arr(size);
        // std::vector<int> index_vector(size);
        // std::vector<EdgeWithDirection> sorted_edges(size);
        

        for(int y = top; y < bottom; y++) {
            int w = 0;
            bool erased = false;
            int i = actual_start;
            int L;
            
            while(i < size && GEdges[i].isValid(y)) {
                int x = GRoundToIntNonNegative(GEdges[i].computeX(y));
                if(w == 0) 
                    L = x;
                w += GEdges[i].goingUp;
                if(w==0) {
                    int R = x;
                    assert(R >= L);
                    GPixel* row = this->fDevice.getAddr(0, y);
                    blit(src, a, L, R, row); 
                }
                if(GEdges[i].isValid(y + 1)) {
                    i += 1;
                } else {
                    std::swap(GEdges[i], GEdges[actual_start]);
                    actual_start++;
                    i++;
                    erased = true;
                    // i points to next element; 
                }
            }

            assert(w == 0);
            while(i < size && GEdges[i].isValid(y + 1)) {
                i += 1;
            }

            // std::sort(GEdges.begin() + actual_start, GEdges.begin() + i, comparator);
            // insertion_sort(GEdges.begin() + actual_start, i - actual_start, comparator);
            int subsize = i - actual_start;
            for(int index = actual_start; index < i; index++) {
                arr[index] = GEdges[index].computeX(y + 1);
            }
            if(subsize < 200) {
                insertion_sort_with_val(GEdges.begin() + actual_start, arr.begin() + actual_start, i - actual_start);
            } else {
                std::vector<int> index_vector(subsize);
                std::vector<EdgeWithDirection> sorted_edges(subsize);
                for(int index = 0; index < subsize; index++) {
                    index_vector[index] = index;
                }
                std::sort(index_vector.begin(), index_vector.begin() + subsize, [&](int a, int b) {
                    return arr[a + actual_start] < arr[b + actual_start];
                });
                for(int index = 0; index < subsize; index++){
                    sorted_edges[index] = GEdges[index_vector[index] + actual_start];
                }
                for(int index = 0; index < subsize; index++){
                    GEdges[index + actual_start] = sorted_edges[index];
                }
            }


        }
    } else {
        if(!shader->setContext(this->mat_stack.top()))
            return;
        int size = GEdges.size();
        EdgeWithDrectionComparator comparator = {0};
        int actual_start = 0;

        for(int y = top; y < bottom; y++) {
            int w = 0;
            int actual_start_addition = 0;
            int i = actual_start;
            int L;
            
            while(i < size && GEdges[i].isValid(y)) {
                int x = GRoundToIntNonNegative(GEdges[i].computeX(y));
                if(w == 0) 
                    L = x;
                w += GEdges[i].goingUp;
                if(w==0) {
                    int R = x;
                    assert(R >= L);
                    GPixel* row = this->fDevice.getAddr(0, y);
                    std::vector<GPixel> src_list(R - L, 0);
                    shader->shadeRow(L, y, R - L, &src_list[0]);
                    blit_shader(shader, L, R, row, src_list); 
                }
                if(GEdges[i].isValid(y + 1)) {
                    i += 1;
                } else {
                    GEdges[i].offset = -INFINITY;
                    i++;
                    actual_start_addition++;
                    // i points to next element; 
                }
            }

            assert(w == 0);
            while(i < size && GEdges[i].isValid(y + 1)) {
                i += 1;
            }

            comparator.y = y + 1;
            std::sort(GEdges.begin() + actual_start, GEdges.begin() + i, comparator);
            actual_start += actual_start_addition;
            
            // int sublist_size = i - actual_start;
            // for(int index = 0; index < size; index++) {
            //     arr[index] = GEdges[index].computeX(y + 1);
            // }

            // insertion_sort_with_val(GEdges.begin() + actual_start, arr, i - actual_start);
        }
    }
}

void MyCanvas::drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                          int count, const int indices[], const GPaint& paint) {
    int n = 0;
    GPoint triangle[3];
    GPoint texs_shader[3];
    GColor colors_shader[3];
    GPaint paint_cp = paint;
        if(colors && texs) {
            for(int i = 0; i < count; i++) {
                triangle[0] = verts[indices[n]];
                triangle[1] = verts[indices[n + 1]];
                triangle[2] = verts[indices[n + 2]];
                colors_shader[0] = colors[indices[n]];
                colors_shader[1] = colors[indices[n + 1]];
                colors_shader[2] = colors[indices[n + 2]];
                texs_shader[0] = texs[indices[n]];
                texs_shader[1] = texs[indices[n + 1]];
                texs_shader[2] = texs[indices[n + 2]];
                std::shared_ptr<GShader> sp = GCreateCombinedShader(triangle, texs_shader, colors_shader, paint.peekShader());
                
                paint_cp.setShader(sp);
                drawConvexPolygon(&triangle[0], 3, paint_cp);
                n += 3;
            }
        } else if(colors) {
            for(int i = 0; i < count; i++) {
                triangle[0] = verts[indices[n]];
                triangle[1] = verts[indices[n + 1]];
                triangle[2] = verts[indices[n + 2]];
                colors_shader[0] = colors[indices[n]];
                colors_shader[1] = colors[indices[n + 1]];
                colors_shader[2] = colors[indices[n + 2]];
                std::shared_ptr<GShader> sp = GCreateMeshColorShader(&triangle[0], &colors_shader[0]);
                paint_cp.setShader(sp);
                drawConvexPolygon(&triangle[0], 3, paint_cp);
                n += 3;
            }
        } else if (texs) {
            for(int i = 0; i < count; i++) {
                triangle[0] = verts[indices[n]];
                triangle[1] = verts[indices[n + 1]];
                triangle[2] = verts[indices[n + 2]];
                texs_shader[0] = texs[indices[n]];
                texs_shader[1] = texs[indices[n + 1]];
                texs_shader[2] = texs[indices[n + 2]];
                std::shared_ptr<GShader> sp = GCreateProxyShader(&triangle[0], &texs_shader[0], paint.peekShader());
                paint_cp.setShader(sp);
                drawConvexPolygon(&triangle[0], 3, paint_cp);
                n += 3;
            }
        } else {
            for(int i = 0; i < count; i++) {
                triangle[0] = verts[indices[n]];
                triangle[1] = verts[indices[n + 1]];
                triangle[2] = verts[indices[n + 2]];
                drawConvexPolygon(&triangle[0], 3, paint);
                n += 3;
            }
        }
        
}

static inline GPoint compute_point_quad(float u, float v, const GPoint points[4]) {
    return  (1 - u) * (points[0] + v * (points[3] - points[0]))
            + u * (points[1] + v * (points[2] - points[1]));
}

static inline GColor compute_color_quad(float u, float v, const GColor colors[4]) {
    return (1 - u) * (colors[0] + v * (colors[3] - colors[0]))
            + u * (colors[1] + v * (colors[2] - colors[1]));
}

void MyCanvas::drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                          int level, const GPaint& paint) {
    int size = level + 1;
    int num_points = size + 1;
    float size_inv = 1.0f / size;
    float u = 0.0f;
    float v = 0.0f;
    int num_triangle = size * size * 2;
    std::vector<int> indices(num_triangle * 3);
    int index = 0;
    int index_0 = 0;
    int index_1 = 1;
    int index_2 = num_points;
    int index_3 = 1;
    int index_4 = num_points;
    int index_5 = num_points + 1;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            indices[index] = index_0;
            indices[index + 1] = index_1;
            indices[index + 2] = index_2;
            indices[index + 3] = index_3;
            indices[index + 4] = index_4;
            indices[index + 5] = index_5;
            index_0++;
            index_1++;
            index_2++;
            index_3++;
            index_4++;
            index_5++;
            index += 6;
        }
        index_0++;
        index_1++;
        index_2++;
        index_3++;
        index_4++;
        index_5++;
    }

    // index_0 = 1;
    // index_1 = num_points;
    // index_2 = num_points + 1;
    // for(int i = 0; i < size; i++) {
    //     for(int j = 0; j < size; j++) {
    //         indices[index] = index_0;
    //         indices[index + 1] = index_1;
    //         indices[index + 2] = index_2;
    //         index_0++;
    //         index_1++;
    //         index_2++;
    //         index += 3;
    //     }
    //     index_0++;
    //     index_1++;
    //     index_2++;
    // }

    if(colors != NULL && texs != NULL) {
        std::vector<GPoint> verts_level(num_points * num_points);
        std::vector<GColor> colors_level(num_points * num_points);
        std::vector<GPoint> texs_level(num_points * num_points);
        index = 0;
        for(int i = 0; i < num_points; i++) {
            u = 0;
            for(int j = 0; j < num_points; j++) {
                verts_level[index] = compute_point_quad(u, v, verts);
                colors_level[index] = compute_color_quad(u, v, colors);
                texs_level[index] = compute_point_quad(u, v, texs);
                index++;
                u += size_inv;
            }
            v += size_inv;
        }
        this->drawMesh(&verts_level[0], &colors_level[0], &texs_level[0],
                          num_triangle, &indices[0], paint);
    } else if (colors != NULL) {
        std::vector<GPoint> verts_level(num_points * num_points);
        std::vector<GColor> colors_level(num_points * num_points);
        for(int i = 0; i < num_points; i++) {
            u = 0.0f;
            index = i * num_points;
            for(int j = 0; j < num_points; j++) {
                verts_level[index] = compute_point_quad(u, v, verts);
                colors_level[index] = compute_color_quad(u, v, colors);
                index++;
                u += size_inv;
            }
            v += size_inv;
        }
        this->drawMesh(&verts_level[0], &colors_level[0], NULL,
                          num_triangle, &indices[0], paint);
    } else if (texs != NULL) {
        std::vector<GPoint> verts_level(num_points * num_points);
        std::vector<GPoint> texs_level(num_points * num_points);
        index = 0;
        for(int i = 0; i < num_points; i++) {
            u = 0;
            for(int j = 0; j < num_points; j++) {
                verts_level[index] = compute_point_quad(u, v, verts);
                texs_level[index] = compute_point_quad(u, v, texs);
                index++;
                u += size_inv;
            }
            v += size_inv;
        }
        this->drawMesh(&verts_level[0], NULL, &texs_level[0],
                          num_triangle, &indices[0], paint);
    } else {
        std::vector<GPoint> verts_level(num_points * num_points);
        index = 0;
        for(int i = 0; i < num_points; i++) {
            u = 0;
            for(int j = 0; j < num_points; j++) {
                verts_level[index] = compute_point_quad(u, v, verts);
                index++;
                u += size_inv;
            }
            v += size_inv;
        }
        this->drawMesh(&verts_level[0], NULL, NULL,
                          num_triangle, &indices[0], paint);
    }
    

}

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

void MyCanvas::save() {
    this->mat_stack.push(this->mat_stack.top());
}

void MyCanvas::restore() {
    this->mat_stack.pop();
}

void MyCanvas::concat(const GMatrix& matrix) {
    this->mat_stack.top() = GMatrix::Concat(this->mat_stack.top(), matrix);
}


std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    // as fancy as you like
    // ...
    canvas->clear(GColor::RGBA(0.0,1.0,0.0,1.0));
    // canvas->fillRect(...);

    return "Lazy Student";
}