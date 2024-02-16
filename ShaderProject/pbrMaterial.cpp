#include "PbrMaterial.h"
#include <glm/gtc/type_ptr.hpp>

void PBRMaterial::bind() {
    shaders->bind();
    albedo->bind(albedoUnit);
    normalMap->bind(normalMapUnit);
    // roughnessMap->bind(roughnessMapUnit);
    // metallicMap->bind(metallicMapUnit);
    metallicRoughnessMap->bind(metallicRoughnessMapUnit);
    // aoMap->bind(aoMapUnit);
    environment->bindIrradianceMap(environmentUnit);
    environment->bindPreFilterMap(environmentUnit + 1);
    environment->bindBrdfLUT(environmentUnit + 2);
}

void PBRMaterial::unbind() {
    shaders->unbind();
    albedo->unbind();
    normalMap->unbind();
    // roughnessMap->unbind();
    // metallicMap->unbind();
    metallicRoughnessMap->unbind();
    // aoMap->unbind();
    environment->unbindIrradianceMap();
    environment->unbindPreFilterMap();
    environment->unbindBrdfLUT();
}


void PBRMaterial::setAlbedo(const std::shared_ptr<mgl::Texture2D>& albedoTexture, int unit, const glm::vec3& albedoColor, const float opacity) {
    AlbedoId = shaders->Uniforms["material.AlbedoTexture"].index;
    AlbedoColorId = shaders->Uniforms["material.albedoColor"].index;
    OpacityId = shaders->Uniforms["material.opacity"].index;
    albedo = albedoTexture;
    this->albedoColor = albedoColor;
    this->opacity = opacity;
    albedoUnit = unit;
    shaders->bind();
    glUniform1i(AlbedoId, albedoUnit);
    glUniform3fv(AlbedoColorId, 1, glm::value_ptr(this->albedoColor));
    glUniform1f(OpacityId, this->opacity);
    shaders->unbind();
}

void PBRMaterial::setNormalMap(const std::shared_ptr<mgl::Texture2D>& normalMapTexture, int unit) {
    NormalMapId = shaders->Uniforms["material.NormalTexture"].index;
    normalMap = normalMapTexture;
    normalMapUnit = unit;
    shaders->bind();
    glUniform1i(NormalMapId, normalMapUnit);
    shaders->unbind();
}

// void PBRMaterial::setRoughnessMap(const std::shared_ptr<mgl::Texture2D>& roughnessMapTexture, int unit) {
//     RoughnessMapId = shaders->Uniforms["material.RoughnessTexture"].index;
//     roughnessMap = roughnessMapTexture;
//     roughnessMapUnit = unit;
//     shaders->bind();
//     glUniform1i(RoughnessMapId, roughnessMapUnit);
//     shaders->unbind();
// }

// void PBRMaterial::setMetallicMap(const std::shared_ptr<mgl::Texture2D>& metallicMapTexture, int unit) {
//     MetallicMapId = shaders->Uniforms["material.MetallicTexture"].index;
//     metallicMap = metallicMapTexture;
//     metallicMapUnit = unit;
//     shaders->bind();
//     glUniform1i(MetallicMapId, metallicMapUnit);
//     shaders->unbind();
// }

void PBRMaterial::setMetallicRoughnessMap(const std::shared_ptr<mgl::Texture2D>& metallicRoughnessMapTexture, int unit, float metallic, float roughness) {
    MetallicRoughnessMapId = shaders->Uniforms["material.MetallicRoughnessTexture"].index;
    MettallicId = shaders->Uniforms["material.metallic"].index;
    RoughnessId = shaders->Uniforms["material.roughness"].index;
    this->metallic = metallic;
    this->roughness = roughness;
    metallicRoughnessMap = metallicRoughnessMapTexture;
    metallicRoughnessMapUnit = unit;
    shaders->bind();
    glUniform1i(MetallicRoughnessMapId, metallicRoughnessMapUnit);
    glUniform1f(MettallicId, metallic);
    glUniform1f(RoughnessId, roughness);
    shaders->unbind();
}

// void PBRMaterial::setAOMap(const std::shared_ptr<mgl::Texture2D>& aoMapTexture, int unit) {
//     AOMapId = shaders->Uniforms["material.AOTexture"].index;
//     aoMap = aoMapTexture;
//     aoMapUnit = unit;
//     shaders->bind();
//     glUniform1i(AOMapId, aoMapUnit);
//     shaders->unbind();
// }

void PBRMaterial::setEnvironment(Environment* environment, int unit) {
    this->environment = environment;
    environmentUnit = unit;
    GLint IrradianceMapId = shaders->Uniforms["irradianceMap"].index;
    GLint PrefilterMapId = shaders->Uniforms["prefilterMap"].index;
    GLint BrdfLUTId = shaders->Uniforms["brdfLUT"].index;
    shaders->bind();
    glUniform1i(IrradianceMapId, unit);
    glUniform1i(PrefilterMapId, unit + 1);
    glUniform1i(BrdfLUTId, unit + 2);
    shaders->unbind();
}

void PBRMaterial::processMaterial(const aiMaterial* aimaterial, const std::string& directory) {
    // only supports one texture per type
    aiString diffusePath;
    aiString normalPath;
    aiString metallicRoughnessPath;
    // aiString roughnessPath;
    // aiString metallicPath;
    // aiString aoPath;

    std::string diffusePathStr;
    std::string normalPathStr;
    std::string metallicRoughnessPathStr;

    // In case the material does not have textures
    glm::vec3 albedoColor = glm::vec3(1.0f);
    float metallic = 1.0f;
    float roughness = 1.0f;
    float opacity = 1.0f;

    aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);
    aimaterial->GetTexture(aiTextureType_NORMALS, 0, &normalPath);
    aimaterial->GetTexture(aiTextureType_UNKNOWN, 0, &metallicRoughnessPath);
    // aimaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughnessPath);
    // aimaterial->GetTexture(aiTextureType_METALNESS, 0, &metallicPath);
    // aimaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &aoPath);

    // if any of the textures are not found, error
    if (diffusePath.length == 0) {
        std::cerr << "PBRMaterial::processMaterial: No albedo texture found" << std::endl;
        diffusePathStr = "../assets/textures/white1x1.png";
        aiColor3D aiAlbedoColor; // default to white
        aimaterial->Get(AI_MATKEY_BASE_COLOR, aiAlbedoColor);
        albedoColor = glm::vec3(aiAlbedoColor.r, aiAlbedoColor.g, aiAlbedoColor.b);
    } else {
        diffusePathStr = directory + '/' + diffusePath.C_Str();
    }
    std::cout << "Albedo texture: " << diffusePathStr << std::endl;

    if (normalPath.length == 0 ) {
        std::cerr << "PBRMaterial::processMaterial: No normal map found" << std::endl;
        normalPathStr = "../assets/textures/normal_default.png";
    } else {
        normalPathStr = directory + '/' + normalPath.C_Str();
    }
    std::cout << "Normal map: " << normalPathStr << std::endl;

    if (metallicRoughnessPath.length == 0 ) {
        std::cerr << "PBRMaterial::processMaterial: No metallic roughness map found" << std::endl;
        metallicRoughnessPathStr = "../assets/textures/white1x1.png";
        aimaterial->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
        aimaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
    } else {
        metallicRoughnessPathStr = directory + '/' + metallicRoughnessPath.C_Str();
    }
    std::cout << "Metallic Roughness map: " << metallicRoughnessPathStr << std::endl;

    // opacity
    aimaterial->Get(AI_MATKEY_OPACITY, opacity);

    // if (roughnessPath.length == 0 ) {
    //     std::cerr << "PBRMaterial::processMaterial: No roughness map found" << std::endl;
    //     roughnessPath = aiString("white.png");
    // }
    // std::cout << "Roughness map: " << directory + '/' + roughnessPath.C_Str() << std::endl;

    // if (metallicPath.length == 0 ) {
    //     std::cerr << "PBRMaterial::processMaterial: No metallic map found" << std::endl;
    //     metallicPath = aiString("black.png");
    // }
    // std::cout << "Metallic map: " << directory + '/' + metallicPath.C_Str() << std::endl;

    // if (aoPath.length == 0 ) {
    //     std::cerr << "PBRMaterial::processMaterial: No ambient occlusion map found" << std::endl;
    //     aoPath = aiString("white.png");
    // }
    // std::cout << "AO map: " << directory + '/' + aoPath.C_Str() << std::endl;

    std::shared_ptr<mgl::Texture2D> albedoTexture = std::make_shared<mgl::Texture2D>();
    albedoTexture->load(diffusePathStr);
    setAlbedo(albedoTexture, 0, albedoColor, opacity);

    std::shared_ptr<mgl::Texture2D> normalMapTexture = std::make_shared<mgl::Texture2D>();
    normalMapTexture->load(normalPathStr);
    setNormalMap(normalMapTexture, 1);

    std::shared_ptr<mgl::Texture2D> metallicRoughnessMapTexture = std::make_shared<mgl::Texture2D>();
    metallicRoughnessMapTexture->load(metallicRoughnessPathStr);
    setMetallicRoughnessMap(metallicRoughnessMapTexture, 2, metallic, roughness);

    // std::shared_ptr<mgl::Texture2D> metallicMapTexture = std::make_shared<mgl::Texture2D>();
    // metallicMapTexture->load(directory + '/' + metallicPath.C_Str());
    // setMetallicMap(metallicMapTexture, 2);

    // std::shared_ptr<mgl::Texture2D> roughnessMapTexture = std::make_shared<mgl::Texture2D>();
    // roughnessMapTexture->load(directory + '/' + roughnessPath.C_Str());
    // setRoughnessMap(roughnessMapTexture, 3);

    // std::shared_ptr<mgl::Texture2D> aoMapTexture = std::make_shared<mgl::Texture2D>();
    // aoMapTexture->load(directory + '/' + aoPath.C_Str());
    // setAOMap(aoMapTexture, 4);
    // setAOMap(aoMapTexture, 3);
}
