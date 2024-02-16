#pragma once
#include "Material.h"

class BlinnMaterial : public Material {
public:
    BlinnMaterial() {}
    ~BlinnMaterial() {}
    void bind() override;
    void unbind() override;
    void setAlbedo(const std::shared_ptr<mgl::Texture2D>& albedoTexture, int unit = 0);
    void setShininess(float shininess);
    void setSpecularColor(const glm::vec3& color);
    void setAmbientColor(const glm::vec3& color);
private:
    GLint AlbedoId; // uniform location
    std::shared_ptr<mgl::Texture2D> albedo = nullptr;
    int albedoUnit = -1;

    float shininess;
    glm::vec3 specularColor;
    glm::vec3 ambientColor;
};

