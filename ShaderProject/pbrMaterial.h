#pragma once
#include "Material.h"
#include "Environment.h"

class PBRMaterial : public Material {
public:
    PBRMaterial() {}
    ~PBRMaterial() {}
    void bind() override;
    void unbind() override;
    void processMaterial(const aiMaterial* aimaterial, const std::string& directory) override;
    void setAlbedo(const std::shared_ptr<mgl::Texture2D>& albedoTexture, int unit = 0, const glm::vec3& albedoColor = glm::vec3(1.0f), const float opacity = 1.0f);
    void setNormalMap(const std::shared_ptr<mgl::Texture2D>& normalMapTexture, int unit = 1);
    // void setRoughnessMap(const std::shared_ptr<mgl::Texture2D>& roughnessMapTexture, int unit = 2);
    // void setMetallicMap(const std::shared_ptr<mgl::Texture2D>& metallicMapTexture, int unit = 3);
    void setMetallicRoughnessMap(const std::shared_ptr<mgl::Texture2D>& metallicRoughnessMapTexture, int unit = 2, float metallic = 1.0f, float roughness = 1.0f);
    // void setAOMap(const std::shared_ptr<mgl::Texture2D>& aoMapTexture, int unit = 3);
    // void setAOMap(const std::shared_ptr<mgl::Texture2D>& aoMapTexture, int unit = 4);
    void setEnvironment(Environment* environment, int unit = 5);

private:
    GLint AlbedoId; // uniform location
    std::shared_ptr<mgl::Texture2D> albedo = nullptr;
    int albedoUnit = -1;

    GLint AlbedoColorId; // uniform location
    glm::vec3 albedoColor = glm::vec3(1.0f);

    GLint NormalMapId; // uniform location
    std::shared_ptr<mgl::Texture2D> normalMap = nullptr;
    int normalMapUnit = -1;

    // GLint RoughnessMapId; // uniform location
    // std::shared_ptr<mgl::Texture2D> roughnessMap = nullptr;
    // int roughnessMapUnit = -1;

    // GLint MetallicMapId; // uniform location
    // std::shared_ptr<mgl::Texture2D> metallicMap = nullptr;
    // int metallicMapUnit = -1;

    GLint MetallicRoughnessMapId; // uniform location
    std::shared_ptr<mgl::Texture2D> metallicRoughnessMap = nullptr;
    int metallicRoughnessMapUnit = -1;

    GLint MettallicId; // uniform location
    float metallic = 1.0f;

    GLint RoughnessId; // uniform location
    float roughness = 1.0f;

    GLint OpacityId; // uniform location
    float opacity = 1.0f;

    // GLint AOMapId; // uniform location
    // std::shared_ptr<mgl::Texture2D> aoMap = nullptr;
    // int aoMapUnit = -1;

    Environment* environment;
    int environmentUnit = -1;

};
