#pragma once

#include "mgl/mgl.hpp"
#include <glm/glm.hpp>
#include "utils.h"

class Material {
public:
    Material() {}
    virtual ~Material() {}
    void addShaders(mgl::ShaderProgram* shaderProgram);
    mgl::ShaderProgram* getShaders() const;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    void setLights(std::vector<Light>& lights);
    virtual void processMaterial(const aiMaterial* aimaterial, const std::string& directory) = 0;
protected:
    mgl::ShaderProgram* shaders = nullptr;
    std::vector<Light> lights;
};
