/*
 *  Copyright 2024 <me>
 */

#include "include/GMatrix.h"
#include "include/GTypes.h"
#include <iostream>
#include <algorithm>
#include <cstring>

GMatrix::GMatrix() {
    fMat[0] = 1;
    fMat[1] = 0;
    fMat[2] = 0;
    fMat[3] = 1;
    fMat[4] = 0;
    fMat[5] = 0;
}

GMatrix GMatrix::Translate(float tx, float ty) {
    return GMatrix(1, 0, tx, 0, 1, ty);
}

GMatrix GMatrix::Scale(float sx, float sy) {
    return GMatrix(sx, 0, 0, 0, sy, 0);
}

GMatrix GMatrix::Rotate(float radians) {
    float cos = std::cos(radians);
    float sin = std::sin(radians);
    return GMatrix(cos, -sin, 0, sin, cos, 0);
}

GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b) {
    return GMatrix(
        a.fMat[0] * b.fMat[0] + a.fMat[2] * b.fMat[1],
        a.fMat[0] * b.fMat[2] + a.fMat[2] * b.fMat[3],
        a.fMat[0] * b.fMat[4] + a.fMat[2] * b.fMat[5] + a.fMat[4],
        a.fMat[1] * b.fMat[0] + a.fMat[3] * b.fMat[1],
        a.fMat[1] * b.fMat[2] + a.fMat[3] * b.fMat[3],
        a.fMat[1] * b.fMat[4] + a.fMat[3] * b.fMat[5] + a.fMat[5] 
    );
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    while(count--) {
        float x = src->x;
        float y = src->y;
        dst->x = fMat[0] * x + fMat[2] * y + fMat[4];
        dst->y = fMat[1] * x + fMat[3] * y + fMat[5];
        src++;
        dst++;
    }
}

nonstd::optional<GMatrix> GMatrix::invert() const {
    float det = fMat[0] * fMat[3] - fMat[1] * fMat[2];
    if(det == 0) {
        return nonstd::nullopt;
    }
    float invdet = 1 / det;
    return nonstd::optional<GMatrix>{GMatrix(
        fMat[3] * invdet, 
        -fMat[2] * invdet, 
        (fMat[2] * fMat[5] - fMat[3] * fMat[4]) * invdet, 
        -fMat[1] * invdet, 
        fMat[0] * invdet, 
        -(fMat[0] * fMat[5] - fMat[1] * fMat[4]) * invdet
    )};
}