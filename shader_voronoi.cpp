#include "shader_voronoi.h"
#include "utils.h"
#include <math.h>
#include <iostream>


bool VoronoiShader::isOpaque() {
    return false;
}

bool VoronoiShader::setContext(const GMatrix& ctm) {
    if (auto inv = (ctm * fLocalMatrix).invert()) {
        fInverse = *inv;
        return true;
    }
    return false;
}

void VoronoiShader::shadeRow(int x, int y, int count, GPixel row[]) {
    const auto step = fInverse.e0();

    GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
    float distance;
    for(int i = 0; i < count; i++) {
        float min = INFINITY; 
        int min_index = -1;
        for(int j = 0; j < this->num_colors; j++) {
            float x_point = this->points[j].x;
            float y_point = this->points[j].y;
            distance = (x_point - loc.x) * (x_point - loc.x) + (y_point - loc.y) * (y_point - loc.y);
            if(distance < min) {
                min = distance;
                min_index = j;
            }    
        }
        GColor color = this->colors[min_index];
        const int a = (int)(color.a * 255 + 0.5f);
        const int r = (int)(a * color.r + 0.5f);
        const int g = (int)(a * color.g + 0.5f);
        const int b = (int)(a * color.b + 0.5f);
        row[i] = GPixel_PackARGB(a,r,g,b);
        loc += step;
    }
}