/*
 *  Copyright 2024 <me>
 */

#ifndef _g_shader_proxy_h_
#define _g_shader_proxy_h_

#include "include/GShader.h"
#include "include/GRect.h"
#include "include/GPixel.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"


class ProxyShader : public GShader {
public:
    ProxyShader(const GPoint triangle[3],  const GPoint tex[3], GShader* originalShader) {
        GMatrix T = GMatrix(triangle[1] - triangle[0], triangle[2] - triangle[0], triangle[0]);
        GMatrix P = GMatrix(tex[1] - tex[0], tex[2] - tex[0], tex[0]);
        if (auto inv = P.invert()) {
            this->fLocalMatrix = nonstd::optional<GMatrix>{T * (*inv)};
        } else {
            this->fLocalMatrix = nonstd::nullopt;
        }

        this->opaqueness = false;
        this->originalShader = originalShader;
    }
    bool isOpaque() override;
    bool setContext(const GMatrix& ctm) override;
    void shadeRow(int x, int y, int count, GPixel row[]) override;
    


private:
    nonstd::optional<GMatrix> fLocalMatrix; 
    bool opaqueness;
    GShader* originalShader;
};


std::shared_ptr<GShader> GCreateProxyShader(const GPoint triangle[3], const GPoint tex[3], GShader* originalShader);



#endif