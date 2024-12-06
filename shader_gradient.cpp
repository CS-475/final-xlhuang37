/*
 *  Copyright 2024 <me>
 */

#include "shader_gradient.h"
#include "utils.h"
#include <iostream>

bool GradientShader::isOpaque() {
    return this->opaqueness;
}

bool GradientShader::setContext(const GMatrix& ctm) {
    if (auto inv = (ctm * fLocalMatrix).invert()) {
        fInverse = *inv;
        return true;
    }
    return false;
}

void GradientShader::shadeRow(int x, int y, int count, GPixel row[]) {
    (this->*shadeRowPtr)(x, y, count, row); 
}

void GradientShader::shadeRowOpaqueClamp(int x, int y, int count, GPixel row[]) {
        if(this->numPixels == 1) {
            GPixel pix = this->pixels[0];
            for(int i = 0; i < count; i++) {
                row[i] = pix;
            }
            return;
        } 
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        int max_index = this->numPixels - 1;

        if (this->numPixels == 2) {
            float extended_x = loc.x; 
            float extended_step = step.x;
            GPixel left_pix = this->pixels[0];
            GPixel right_pix = this->pixels[1];
            GColor left_color = this->colors[0];
            GColor color_diff = this->colors_diff[0];
            for(int i = 0; i < count; i++) {
                if(extended_x > max_index) {
                    row[i] = right_pix;
                } else if (extended_x < 0) {
                    row[i] = left_pix;
                } else {
                    float t = extended_x;
                    GColor new_color = left_color + color_diff * t;
                    const int a = 255;
                    const int r = (int)(new_color.r * 255 + 0.5f);
                    const int g = (int)(new_color.g * 255 + 0.5f);
                    const int b = (int)(new_color.b * 255 + 0.5f);
                    row[i] = GPixel_PackARGB(a,r,g,b);
                }
                extended_x += extended_step;
            }
            
            return;
        }

        float extended_x = loc.x * max_index; 
        float extended_step = step.x * max_index;

        for(int i = 0; i < count; i++) {
            if(extended_x > max_index) {
                row[i] = this->pixels[max_index];
            } else if (extended_x < 0) {
                row[i] = this->pixels[0];
            } else {
                int floor =  GFloorToIntNonNegative(extended_x);
                float t = extended_x - floor;
                const GColor& left_color = this->colors[floor];
                const GColor& color_diff = this->colors_diff[floor];
                float fr =    left_color.r + color_diff.r * t;
                float fg =    left_color.g + color_diff.g * t;
                float fb =    left_color.b + color_diff.b * t;
                const int a = 255;
                const int r = (int)(fr * 255 + 0.5f);
                const int g = (int)(fg * 255 + 0.5f);
                const int b = (int)(fb * 255 + 0.5f);
                row[i] = GPixel_PackARGB(a,r,g,b);
            }

            extended_x += extended_step;
        }
}

void GradientShader::shadeRowNonOpaqueClamp(int x, int y, int count, GPixel row[]) {
        if(this->numPixels == 1) {
            GPixel pix = this->pixels[0];
            for(int i = 0; i < count; i++) {
                row[i] = pix;
            }
            return;
        } 
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        int max_index = this->numPixels - 1;

        if (this->numPixels == 2) {
            float extended_x = loc.x; 
            float extended_step = step.x;
            GPixel left_pix = this->pixels[0];
            GPixel right_pix = this->pixels[1];
            GColor left_color = this->colors[0];
            GColor color_diff = this->colors_diff[0];
            for(int i = 0; i < count; i++) {
                if(extended_x > max_index) {
                    row[i] = right_pix;
                } else if (extended_x < 0) {
                    row[i] = left_pix;
                } else {
                    float t = extended_x;
                    GColor new_color = left_color + color_diff * t;
                    const int a = (int)(new_color.a * 255 + 0.5f);
                    const int r = (int)(a * new_color.r + 0.5f);
                    const int g = (int)(a * new_color.g + 0.5f);
                    const int b = (int)(a * new_color.b + 0.5f);
                    row[i] = GPixel_PackARGB(a,r,g,b);
                }
                extended_x += extended_step;
            }
            
            return;
        }

        float extended_x = loc.x * max_index; 
        float extended_step = step.x * max_index;

        for(int i = 0; i < count; i++) {
            if(extended_x > max_index) {
                row[i] = this->pixels[max_index];
            } else if (extended_x < 0) {
                row[i] = this->pixels[0];
            } else {
                int floor = GFloorToIntNonNegative(extended_x);
                float t = extended_x - floor;
                const GColor& left_color = this->colors[floor];
                const GColor& color_diff = this->colors_diff[floor];
                float fa =    left_color.a + color_diff.a * t;
                float fr =    left_color.r + color_diff.r * t;
                float fg =    left_color.g + color_diff.g * t;
                float fb =    left_color.b + color_diff.b * t;
                const int a = (int)(fa * 255 + 0.5f);
                const int r = (int)(fa * fr * 255 + 0.5f);
                const int g = (int)(fa * fg * 255 + 0.5f);
                const int b = (int)(fa * fb * 255 + 0.5f);
                row[i] = GPixel_PackARGB(a,r,g,b);
            }

            extended_x += extended_step;
        }
}

void GradientShader::shadeRowNonOpaqueMirror (int x, int y, int count, GPixel row[]) {
        if(this->numPixels == 1) {
            GPixel pix = this->pixels[0];
            for(int i = 0; i < count; i++) {
                row[i] = pix;
            }
            return;
        } 
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        int max_index = this->numPixels - 1;

        if (this->numPixels == 2) {
            float extended_x = loc.x; 
            float extended_step = step.x;
            GColor left_color = this->colors[0];
            GColor color_diff = this->colors_diff[0];
            for(int i = 0; i < count; i++) {
                float quotient = extended_x * 0.5f;
                float remainder = quotient - floorf(quotient);
                float t;
                if(remainder * 2 >= 1) {
                    t = 2 - (remainder * 2);
                } else {
                    t = remainder * 2;
                }
                GColor new_color = left_color + color_diff * t;
                const int a = (int)(new_color.a * 255 + 0.5f);
                const int r = (int)(a * new_color.r + 0.5f);
                const int g = (int)(a * new_color.g + 0.5f);
                const int b = (int)(a * new_color.b + 0.5f);
                row[i] = GPixel_PackARGB(a,r,g,b);
                extended_x += extended_step;
            }
            
            return;
        }


        float extended_x = loc.x * max_index; 
        float extended_step = step.x * max_index;
        float inv_width = 1.0f / max_index / 2;

        for(int i = 0; i < count; i++) {
            
            // 23.5
            float quotient = extended_x * inv_width;
            // 2.35 - 2 = 0.35
            float remainder = quotient - floorf(quotient);
            float x = (-fabs((remainder * max_index * 2 - max_index)) + max_index - 0.001f);
            int floor = GFloorToIntNonNegative(x);
            float t = x - floor;
            assert(floor >= 0);

                const GColor& left_color = this->colors[floor];
                const GColor& color_diff = this->colors_diff[floor];
                float fa =    left_color.a + color_diff.a * t;
                float fr =    left_color.r + color_diff.r * t;
                float fg =    left_color.g + color_diff.g * t;
                float fb =    left_color.b + color_diff.b * t;
                const int a = (int)(fa * 255 + 0.5f);
                const int r = (int)(fa * fr * 255 + 0.5f);
                const int g = (int)(fa * fg * 255 + 0.5f);
                const int b = (int)(fa * fb * 255 + 0.5f);
                row[i] = GPixel_PackARGB(a,r,g,b);

            extended_x += extended_step;
        }
}


void GradientShader::shadeRowOpaqueMirror (int x, int y, int count, GPixel row[]) {
        if(this->numPixels == 1) {
            GPixel pix = this->pixels[0];
            for(int i = 0; i < count; i++) {
                row[i] = pix;
            }
            return;
        } 
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        int max_index = this->numPixels - 1;
        float end_x = x + step.x * (count - 1);
        bool startXInUnit = false;
        bool endXInUnit = false;
        if(x >= 0 && x <= 1) {
            startXInUnit = true;
        }
        if(end_x >= 0 && end_x <= 1) {
            endXInUnit = true;
        }

        if (this->numPixels == 2) {
            if(startXInUnit && endXInUnit) {
                // Idea Borrowed from Sam
                GColor& left_color = this->colors[0];
                GColor& color_diff = this->colors_diff[0];     
                GColor color_step = color_diff * step.x;
                GColor curr_color = left_color;
                for(int i = 0; i < count; i++) {
                    const int a = 255;
                    const int r = (int)(curr_color.r * 255 + 0.5f);
                    const int g = (int)(curr_color.g * 255 + 0.5f);
                    const int b = (int)(curr_color.b * 255 + 0.5f);
                    row[i] = GPixel_PackARGB(a,r,g,b);
                    curr_color += color_step;
                }
            } else {
                float extended_x = loc.x; 
                float extended_step = step.x;
                GColor left_color = this->colors[0];
                GColor color_diff = this->colors_diff[0];
                for(int i = 0; i < count; i++) {
                    int floor = GFloorToIntNonNegative(extended_x);
                    float t = extended_x - floor;
                    t = fabs((floor & 1) - t);
                    GColor new_color = left_color + color_diff * t;
                    const int a = 255;
                    const int r = (int)(new_color.r * 255 + 0.5f);
                    const int g = (int)(new_color.g * 255 + 0.5f);
                    const int b = (int)(new_color.b * 255 + 0.5f);
                    row[i] = GPixel_PackARGB(a,r,g,b);
                    extended_x += extended_step;
                }
            }
            
            return;
        }


        float extended_x = loc.x * max_index; 
        float extended_step = step.x * max_index;
        float inv_width = 1.0f / max_index / 2;

        for(int i = 0; i < count; i++) {
            
            // 23.5
            float quotient = extended_x * inv_width;
            // 2.35 - 2 = 0.35
            float remainder = myQuotientMinusFloorf(quotient);
            float x = (-fabs((remainder * max_index * 2 - max_index)) + max_index - 0.001f);
            int floor = GFloorToIntNonNegative(x);
            float t = x - floor;
            assert(floor >= 0);
            
            const GColor& left_color = this->colors[floor];
            const GColor& color_diff = this->colors_diff[floor];
            float fr =    left_color.r + color_diff.r * t;
            float fg =    left_color.g + color_diff.g * t;
            float fb =    left_color.b + color_diff.b * t;
            const int a = 255;
            const int r = (int)(fr * 255 + 0.5f);
            const int g = (int)(fg * 255 + 0.5f);
            const int b = (int)(fb * 255 + 0.5f);
            row[i] = GPixel_PackARGB(a,r,g,b);

            extended_x += extended_step;
        }
}

void GradientShader::shadeRowNonOpaqueRepeat (int x, int y, int count, GPixel row[]) {
        if(this->numPixels == 1) {
            GPixel pix = this->pixels[0];
            for(int i = 0; i < count; i++) {
                row[i] = pix;
            }
            return;
        } 
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        int max_index = this->numPixels - 1;

        if (this->numPixels == 2) {
            float extended_x = loc.x; 
            float extended_step = step.x;
            GColor left_color = this->colors[0];
            GColor color_diff = this->colors_diff[0];
            for(int i = 0; i < count; i++) {
                float t = extended_x - floorf(extended_x);
                GColor new_color = left_color + color_diff * t;
                const int a = (int)(new_color.a * 255 + 0.5f);
                const int r = (int)(a * new_color.r + 0.5f);
                const int g = (int)(a * new_color.g + 0.5f);
                const int b = (int)(a * new_color.b + 0.5f);
                row[i] = GPixel_PackARGB(a,r,g,b);
                extended_x += extended_step;
            }
            
            return;
        }


        float extended_x = loc.x * max_index; 
        float extended_step = step.x * max_index;
        float inv_width = 1.0f / max_index;

        for(int i = 0; i < count; i++) {
            float quotient = extended_x * inv_width;
            float remainder = quotient - floorf(quotient);
            float x = remainder * max_index;
            int floor = GFloorToIntNonNegative(x);
            float t = x - floor;
            
                const GColor& left_color = this->colors[floor];
                const GColor& color_diff = this->colors_diff[floor];
                float fa =    left_color.a + color_diff.a * t;
                float fr =    left_color.r + color_diff.r * t;
                float fg =    left_color.g + color_diff.g * t;
                float fb =    left_color.b + color_diff.b * t;
                const int a = (int)(fa * 255 + 0.5f);
                const int r = (int)(fa * fr * 255 + 0.5f);
                const int g = (int)(fa * fg * 255 + 0.5f);
                const int b = (int)(fa * fb * 255 + 0.5f);
            row[i] = GPixel_PackARGB(a,r,g,b);

            extended_x += extended_step;
        }
}

void GradientShader::shadeRowOpaqueRepeat (int x, int y, int count, GPixel row[]) {
        if(this->numPixels == 1) {
            GPixel pix = this->pixels[0];
            for(int i = 0; i < count; i++) {
                row[i] = pix;
            }
            return;
        } 
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        int max_index = this->numPixels - 1;
        float end_x = x + step.x * (count - 1);
        bool startXInUnit = false;
        bool endXInUnit = false;
        if(x >= 0 && x <= 1) {
            startXInUnit = true;
        }
        if(end_x >= 0 && end_x <= 1) {
            endXInUnit = true;
        }

        if (this->numPixels == 2) {
            if(startXInUnit && endXInUnit) {
                // Idea Borrowed from Sam
                GColor& left_color = this->colors[0];
                GColor& color_diff = this->colors_diff[0];     
                GColor color_step = color_diff * step.x;
                GColor curr_color = left_color;
                for(int i = 0; i < count; i++) {
                    const int a = 255;
                    const int r = (int)(curr_color.r * 255 + 0.5f);
                    const int g = (int)(curr_color.g * 255 + 0.5f);
                    const int b = (int)(curr_color.b * 255 + 0.5f);
                    row[i] = GPixel_PackARGB(a,r,g,b);
                    curr_color += color_step;
                }
            } else {
                float extended_x = loc.x; 
                float extended_step = step.x;
                GColor& left_color = this->colors[0];
                GColor& color_diff = this->colors_diff[0];
                for(int i = 0; i < count; i++) {
                    int floor = GFloorToIntNonNegative(extended_x);
                    float t = extended_x - floor;
                    t = fabs(returnSignBit(floor) - t);
                    
                    GColor new_color = left_color + color_diff * t;
                    const int a = 255;
                    const int r = (int)(new_color.r * 255 + 0.5f);
                    const int g = (int)(new_color.g * 255 + 0.5f);
                    const int b = (int)(new_color.b * 255 + 0.5f);
                    row[i] = GPixel_PackARGB(a,r,g,b);
                    extended_x += extended_step;
                }
            }
            return;
        }


        float extended_x = loc.x * max_index; 
        float extended_step = step.x * max_index;
        float inv_width = 1.0f / max_index;

        for(int i = 0; i < count; i++) {
            float quotient = extended_x * inv_width;
            float remainder = quotient - floorf(quotient);
            float x = remainder * max_index;
            int floor = GFloorToIntNonNegative(x);
            float t = x - floor;
            
            const GColor& left_color = this->colors[floor];
            const GColor& color_diff = this->colors_diff[floor];
            float fr =    left_color.r + color_diff.r * t;
            float fg =    left_color.g + color_diff.g * t;
            float fb =    left_color.b + color_diff.b * t;
            const int a = 255;
            const int r = (int)(fr * 255 + 0.5f);
            const int g = (int)(fg * 255 + 0.5f);
            const int b = (int)(fb * 255 + 0.5f);
            row[i] = GPixel_PackARGB(a,r,g,b);

            extended_x += extended_step;
        }
}







std::shared_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode tileMode) {
    bool isOpaque = true;
   
    if(count < 1) {
        return nullptr;
    } 

    GMatrix map_to_gradient = GMatrix(
        p1.x - p0.x, 
        p0.y - p1.y, 
        p0.x, 
        p1.y - p0.y, 
        p1.x - p0.x, 
        p0.y
    );

    std::vector<GPixel> new_pixels;
    new_pixels.reserve(count);
    std::vector<GColor> new_colors;
    new_colors.reserve(count);
    std::vector<GColor> new_colors_diff;
    new_colors_diff.reserve(count);

    const int a = (int)(colors[0].a * 255 + 0.5f);
    if(a != 255)
        isOpaque = false;
    const int r = (int)(colors[0].a * colors[0].r * 255 + 0.5f);
    const int g = (int)(colors[0].a * colors[0].g * 255 + 0.5f);
    const int b = (int)(colors[0].a * colors[0].b * 255 + 0.5f);
    new_pixels.push_back(GPixel_PackARGB(a,r,g,b));
    new_colors.push_back(colors[0]);
    for(int i = 1; i < count; i++) {
        const int a = (int)(colors[i].a * 255 + 0.5f);
        if(a != 255)
            isOpaque = false;
        const int r = (int)(colors[i].a * colors[i].r * 255 + 0.5f);
        const int g = (int)(colors[i].a * colors[i].g * 255 + 0.5f);
        const int b = (int)(colors[i].a * colors[i].b * 255 + 0.5f);
        new_pixels.push_back(GPixel_PackARGB(a,r,g,b));
        new_colors.push_back(colors[i]);
        new_colors_diff.push_back(colors[i] - colors[i - 1]);
    }
    new_colors_diff.push_back(colors[0] - colors[count - 1]);

    std::shared_ptr<GradientShader> sp = std::make_shared<GradientShader>(new_pixels, new_colors, new_colors_diff,  
                                            map_to_gradient, isOpaque, count, tileMode);

    return sp;
}