/*
 *  Copyright 2024 <me>
 */

#ifndef _g_starter_canvas_h_
#define _g_starter_canvas_h_
#define _g_starter_canvas_h_debug_ 0

#include "include/GCanvas.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GBitmap.h"
#include "utils.h"
#include <stack>

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device) {
        this->mat_stack.push(GMatrix());
        this->is_rotated.push(false);
    }

    void clear(const GColor& color) override;
    void drawRect(const GRect &rect, const GPaint &paint) override;
    void drawConvexPolygon(const GPoint points[], int count, const GPaint& paint) override;
    void save() override;
    void restore() override;
    void concat(const GMatrix& matrix) override;
    void drawPath(const GPath& path, const GPaint& paint) override;
    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                          int count, const int indices[], const GPaint&)  override;
    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                          int level, const GPaint&) override;

private:
    // Note: we store a copy of the bitmap
    const GBitmap fDevice;
    std::stack<GMatrix> mat_stack;
    std::stack<bool> is_rotated;
};

#endif