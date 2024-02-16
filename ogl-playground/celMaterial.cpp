#include "CelMaterial.h"
#include <glm/gtc/type_ptr.hpp>

void CelMaterial::bind() {
    shaders->bind();
    albedo->bind(albedoUnit);
}

void CelMaterial::unbind() {
    shaders->unbind();
    albedo->unbind();
}

void CelMaterial::setAlbedo(const std::shared_ptr<mgl::Texture2D>& albedoTexture, int unit) {
    AlbedoId = shaders->Uniforms["material.AlbedoTexture"].index;
    albedo = albedoTexture;
    albedoUnit = unit;
    shaders->bind();
    glUniform1i(AlbedoId, albedoUnit);
    shaders->unbind();
}

void CelMaterial::setAmbientColor(const glm::vec3& color) {
    this->ambientColor = color;
    GLint ambientColorId = shaders->Uniforms["material.ambientColor"].index;
    shaders->bind();
    glUniform3fv(ambientColorId, 1, glm::value_ptr(ambientColor));
    shaders->unbind();
}
