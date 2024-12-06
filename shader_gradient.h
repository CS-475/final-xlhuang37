/*
 *  Copyright 2024 <me>
 */

#ifndef _g_shader_gradient_h_
#define _g_shader_gradient_h_

#include "include/GShader.h"
#include "include/GRect.h"
#include "include/GPixel.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class GradientShader : public GShader {
public:
    GradientShader(std::vector<GPixel> pixels, std::vector<GColor> colors, std::vector<GColor> colors_diff, GMatrix ctm, bool opaqueness, int numPixels, GTileMode tileMode) 
    : pixels(std::move(pixels)),
        colors(std::move(colors)),
        colors_diff(std::move(colors_diff)),
        fLocalMatrix(ctm),
        opaqueness(opaqueness),
        numPixels(numPixels),
        tileMode(tileMode)
    {
        if(this->opaqueness) {
            switch(tileMode) {
                case GTileMode::kClamp:
                this->shadeRowPtr = &GradientShader::shadeRowOpaqueClamp;
                break;
                case GTileMode::kMirror:
                this->shadeRowPtr = &GradientShader::shadeRowOpaqueMirror;
                break;
                case GTileMode::kRepeat:
                this->shadeRowPtr = &GradientShader::shadeRowOpaqueRepeat;
                break;
                default:
                assert(0);
            }
        } else {
            switch(tileMode) {
                case GTileMode::kClamp:
                this->shadeRowPtr = &GradientShader::shadeRowNonOpaqueClamp;
                break;
                case GTileMode::kMirror:
                this->shadeRowPtr = &GradientShader::shadeRowNonOpaqueMirror;
                break;
                case GTileMode::kRepeat:
                this->shadeRowPtr = &GradientShader::shadeRowNonOpaqueMirror;
                break;
                default:
                assert(0);
            }
        }
    }
    bool isOpaque() override;
    bool setContext(const GMatrix& ctm) override;
    void shadeRow(int x, int y, int count, GPixel row[]) override;
    void shadeRowNonOpaqueClamp (int x, int y, int count, GPixel row[]);
    void shadeRowOpaqueClamp (int x, int y, int count, GPixel row[]);
    void shadeRowNonOpaqueMirror (int x, int y, int count, GPixel row[]);
    void shadeRowOpaqueMirror (int x, int y, int count, GPixel row[]);
    void shadeRowNonOpaqueRepeat (int x, int y, int count, GPixel row[]);
    void shadeRowOpaqueRepeat (int x, int y, int count, GPixel row[]);

    


private:
    std::vector<GPixel> pixels; 
    std::vector<GColor> colors; 
    std::vector<GColor> colors_diff;
    GMatrix fLocalMatrix; 
    GMatrix fInverse;
    bool opaqueness;
    int numPixels;
    GTileMode tileMode;
    

    void (GradientShader::*shadeRowPtr)(int x, int y, int count, GPixel row[]);
};



#endif