#include "final.h"
#include "shader_voronoi.h"
#include "shader_sweep.h"
#include <iostream>

std::shared_ptr<GShader> MyFinal::createVoronoiShader(const GPoint points[],
                                                         const GColor colors[],
                                                         int count) {                       
    std::shared_ptr<VoronoiShader> sp = std::make_shared<VoronoiShader>(points, colors, count);
    return sp;
}

std::shared_ptr<GShader> MyFinal::createSweepGradient(GPoint center, float startRadians,
                                                         const GColor colors[], int count) {
    std::shared_ptr<SweepShader> sp = std::make_shared<SweepShader>(center, startRadians, colors, count);
    return sp;
}

std::unique_ptr<GFinal> GCreateFinal() {
    return std::unique_ptr<GFinal>(new MyFinal());
}