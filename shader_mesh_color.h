/*
 *  Copyright 2024 <me>
 */

#ifndef _g_shader_mesh_triangle_h_
#define _g_shader_mesh_triangle_h_

#include "include/GShader.h"
#include "include/GRect.h"
#include "include/GPixel.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"


class MeshColorShader : public GShader {
public:
    MeshColorShader(const GPoint triangle[3], const GColor colors[3]) {
        this->fLocalMatrix = GMatrix(triangle[1] - triangle[0], triangle[2] - triangle[0], triangle[0]);
        this->colors[0] = colors[0];
        this->colors[1] = colors[1];
        this->colors[2] = colors[2];
        if(colors[0].a == 1 && colors[1].a == 1 && colors[2].a == 1) {
            this->opaqueness = true;
        } else {
            this->opaqueness = false;
        }
    }

    bool isOpaque() override;
    bool setContext(const GMatrix& ctm) override;
    void shadeRow(int x, int y, int count, GPixel row[]) override;
    


private:
    GMatrix fLocalMatrix; 
    GMatrix fInverse; 
    bool opaqueness;
    GColor colors[3];
};


std::shared_ptr<GShader> GCreateMeshColorShader(const GPoint triangle[3], const GColor colors[3]);


#endif