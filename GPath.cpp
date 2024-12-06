#include "include/GPath.h"
#include <assert.h>
#include <iostream>

static inline void updateWithExtrema(const float& extrema, float& low, float& high) {
    if(extrema < low) 
        low = extrema;
    if(extrema > high)
        high = extrema;
}

static inline void updateBound(GRect& result, GPoint& pts) {
    updateWithExtrema(pts.x, result.left, result.right);
    updateWithExtrema(pts.y, result.top, result.bottom);
}

static inline float eval_quad_float(const float& t, const float& A, const float& B, const float& C) {
    return (A - 2 * B + C) * t * t 
            + (2 * (-A + B)) * t
            + A;
}

static inline float eval_cubic_float(const float& t, const float& A, const float& B, const float& C, const float& D) {
    return (-A + 3 * (B - C) + D) * t * t * t
            + 3 * (A - 2 * B + C) * t * t
            + 3 * (-A + B) * t
            + A;
}

static inline void updateBoundQuad(const float& A, const float& B, const float& C, float& low, float& high) {
    float numerator = 2 * A - 2 * B;
    float denominator = 2 * (A + C) - 4 * B;
    if(denominator != 0) {
        float t = numerator / denominator;
        if(t > 1 || t < 0)
            return;
        float extrema = eval_quad_float(t, A, B, C);
        updateWithExtrema(extrema, low, high);
    }
    return;
}

static inline void updateBoundCubic(const float& A, const float& B, const float& C, const float& D, float& low, float& high) {
    float a = 9 * B - 3 * A - 9 * C + 3 * D;
    float b = 6 * (A + C) - 12 * B;
    float c = 3 * (B - A);
    // f'(t) = a x ^ 2 + b x + c = 0;
    float inside_sqrt = b * b - 4 * a * c;
    if(a != 0) {
        if(inside_sqrt < 0) {
            // no solution on the real axis. 
            return;
        } else {
            // Solve Quadratic
            // First Solution
            float t = (-b + sqrt(inside_sqrt)) / 2 / a;
            if(t < 0 || t > 1) {
                return;
            } else {
                float extrema = eval_cubic_float(t, A, B, C, D);
                updateWithExtrema(extrema, low, high);
            }

            // Second Solution
            t = (-b - sqrt(inside_sqrt)) / 2 / a;
            if(t < 0 || t > 1) {
                return;
            } else {
                float extrema = eval_cubic_float(t, A, B, C, D);
                updateWithExtrema(extrema, low, high);
            }
        }
    } else {
        if(b == 0)
            return;
        else {
            float t = - c / b;
            if(t > 1 || t < 0) {
                return;
            } else {
                float extrema = eval_cubic_float(t, A, B, C, D);
                updateWithExtrema(extrema, low, high);
            }
        }
    }
}


GRect GPath::bounds() const {
    if(this->fVbs.size() == 0) {
        return {0,0,0,0};
    }
    GRect result = {INFINITY,INFINITY,-INFINITY, -INFINITY};

    GPoint pts[GPath::kMaxNextPoints];
    GPath::Iter iter(*this);
    while(auto v = iter.next(pts)) {
        if(v == GPathVerb::kCubic) {
            GPoint& A = pts[0];
            GPoint& B = pts[1];
            GPoint& C = pts[2];
            GPoint& D = pts[3];
            updateBound(result, D);
            updateBoundCubic(A.x, B.x, C.x, D.x, result.left, result.right);
            updateBoundCubic(A.y, B.y, C.y, D.y, result.top, result.bottom);
            continue;
        } else if(v == GPathVerb::kQuad) {
            GPoint& A = pts[0];
            GPoint& B = pts[1];
            GPoint& C = pts[2];
            updateBound(result, C);
            updateBoundQuad(A.x, B.x, C.x, result.left, result.right);
            updateBoundQuad(A.y, B.y, C.y, result.top, result.bottom);
        } else if (v == GPathVerb::kLine){
            updateBound(result, pts[0]);
            updateBound(result, pts[1]);
        } else {
            // make sure it is move. Suppose we update Path Verb, the if statements here should also be updated
            assert(v == GPathVerb::kMove);
            updateBound(result, pts[0]);
        }
    }


    return result;
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    const GPoint& A = src[0];
    const GPoint& B = src[1];
    const GPoint& C = src[2];

    dst[0] = A;
    dst[1] = (1 - t) * A + t * B;
    dst[2] = (1 - t) * (1 -t) * A + 2 * t * (1 - t) * B + t * t * C;
    dst[3] = (1 - t) * B + t * C;
    dst[4] = C;
}



void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    const GPoint& A = src[0];
    const GPoint& B = src[1];
    const GPoint& C = src[2];
    const GPoint& D = src[3];

    dst[0] = A;
    dst[1] = (1 - t) * A + t * B;
    dst[2] = (1 - t) * (1 -t) * A + 2 * t * (1 - t) * B + t * t * C;
    dst[3] = (1 - t) * (1 -t) * (1 - t) * A 
            + 3 * t * (1 - t) * (1 - t) * B 
            + 3 * t * t * (1 -t ) * C
            + t * t * t * D;
    dst[4] = (1 - t) * (1 -t) * B + 2 * t * (1 - t) * C + t * t * D;
    dst[5] = (1 - t) * C + t * D;
    dst[6] = D;
}