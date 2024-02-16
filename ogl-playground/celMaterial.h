#pragma once
#include "Material.h"

class CelMaterial : public Material {
public:
    CelMaterial() {}
    ~CelMaterial() {}
    void bind() override;
    void unbind() override;
    void setAlbedo(const std::shared_ptr<mgl::Texture2D>& albedoTexture, int unit = 0);
    void setAmbientColor(const glm::vec3& color);
private:
    GLint AlbedoId; // uniform location
    std::shared_ptr<mgl::Texture2D> albedo = nullptr;
    int albedoUnit = -1;
    glm::vec3 ambientColor;
};
