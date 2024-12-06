/*
 *  Copyright 2024 <me>
 */

#ifndef _g_shader_sweep_h_
#define _g_shader_sweep_h_

#include "include/GShader.h"
#include "include/GRect.h"
#include "include/GPixel.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class SweepShader : public GShader {
public:
    SweepShader(GPoint center, float startRadians,
                                                         const GColor colors[], int count) 
    {
        for(int i = 0; i < count; i++) {
            this->colors.push_back(colors[i]);
        }

        this->center = center;
        this->startRadians = startRadians;
        this->num_colors = count;  
        this->fLocalMatrix = GMatrix();
    }
    bool isOpaque() override;
    bool setContext(const GMatrix& ctm) override;
    void shadeRow(int x, int y, int count, GPixel row[]) override;




private:
    GPoint center;
    float startRadians;
    std::vector<GColor> colors;
    int num_colors;

    GMatrix fLocalMatrix; 
    GMatrix fInverse;
};



#endif