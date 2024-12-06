/*
 *  Copyright 2024 <me>
 */

#include "shader_bitmap.h"
#include "utils.h"
#include <vector>
#include <iostream>

bool LongShader::isOpaque() {
    return this->opaqueness;
}

bool LongShader::setContext(const GMatrix& ctm) {
    if (auto inv = (ctm * fLocalMatrix).invert()) {
        fInverse = *inv;
        return true;
    }
    return false;
}

void LongShader::shadeRow(int x, int y, int count, GPixel row[]) {
    (this->*shadeRowPtr)(x, y, count, row);
}

void LongShader::shadeRowClamp(int x, int y, int count, GPixel row[]) {
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        int width = this->bitmap.width();
        int height = this->bitmap.height();
        int curr_x = (int)(loc.x * (1 << 16));
        int step_x = (int)(step.x * (1 << 16));
        if(step.y == 0) {
            int y = clamping(GFloorToIntNonNegative(loc.y), 0, height - 1);
            if(step.x == 0) {
                int x = clamping(GFloorToIntNonNegative(loc.x), 0, width - 1);
                GPixel* start = this->bitmap.getAddr(x, y);
                for (int i = 0; i < count; ++i) {
                    row[i] = *start;
                }
            } else {
                for (int i = 0; i < count; ++i) {
                    int x = clamping((curr_x >> 16), 0, width - 1);
                    GPixel* start = this->bitmap.getAddr(x, y);
                    row[i] = *start;
                    curr_x += step_x;
                }
            }
        } else {
            if(step.x == 0) {
                int x = clamping(GFloorToIntNonNegative(loc.x), 0, width - 1);
                for (int i = 0; i < count; ++i) {
                    int y = clamping(GFloorToIntNonNegative(loc.y), 0, height - 1);
                    GPixel* start = this->bitmap.getAddr(x, y);
                    row[i] = *start;
                    loc += step;
                }
            } else {
                for (int i = 0; i < count; ++i) {
                    int x = clamping(GFloorToIntNonNegative(loc.x), 0, width - 1);
                    int y = clamping(GFloorToIntNonNegative(loc.y), 0, height - 1);
                    GPixel* start = this->bitmap.getAddr(x, y);
                    row[i] = *start;
                    loc += step;
                }
            }
        }
}

void LongShader::shadeRowMirror(int x, int y, int count, GPixel row[]) {
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        float curr_x = loc.x;
        float step_x = step.x;
        float curr_y = loc.y;
        float step_y = step.y;
        int width = this->bitmap.width();
        float inv_width = 1.0f / width / 2;
        int height = this->bitmap.height();
        float inv_height = 1.0f / height / 2;
        float widthLeftBound = width - 0.001f;
        int widthTimesTwo = width * 2;
        if(step.y == 0) {
            float quotient = inv_height * curr_y;
            float remainder = myQuotientMinusFloorf(quotient);
            assert(remainder >= 0);
            assert(remainder <= 1);
            // We just want to cut the decimal
            int y = -getAbsoluteValue((int)(remainder * height * 2  - height)) + height - 1;
            assert(y < height);
            if(step.x == 0) {
                quotient = inv_width * curr_x;
                remainder = myQuotientMinusFloorf(quotient);
                assert(remainder >= 0);
                assert(remainder <= 1);            
                int x = -getAbsoluteValue((int)(remainder * width * 2 - width)) + width - 1;
                assert(x < width);
                GPixel* start = this->bitmap.getAddr(x, y);
                for (int i = 0; i < count; ++i) {
                    row[i] = *start;
                }
            } else {
                for (int i = 0; i < count; ++i) {
                    quotient = inv_width * curr_x;
                    remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    float x = (-fabs(((remainder - 0.5f) * widthTimesTwo)) + widthLeftBound);
                    int floor = GFloorToIntNonNegative(x);
                    assert(x < width);
                    GPixel* start = this->bitmap.getAddr(floor, y);
                    row[i] = *start;
                    curr_x += step_x;
                }
            }
        } else {
            if(step.x == 0) {
                float quotient = inv_width * curr_x;
                float remainder = myQuotientMinusFloorf(quotient);
                assert(remainder >= 0);
                assert(remainder <= 1);
                int x = -getAbsoluteValue((int)(remainder * width * 2 - width)) + width - 1;
                assert(x < width);
                for (int i = 0; i < count; ++i) {
                    quotient = inv_height * curr_y;
                    remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    int y = -getAbsoluteValue((int)(remainder * height * 2  - height)) + height - 1;
                    assert(y < height);
                    GPixel* start = this->bitmap.getAddr(x, y);
                    row[i] = *start;
                    curr_y += step_y;
                }
            } else {
                for (int i = 0; i < count; ++i) {
                    float quotient = inv_width * curr_x;
                    float remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    float x = (-fabs((remainder * width * 2 - width)) + width - 0.001f);
                    int floor_x = GFloorToIntNonNegative(x);
                    assert(x < width);
                    quotient = inv_height * curr_y;
                    remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    float y = (-fabs((remainder * height * 2 - height)) + height - 0.001f);
                    int floor_y = GFloorToIntNonNegative(y);
                    assert(y < height);
                    GPixel* start = this->bitmap.getAddr(floor_x, floor_y);
                    row[i] = *start;
                    curr_x += step_x;
                    curr_y += step_y;
                }
            }
        }
}

void LongShader::shadeRowRepeat(int x, int y, int count, GPixel row[]) {
        const auto step = fInverse.e0();
        GPoint loc = fInverse * GPoint{x + 0.5f, y + 0.5f};
        float curr_x = loc.x;
        float step_x = step.x;
        float curr_y = loc.y;
        float step_y = step.y;
        int width = this->bitmap.width();
        float inv_width = 1.0f / width;
        int height = this->bitmap.height();
        float inv_height = 1.0f / height;
        if(step.y == 0) {
            float quotient = inv_height * curr_y;
            float remainder = myQuotientMinusFloorf(quotient);
            assert(remainder >= 0);
            assert(remainder <= 1);
            int y = GFloorToIntNonNegative(remainder * height);
            assert(y < height);
            if(step.x == 0) {
                quotient = inv_width * curr_x;
                remainder = myQuotientMinusFloorf(quotient);
                assert(remainder >= 0);
                assert(remainder <= 1);
                int x = GFloorToIntNonNegative(remainder * width);
                assert(x < width);
                GPixel* start = this->bitmap.getAddr(x, y);
                for (int i = 0; i < count; ++i) {
                    row[i] = *start;
                }
            } else {
                for (int i = 0; i < count; ++i) {
                    quotient = inv_width * curr_x;
                    remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    int x = GFloorToIntNonNegative(remainder * width);
                    assert(x < width);
                    GPixel* start = this->bitmap.getAddr(x, y);
                    row[i] = *start;
                    curr_x += step_x;
                }
            }
        } else {
            if(step.x == 0) {
                float quotient = inv_width * curr_x;
                float remainder = myQuotientMinusFloorf(quotient);
                assert(remainder >= 0);
                assert(remainder <= 1);
                int x = GFloorToIntNonNegative(remainder * width);
                assert(x < width);
                for (int i = 0; i < count; ++i) {
                    quotient = inv_height * curr_y;
                    remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    int y = GFloorToIntNonNegative(remainder * height);
                    assert(y < height);
                    GPixel* start = this->bitmap.getAddr(x, y);
                    row[i] = *start;
                    curr_y = step_y;
                }
            } else {
                for (int i = 0; i < count; ++i) {
                    float quotient = inv_width * curr_x;
                    float remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    int x = GFloorToIntNonNegative(remainder * width);
                    assert(x < width);
                    quotient = inv_height * curr_y;
                    remainder = myQuotientMinusFloorf(quotient);
                    assert(remainder >= 0);
                    assert(remainder <= 1);
                    int y = GFloorToIntNonNegative(remainder * height);
                    assert(y < height);
                    GPixel* start = this->bitmap.getAddr(x, y);
                    row[i] = *start;
                    curr_x += step_x;
                    curr_y += step_y;
                }
            }
        }
}

std::shared_ptr<GShader> GCreateBitmapShader(const GBitmap& bitmap, const GMatrix& localMatrix, GTileMode tileMode) {
    std::shared_ptr<LongShader> sp = std::make_shared<LongShader>(bitmap, localMatrix, tileMode);
    return sp;
}
