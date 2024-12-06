/*
 *  Copyright 2024 <me>
 */

#include "shader_combined.h"
#include "utils.h"
#include <vector>
#include <iostream>

bool CombinedShader::isOpaque() {
    return this->opaqueness;
}

bool CombinedShader::setContext(const GMatrix& ctm) {
    if(this->fLocalMatrixProxy.has_value()) {
        this->originalShader->setContext(ctm * fLocalMatrixProxy.value());
        if(auto inv = (ctm * fLocalMatrix).invert()) {
            this->fInverse = *inv;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void CombinedShader::shadeRow(int x, int y, int count, GPixel row[]) {
    this->originalShader->shadeRow(x, y, count, row);

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
            GPixel& src = row[i];
            const int a = (int)(start_color.a * GPixel_GetA(src) + 0.5f);
            const int r = (int)(start_color.a * start_color.r * GPixel_GetR(src) + 0.5f);
            const int g = (int)(start_color.a * start_color.g * GPixel_GetG(src) + 0.5f);
            const int b = (int)(start_color.a * start_color.b * GPixel_GetB(src) + 0.5f);
            // std::cout << a << "~~~" << r << "~~~" << g << "~~~" << b << std::endl;
            row[i] = GPixel_PackARGB(a,r,g,b);
            start_color += color_diff;
    }
}

std::shared_ptr<GShader> GCreateCombinedShader(const GPoint triangle[3],  const GPoint tex[3], const GColor colors[3], GShader* originalShader) {
    std::shared_ptr<CombinedShader> sp = std::make_shared<CombinedShader>(triangle, tex, colors, originalShader);

    return sp;
}