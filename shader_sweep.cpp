#include "shader_sweep.h"
#include "utils.h"
#include <math.h>
#include <iostream>


bool SweepShader::isOpaque() {
    return false;
}

bool SweepShader::setContext(const GMatrix& ctm) {
    if (auto inv = (ctm * fLocalMatrix).invert()) {
        fInverse = *inv;
        return true;
    }
    return false;
}

void SweepShader::shadeRow(int x, int y, int count, GPixel row[]) {
    if(this->num_colors == 1) {
        for(int i = 0; i < count; i++) {
            assert(0);
        }
    }
    const auto step = fInverse.e0();
    GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
    for(int i = 0; i < count; i++) {
        float distance_x = loc.x - center.x;
        float distance_y = loc.y - center.y;
        float radian = atan2(distance_y, distance_x);
        radian -= startRadians;
        if(radian < 0) {
            radian = 2 * M_PI + radian;
        }
        radian = radian * 180 / M_PI;
        
        float quotient = radian / (360 / (this->num_colors - 1));
        int left_index = floorf(quotient);
        int right_index = (left_index + 1) % this->num_colors;
        float t = quotient - left_index;
        GColor left_color = this->colors[left_index];
        GColor right_color = this->colors[right_index];
        
        GColor new_color = left_color * (1 - t) + right_color * t;
        const int a = (int)(new_color.a * 255 + 0.5f);
        const int r = (int)(a * new_color.r + 0.5f);
        const int g = (int)(a * new_color.g + 0.5f);
        const int b = (int)(a * new_color.b + 0.5f);
        row[i] = GPixel_PackARGB(a,r,g,b);
        loc += step;
        // if(distance_y < 0) {
        //     rad
        // }
        // GColor color = this->colors[min_index];
        // const int a = (int)(color.a * 255 + 0.5f);
        // const int r = (int)(a * color.r + 0.5f);
        // const int g = (int)(a * color.g + 0.5f);
        // const int b = (int)(a * color.b + 0.5f);
        // row[i] = GPixel_PackARGB(a,r,g,b);
        // loc += step;
    }
}