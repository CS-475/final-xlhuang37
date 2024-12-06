#include "include/GPathBuilder.h"
#include <assert.h>

void GPathBuilder::addRect(const GRect& rect, GPathDirection direction) {
    this->moveTo(rect.left, rect.top);
    if(direction == GPathDirection::kCW) {
        this->lineTo(rect.right, rect.top);
        this->lineTo(rect.right, rect.bottom);
        this->lineTo(rect.left, rect.bottom);
    } else if(direction == GPathDirection::kCCW) {
        this->lineTo(rect.left, rect.bottom);
        this->lineTo(rect.right, rect.bottom);
        this->lineTo(rect.right, rect.top);
    } else {
        assert(0);
    }
}

void GPathBuilder::addPolygon(const GPoint pts[], int count) {
    this->moveTo(pts[0]);
    for(int i = 1; i < count; i++) {
        this->lineTo(pts[i]);
    } 
}

void GPathBuilder::addCircle(GPoint center, float radius, GPathDirection direction) {
    float c = 0.55228474983079f * radius;
    if(direction == GPathDirection::kCW) {
        GPoint shift = {0, radius};
        this->moveTo(center + shift);
        GPoint p1 = {-c, radius};
        GPoint p2 = {-radius, c};
        GPoint p3 = {-radius, 0};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
        p1 = {-radius, -c};
        p2 = {-c, -radius};
        p3 = {0, -radius};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
        p1 = {c, -radius};
        p2 = {radius, -c};
        p3 = {radius, 0};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
        p1 = {radius, c};
        p2 = {c, radius};
        p3 = {0, radius};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
    } else {
        GPoint shift = {0, radius};
        this->moveTo(center + shift);
        GPoint p1 = {c, radius};
        GPoint p2 = {radius, c};
        GPoint p3 = {radius, 0};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
        p1 = {radius, -c};
        p2 = {c, -radius};
        p3 = {0, -radius};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
        p1 = {-c, -radius};
        p2 = {-radius, -c};
        p3 = {-radius, 0};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
        p1 = {-radius, c};
        p2 = {-c, radius};
        p3 = {0, radius};
        this->cubicTo(p1 + center, p2 + center, p3 + center);
    }

    return;
}

