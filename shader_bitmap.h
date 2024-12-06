/*
 *  Copyright 2024 <me>
 */

#ifndef _g_shader_bitmap_h_
#define _g_shader_bitmap_h_

#include "include/GShader.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class LongShader : public GShader {
public:
    LongShader(const GBitmap& bitmap, GMatrix ctm, GTileMode tileMode) {
        this->bitmap = bitmap;
        this->fLocalMatrix = ctm;
        this->tileMode = tileMode;
        switch(tileMode) {
            case GTileMode::kClamp:
            this->shadeRowPtr = &LongShader::shadeRowClamp;
            break;
            case GTileMode::kMirror:
            this->shadeRowPtr = &LongShader::shadeRowMirror;
            break;
            case GTileMode::kRepeat:
            this->shadeRowPtr = &LongShader::shadeRowRepeat;
            break;
            default:
            assert(0);
            break;
        }

        // set opaqueness
        int width = this->bitmap.width();
        int height = this->bitmap.height();
        int real_width = this->bitmap.rowBytes() >> 2;
        bool result = true;
        GPixel* pix_start = this->bitmap.pixels();
        for(int j = 0; j < height; j++) {
            GPixel* pix = pix_start + j * real_width;
            for(int i = 0; i < width; i++) {
                result = (GPixel_GetA(*pix) == 0xFF) && result;
                pix++;
            }
        }
        this->opaqueness = result;
    }
    bool isOpaque() override;
    bool setContext(const GMatrix& ctm) override;
    void shadeRow(int x, int y, int count, GPixel row[]) override;
    void shadeRowClamp(int x, int y, int count, GPixel row[]);
    void shadeRowMirror(int x, int y, int count, GPixel row[]);
    void shadeRowRepeat(int x, int y, int count, GPixel row[]);


private:
    GBitmap bitmap;
    GTileMode tileMode;
    bool opaqueness;
    void (LongShader::*shadeRowPtr)(int x, int y, int count, GPixel row[]);


    GMatrix fLocalMatrix;
    GMatrix fInverse;
};



#endif