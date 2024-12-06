/*
 *  Copyright 2024 <me>
 */

#ifndef _g_shader_voronoi_h_
#define _g_shader_voronoi_h_

#include "include/GShader.h"
#include "include/GRect.h"
#include "include/GPixel.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class VoronoiShader : public GShader {
public:
    VoronoiShader(const GPoint points[], const GColor colors[], int count) 
    {
        for(int i = 0; i < count; i++) {
            this->points.push_back(points[i]);
            this->colors.push_back(colors[i]);
        }
        this->num_colors = count;

        this->fLocalMatrix = GMatrix();
    }
    bool isOpaque() override;
    bool setContext(const GMatrix& ctm) override;
    void shadeRow(int x, int y, int count, GPixel row[]) override;




private:
    std::vector<GPoint> points;
    std::vector<GColor> colors;
    int num_colors;

    GMatrix fLocalMatrix; 
    GMatrix fInverse;
};



#endif