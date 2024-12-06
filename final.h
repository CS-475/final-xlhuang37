#ifndef _g_final_h_
#define _g_final_h_

#include "include/GFinal.h"

class MyFinal : public GFinal {

    std::shared_ptr<GShader> createVoronoiShader(const GPoint points[],
                                                         const GColor colors[],
                                                         int count) override;
    /**
     *  Return a sweep-gradient shader, centered at 'center', starting wiht color[0] at  startRadians
     *  and ending with colors[count-1] at startRadians+2pi. The colors are distributed evenly around the sweep.
     */
    std::shared_ptr<GShader> createSweepGradient(GPoint center, float startRadians,
                                                         const GColor colors[], int count) override;

    // std::shared_ptr<GShader> createLinearPosGradient(GPoint p0, GPoint p1,
    //                                                          const GColor colors[],
    //                                                          const float pos[],
    //                                                          int count) override{
    //     const GPoint pt = {0,0};
    //     const GColor color = {1,1,1,1};
    //     return createVoronoiShader(&pt, &color, 1);
    // }

    // std::shared_ptr<GShader> createColorMatrixShader(const GColorMatrix&,
    //                                                          GShader* realShader) override{
    //     const GPoint pt = {0,0};
    //     const GColor color = {1,1,1,1};
    //     return createVoronoiShader(&pt, &color, 1);
    // }

    // std::shared_ptr<GPath> strokePolygon(const GPoint[], int count, float width, bool isClosed) override {
    //     const GPoint pt = {0,0};
    //     const GColor color = {1,1,1,1};
    //     return createVoronoiShader(&pt, &color, 1);
    // }

    // /*
    //  *  Draw the corresponding mesh constructed from a quad with each side defined by a
    //  *  quadratic bezier, evaluating them to produce "level" interior lines (same convention
    //  *  as drawQuad().
    //  *
    //  *  pts[0]    pts[1]    pts[2]
    //  *  pts[7]              pts[3]
    //  *  pts[6]    pts[5]    pts[4]
    //  *
    //  *  Evaluate values within the mesh using the Coons Patch formulation:
    //  *
    //  *  value(u,v) = TB(u,v) + LR(u,v) - Corners(u,v)
    //  *
    //  *     Top is quadratic bezier: pts[0], pts[1], pts[2]
    //  *  Bottom is quadratic bezier: pts[6], pts[5], pts[4]
    //  *    Left is quadratic bezier: pts[0], pts[7], pts[6]
    //  *   Right is quadratic bezier: pts[2], pts[3], pts[4]
    //  *
    //  *  Where
    //  *      TB is computed by first evaluating the Top and Bottom curves at (u), and then
    //  *      linearly interpolating those points by (v)
    //  *
    //  *      LR is computed by first evaluating the Left and Right curves at (v), and then
    //  *      linearly interpolating those points by (u)
    //  *
    //  *      Corners is computed by our standard "drawQuad" evaluation using the 4 corners 0,2,4,6
    //  */
    // virtual void drawQuadraticCoons(GCanvas*, const GPoint pts[8], const GPoint tex[4],
    //                                 int level, const GPaint&) {}


};













#endif