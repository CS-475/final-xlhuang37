#ifndef _g_blend_nonconvex_h_
#define _g_blend_nonconvex_h_

#include "include/GTypes.h"
#include "include/GMath.h"
#include "include/GPoint.h"
#include "utils.h"
#include <iostream>

struct EdgeWithDirection {
    // x = my + b
    float slope;
    float offset;
    int top;
    int bottom;
    int goingUp;

    int computeX(int y) const {
        return GRoundToIntNonNegative(slope * (y + 0.5f) + offset);
    }

    // Overloading the < operator for sorting by top
    bool operator<(const EdgeWithDirection& other) const {
        if(top == other.top) {
            int x0 = GRoundToIntNonNegative(computeX(top));
            float x1 = GRoundToIntNonNegative(other.computeX(other.top));
            return x0 < x1; 
        } else {
            return top < other.top;
        }
    }

    // inline bool compareByX(const EdgeWithDirection& other) const {
    //     return currX < other.currX;
    // }

    inline bool isValid(int y) const {
        return (y >= top) && (y < bottom);
    }
};

struct EdgeWithDrectionComparator {
    int y;

    inline bool operator()(EdgeWithDirection &left, EdgeWithDirection &right) {
        return left.computeX(y) < right.computeX(y);
    }
};

// Copied from Online
static inline void insertion_sort(std::vector<EdgeWithDirection>::iterator begin, int n, EdgeWithDrectionComparator comparator) {
    for (int i = 1; i < n; ++i) {
        int j = i - 1;
        EdgeWithDirection tmp = *(begin + i);

        /* Move elements of arr[0..i-1], that are
           greater than key, to one position ahead
           of their current position */
        while (j >= 0 && comparator(tmp, *(begin + j))) {
            *(begin + j + 1) = *(begin + j);
            --j;
        }

        *(begin + j + 1) = tmp;
    }
}

// Copied from Online
static inline void insertion_sort_with_val(std::vector<EdgeWithDirection>::iterator begin, std::vector<int>::iterator arr, int n) {
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        EdgeWithDirection tmp = *(begin + i);
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            *(begin + j + 1) = *(begin + j);
            j = j - 1;
        }
        arr[j + 1] = key;
        *(begin + j + 1) = tmp;
    }
}

// Copied from Online
// static inline void insertion_sort_new_field(std::vector<EdgeWithDirection>::iterator begin, int n) {
//     for (int i = 1; i < n; ++i) {
//         int j = i - 1;
//         EdgeWithDirection tmp = *(begin + i);

//         /* Move elements of arr[0..i-1], that are
//            greater than key, to one position ahead
//            of their current position */
//         while (j >= 0 && tmp.compareByX(*(begin + j))) {
//             *(begin + j + 1) = *(begin + j);
//             --j;
//         }

//         *(begin + j + 1) = tmp;
//     }
// }


static inline void connectTwoPoints(std::vector<EdgeWithDirection> &GEdges, const GPoint& pt_1, const GPoint& pt_2, int width, int height, bool hasPrev) {
        const float& lefty = pt_1.y;
        const float& righty = pt_2.y;
        int rounded_lefty = clamping(GRoundToIntNonNegative(lefty), 0, height);
        int rounded_righty = clamping(GRoundToIntNonNegative(righty), 0, height); 
        float leftx = pt_1.x;
        float rightx = pt_2.x;
        int prev_index = GEdges.size() - 1;
        
        if((rounded_lefty != rounded_righty)) {
            // int goingUp = signFunction(rounded_righty - rounded_lefty);
            int goingUp;
            if(rounded_lefty < rounded_righty) {
                goingUp = 1;
            }
            else { 
                goingUp = -1;
            }
                
            // slope and offset
            float delta_x = leftx - rightx;
            float delta_y = lefty - righty;
            float slope = delta_x / delta_y;
            float offset = leftx - slope * lefty;

            leftx = (rounded_lefty - lefty) * slope + leftx;
            rightx = (rounded_righty - righty) * slope + rightx;
            // left intersection and excess
            int top = std::min(rounded_lefty, rounded_righty);
            int bottom = std::max(rounded_lefty, rounded_righty);
            float left = (leftx < rightx) ? leftx : rightx;
            float right = (leftx >= rightx) ? leftx : rightx;
            if(left <= 0) {
                if(right <= 0) {
                    slope = 0;
                    offset = 0;
                } else {
                    int inter_0 = GRoundToIntNonNegative(- offset / slope);
                    if(inter_0 > top && inter_0 < bottom){
                        if(slope < 0) {
                            EdgeWithDirection vertical_edge = EdgeWithDirection();
                            vertical_edge.top = inter_0;
                            vertical_edge.bottom = bottom;
                            vertical_edge.slope  = 0;
                            vertical_edge.offset = 0;
                            vertical_edge.goingUp = goingUp;
                            GEdges.push_back(vertical_edge);                                
                            bottom = inter_0;
                        } else {
                                EdgeWithDirection vertical_edge = EdgeWithDirection();
                                vertical_edge.slope  = 0;
                                vertical_edge.offset = 0;
                                vertical_edge.top = top;
                                vertical_edge.bottom = inter_0;
                                vertical_edge.goingUp = goingUp;
                                GEdges.push_back(vertical_edge);
                            top = inter_0;
                        }
                    }
                }
            } 
            if(right >= width) {
                if(left >= width) {
                    slope = 0;
                    offset = width;
                } else {
                    int inter_width = GRoundToIntNonNegative((width - offset) / slope);
                    if(inter_width > top && inter_width < bottom){
                        if(slope < 0) {
                                EdgeWithDirection vertical_edge = EdgeWithDirection();
                                vertical_edge.top = top;
                                vertical_edge.bottom = inter_width;
                                vertical_edge.slope  = 0;
                                vertical_edge.offset = width;
                                vertical_edge.goingUp = goingUp;
                                GEdges.push_back(vertical_edge);
                                top = inter_width;
                        } else {
                            EdgeWithDirection vertical_edge = EdgeWithDirection();
                            vertical_edge.top = inter_width;
                            vertical_edge.bottom = bottom;
                            vertical_edge.slope  = 0;
                            vertical_edge.offset = width;
                            vertical_edge.goingUp = goingUp;
                            GEdges.push_back(vertical_edge);
                            bottom = inter_width;
                        }
                    }
                }
            } 
            if(hasPrev) {
                EdgeWithDirection& prev_edge = GEdges[prev_index];
                if(prev_edge.offset == offset && prev_edge.slope == slope && prev_edge.goingUp == goingUp) {
                    if(goingUp == 1) {
                        if(prev_edge.bottom == top)
                            prev_edge.bottom = bottom;
                    } else {
                        if(prev_edge.top == bottom)
                            prev_edge.top = top;
                    }
                    return;
                } 
            }
            EdgeWithDirection new_edge = EdgeWithDirection();
            new_edge.slope = slope;
            new_edge.offset = offset;
            new_edge.top = top;
            new_edge.bottom = bottom;
            assert(top < bottom);

            new_edge.goingUp = goingUp;
            GEdges.push_back(new_edge);

        }
}



static inline void connectTwoPointsBounded(std::vector<EdgeWithDirection> &GEdges, const GPoint& pt_1, const GPoint& pt_2) {
        const float& lefty = pt_1.y;
        const float& righty = pt_2.y;
        float leftx = pt_1.x;
        float rightx = pt_2.x;

        int rounded_lefty = GRoundToIntNonNegative(lefty);
        int rounded_righty = GRoundToIntNonNegative(righty);

        if(!(rounded_lefty == rounded_righty)) {
            int goingUp; 
            if(rounded_lefty < rounded_righty) {
                goingUp = 1;
            }
            else { 
                goingUp = -1;
            }
                
            EdgeWithDirection new_edge = EdgeWithDirection();
            // slope and offset
            float delta_x = leftx - rightx;
            float delta_y = lefty - righty;
            float slope = delta_x / delta_y;
            float offset = leftx - slope * lefty;
            new_edge.slope = slope;
            new_edge.offset = offset;
            // left intersection and excess
            int top = std::min(rounded_lefty, rounded_righty);
            int bottom = std::max(rounded_lefty, rounded_righty);

            new_edge.top = top;
            new_edge.bottom = bottom;
            assert(top < bottom);
            new_edge.goingUp = goingUp;
            GEdges.push_back(new_edge);
        }
}

// void recursion_quad(GPoint& A, GPoint& B, GPoint& C, int remaining_segments, std::vector<EdgeWithDirection>& edges, int width, int height) {
//     assert(remaining_segments != 0);
//     if(remaining_segments == 1) {
//         connectTwoPoints(edges, A, C, width, height);
//         return;
//     }
//     GPoint new_B_left = (A + B) * 0.5;
//     GPoint P = (A + 2 * B + C) * 0.25;
//     GPoint new_B_right = (B + C) * 0.5;
//     recursion_quad(A, new_B_left, P, remaining_segments >> 1, edges, width, height);
//     recursion_quad(P, new_B_right, C, remaining_segments >> 1, edges, width, height);
// }

// void recursion_cubic(GPoint& A, GPoint& B, GPoint& C, GPoint& D, int remaining_segments, std::vector<EdgeWithDirection>& edges, int width, int height) {
//     assert(remaining_segments != 0);
//     if(remaining_segments == 1) {
//         connectTwoPoints(edges, A, D, width, height);
//         return;
//     }
//     GPoint new_B_left = (A + B) * 0.5;
//     GPoint new_C_left = (A + 2 * B + C) * 0.25;
//     GPoint P = (A + 3 * B + 3 * C + D) * 0.125;
//     GPoint new_B_right = (B + 2 * C + D) * 0.25;
//     GPoint new_C_right = (C + D) * 0.5;

//     recursion_cubic(A, new_B_left, new_C_left, P, remaining_segments >> 1, edges, width, height);
//     recursion_cubic(P, new_B_right, new_C_right , D, remaining_segments >> 1, edges, width, height);
// }

#endif