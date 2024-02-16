#include "Material.h"
#include <glm/gtc/type_ptr.hpp>

void Material::addShaders(mgl::ShaderProgram* shaderProgram) {
    shaders = shaderProgram;
}

mgl::ShaderProgram* Material::getShaders() const {
	return shaders;
}

void Material::setLights(std::vector<Light>& lights) {
    this->lights = lights;
    shaders->bind();
    for (int i = 0; i < lights.size(); i++) {
        glUniform3fv(shaders->Uniforms["lights[" + std::to_string(i) + "].position"].index, 1, glm::value_ptr(this->lights[i].position));
        glUniform3fv(shaders->Uniforms["lights[" + std::to_string(i) + "].color"].index, 1, glm::value_ptr(this->lights[i].color));
        glUniform3fv(shaders->Uniforms["lights[" + std::to_string(i) + "].attenuation"].index, 1, glm::value_ptr(this->lights[i].attenuation));
    }
    shaders->unbind();
}
