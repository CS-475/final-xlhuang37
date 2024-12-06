/*
 *  Copyright 2024 <me>
 */

#include "shader_mesh_color.h"
#include "utils.h"
#include <iostream>

bool MeshColorShader::isOpaque() {
    return this->opaqueness;
}

bool MeshColorShader::setContext(const GMatrix& ctm) {
    if (auto inv = (ctm * fLocalMatrix).invert()) {
        fInverse = *inv;
        return true;
    }
    return false;
}

void MeshColorShader::shadeRow(int x, int y, int count, GPixel row[]) {
    const auto step = fInverse.e0();
    GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
    GColor diff_color_1 = this->colors[1] - this->colors[0];
    GColor diff_color_2 = this->colors[2] - this->colors[0];
    GColor start_color = loc.x * diff_color_1
                            + loc.y * diff_color_2
                            + this->colors[0];
    GColor color_diff = step.x * diff_color_1 + step.y * diff_color_2;

        for(int i = 0; i < count; i++) {
            //  std::cout << start_color.a << "~~~" << start_color.r << "~~~" << start_color.g << "~~~" << start_color.b << std::endl;
            const int a = (int)(start_color.a * 255 + 0.5f);
            const int r = (int)(a * start_color.r + 0.5f);
            const int g = (int)(a * start_color.g + 0.5f);
            const int b = (int)(a * start_color.b + 0.5f);
            // std::cout << a << "~~~" << r << "~~~" << g << "~~~" << b << std::endl;
            row[i] = GPixel_PackARGB(a,r,g,b);
            start_color += color_diff;
        }

}


std::shared_ptr<GShader> GCreateMeshColorShader(const GPoint triangle[3], const GColor colors[3]) {

    std::shared_ptr<MeshColorShader> sp = std::make_shared<MeshColorShader>(triangle, colors);

    return sp;
}
