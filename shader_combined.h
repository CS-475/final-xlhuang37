/*
 *  Copyright 2024 <me>
 */

#ifndef _g_shader_combined_h_
#define _g_shader_combined_h_

#include "include/GShader.h"
#include "include/GRect.h"
#include "include/GPixel.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"


class CombinedShader : public GShader {
public:
    CombinedShader(const GPoint triangle[3],  const GPoint tex[3], const GColor colors[3], GShader* originalShader) {
        this->colors[0] = colors[0];
        this->colors[1] = colors[1];
        this->colors[2] = colors[2];

        this->fLocalMatrix = GMatrix(triangle[1] - triangle[0], triangle[2] - triangle[0], triangle[0]);
        GMatrix P = GMatrix(tex[1] - tex[0], tex[2] - tex[0], tex[0]);
        if (auto inv = P.invert()) {
            this->fLocalMatrixProxy = nonstd::optional<GMatrix>{this->fLocalMatrix * (*inv)};
        } else {
            this->fLocalMatrixProxy = nonstd::nullopt;
        }

        this->opaqueness = false;
        this->originalShader = originalShader;
    }
    bool isOpaque() override;
    bool setContext(const GMatrix& ctm) override;
    void shadeRow(int x, int y, int count, GPixel row[]) override;
    


private:
    GShader* color_shader;
    GShader* texture_shader;

    GMatrix fLocalMatrix; 
    GMatrix fInverse; 
    bool opaqueness;
    bool multiply;
    GColor colors[3];

    nonstd::optional<GMatrix> fLocalMatrixProxy; 
    GShader* originalShader;
};


std::shared_ptr<GShader> GCreateCombinedShader(const GPoint triangle[3],  const GPoint tex[3], const GColor colors[3], GShader* originalShader);


#endif