#include "BlinnMaterial.h"
#include <glm/gtc/type_ptr.hpp>

void BlinnMaterial::bind() {
    shaders->bind();
    albedo->bind(albedoUnit);
}

void BlinnMaterial::unbind() {
    shaders->unbind();
    albedo->unbind();
}

void BlinnMaterial::setAlbedo(const std::shared_ptr<mgl::Texture2D>& albedoTexture, int unit) {
    AlbedoId = shaders->Uniforms["material.AlbedoTexture"].index;
    albedo = albedoTexture;
    albedoUnit = unit;
    shaders->bind();
    glUniform1i(AlbedoId, albedoUnit);
    shaders->unbind();
}

void BlinnMaterial::setShininess(float shininess) {
    this->shininess = shininess;
    GLint shininessId = shaders->Uniforms["material.shininess"].index;
    shaders->bind();
    glUniform1f(shininessId, shininess);
    shaders->unbind();
}

void BlinnMaterial::setSpecularColor(const glm::vec3& color) {
    this->specularColor = color;
    GLint specularColorId = shaders->Uniforms["material.specularColor"].index;
    shaders->bind();
    glUniform3fv(specularColorId, 1, glm::value_ptr(specularColor));
    shaders->unbind();
}

void BlinnMaterial::setAmbientColor(const glm::vec3& color) {
    this->ambientColor = color;
    GLint ambientColorId = shaders->Uniforms["material.ambientColor"].index;
    shaders->bind();
    glUniform3fv(ambientColorId, 1, glm::value_ptr(ambientColor));
    shaders->unbind();
}
