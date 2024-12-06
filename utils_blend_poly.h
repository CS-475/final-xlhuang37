#ifndef _g_blend_poly_h_
#define _g_blend_poly_h_

#include "include/GTypes.h"
#include "include/GShader.h"
#include "utils.h"
#include <iostream>

// poly blend utilities
// Edge Definitions
struct Edge {
    // x = my + b
    float slope;
    float offset;
    int top;
    int bottom;

    // Overloading the < operator for sorting by top
    bool operator<(const Edge& other) const {
        return top < other.top;  // Sorting bytop
    }
};

void processEdgeInPlace(std::vector<Edge> &GEdges, const GPoint points[], int width, int height, int count) {
            // trim the vertical excess
        float lefty = points[count - 1].y;
        float righty = points[0].y;
        float leftx = points[count - 1].x;
        float rightx = points[0].x;
        int rounded_lefty = clamping(GRoundToIntNonNegative(lefty), 0, height);
        int rounded_righty = clamping(GRoundToIntNonNegative(righty), 0, height); 
        if(!(rounded_lefty == rounded_righty)) {
            Edge new_edge = Edge();
            // slope and offset
            float delta_x = leftx - rightx;
            float delta_y = lefty - righty;
            float slope = delta_x / delta_y;
            float offset = leftx - slope * lefty;
            new_edge.slope = slope;
            new_edge.offset = offset;

            leftx = (rounded_lefty - lefty) * slope + leftx;
            rightx = (rounded_righty - righty) * slope + rightx;
            // left intersection and excess
            int top = std::min(rounded_lefty, rounded_righty);
            int bottom = std::max(rounded_lefty, rounded_righty);
            float left = (leftx < rightx) ? leftx : rightx;
            float right = (leftx >= rightx) ? leftx : rightx;
            if(left < 0) {
                if(right < 0) {
                    new_edge.slope = 0;
                    new_edge.offset = 0;
                } else {
                    int inter_0 = GRoundToIntNonNegative(- offset / slope);
                    if(inter_0 > top && inter_0 < bottom){
                            Edge vertical_edge = Edge();
                            vertical_edge.slope  = 0;
                            vertical_edge.offset = 0;
                            if(slope < 0) {
                                vertical_edge.top = inter_0;
                                vertical_edge.bottom = bottom;
                                bottom = inter_0;
                            } else {
                                vertical_edge.top = top;
                                vertical_edge.bottom = inter_0;
                                top = inter_0;
                            }
                            GEdges.push_back(vertical_edge);
                    }
                }
            } 
            if(right > width) {
                if(left > width) {
                    new_edge.slope = 0;
                    new_edge.offset = width;
                } else {
                    int inter_width = GRoundToIntNonNegative((width - offset) / slope);
                    if(inter_width > top && inter_width < bottom){
                            Edge vertical_edge = Edge();
                            vertical_edge.slope  = 0;
                            vertical_edge.offset = width;
                            if(slope < 0) {
                                vertical_edge.top = top;
                                vertical_edge.bottom = inter_width;
                                top = inter_width;
                            } else {
                                vertical_edge.top = inter_width;
                                vertical_edge.bottom = bottom;
                                bottom = inter_width;
                            }
                            GEdges.push_back(vertical_edge);
                    }
                }
            } 

            new_edge.top = top;
            new_edge.bottom = bottom;
            GEdges.push_back(new_edge);
        }
                   

    // Other Edges
    for(int i = 0; i < count - 1; i++) {
        // trim the vertical excess
        float lefty = points[i].y;
        float righty = points[i + 1].y;
        float leftx = points[i].x;
        float rightx = points[i + 1].x;
        int rounded_lefty = clamping(GRoundToIntNonNegative(lefty), 0, height);
        int rounded_righty = clamping(GRoundToIntNonNegative(righty), 0, height); 
        if(!(rounded_lefty == rounded_righty)) {
            Edge new_edge = Edge();
            // slope and offset
            float delta_x = leftx - rightx;
            float delta_y = lefty - righty;
            float slope = delta_x / delta_y;
            float offset = leftx - slope * lefty;
            new_edge.slope = slope;
            new_edge.offset = offset;

            leftx = (rounded_lefty - lefty) * slope + leftx;
            rightx = (rounded_righty - righty) * slope + rightx;
            // left intersection and excess
            int top = std::min(rounded_lefty, rounded_righty);
            int bottom = std::max(rounded_lefty, rounded_righty);
            float left = (leftx < rightx) ? leftx : rightx;
            float right = (leftx >= rightx) ? leftx : rightx;

            if(left < 0) {
                if(right < 0) {
                    new_edge.slope = 0;
                    new_edge.offset = 0;
                } else {
                    int inter_0 = GRoundToIntNonNegative(- offset / slope);
                    if(inter_0 > top && inter_0 < bottom) {
                            Edge vertical_edge = Edge();
                            vertical_edge.slope  = 0;
                            vertical_edge.offset = 0;
                            if(slope < 0) {
                                vertical_edge.top = inter_0;
                                vertical_edge.bottom = bottom;
                                bottom = inter_0;
                            } else {
                                vertical_edge.top = top;
                                vertical_edge.bottom = inter_0;
                                top = inter_0;
                            }
                            GEdges.push_back(vertical_edge);
                    }
                }
            } 
            if(right > width) {
                if(left > width) {
                    new_edge.slope = 0;
                    new_edge.offset = width;
                } else {
                    int inter_width = GRoundToIntNonNegative((width - offset) / slope);
                    if(inter_width > top && inter_width < bottom){
                            Edge vertical_edge = Edge();
                            vertical_edge.slope  = 0;
                            vertical_edge.offset = width;
                            if(slope < 0) {
                                vertical_edge.top = top;
                                vertical_edge.bottom = inter_width;
                                top = inter_width;
                            } else {
                                vertical_edge.top = inter_width;
                                vertical_edge.bottom = bottom;
                                bottom = inter_width;
                            }
                            GEdges.push_back(vertical_edge);
                    }
                }
            } 

            new_edge.top = top;
            new_edge.bottom = bottom;
            GEdges.push_back(new_edge);
        }
    }
}

#endif