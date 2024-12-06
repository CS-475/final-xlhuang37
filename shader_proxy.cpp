/*
 *  Copyright 2024 <me>
 */

#include "shader_proxy.h"
#include "utils.h"
#include <iostream>

bool ProxyShader::isOpaque() {
    return this->originalShader->isOpaque();
}

bool ProxyShader::setContext(const GMatrix& ctm) {
    if(fLocalMatrix.has_value()) {
        return this->originalShader->setContext(ctm * fLocalMatrix.value());
    }

    return false;
}

void ProxyShader::shadeRow(int x, int y, int count, GPixel row[]) {
    return this->originalShader->shadeRow(x, y, count, row);
}

std::shared_ptr<GShader> GCreateProxyShader(const GPoint triangle[3], const GPoint tex[3], GShader* originalShader) {
    std::shared_ptr<ProxyShader> sp = std::make_shared<ProxyShader>(triangle, tex, originalShader);

    return sp;
}