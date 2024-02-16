#include "Entity.h"
#include "pbrMaterial.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

//Entity::Entity(mgl::Mesh* mesh) : mesh(mesh) {}

//void Entity::addMesh(mgl::Mesh* mesh) {
//    this->mesh = mesh;
//}

glm::mat4 Entity::getModelMatrix() {
    return ModelMatrix;
}

void Entity::setModelMatrix(const glm::mat4& matrix) {
    ModelMatrix = matrix;
}

void Entity::setModelMatrix(glm::vec3 position, glm::vec3 rotationAxis, float angle, glm::vec3 scale) {
    transform.position = position;
    transform.rotation = glm::angleAxis(angle, rotationAxis);
    transform.scale = scale;

    updateModelMatrix();
}

void Entity::setModelMatrix(const Transform &newTransform) {
    transform.position = newTransform.position;
    transform.rotation = newTransform.rotation;
    transform.scale = newTransform.scale;

    updateModelMatrix();
}

void Entity::updateModelMatrix() {
    ModelMatrix = glm::translate(glm::mat4(1.0f), transform.position) * glm::mat4_cast(transform.rotation) * glm::scale(glm::mat4(1.0f), transform.scale);

    if (parent != nullptr) {
        ModelMatrix = parent->ModelMatrix * ModelMatrix;
    }

    NormalMatrix = glm::inverseTranspose(glm::mat3(ModelMatrix));

    for (auto const &child : children) {
        child->updateModelMatrix();
    }
}

void Entity::addChild(const std::shared_ptr<Entity>& child) {
    child->parent = this;
    children.push_back(child);
    child->updateModelMatrix();
}

void Entity::draw() {
    // go over all meshes with index. bind material with same index and draw
    for (int i = 0; i < Models.size(); i++) {
        Models[i].material->bind();
        glUniformMatrix4fv(Models[i].ModelMatrixId, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix3fv(Models[i].NormalMatrixId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        // pre draw
        if (preDrawCallback != nullptr) {
            preDrawCallback();
        }

        Models[i].mesh->draw();

        // post draw
        if (postDrawCallback != nullptr) {
            postDrawCallback();
        }

        Models[i].material->unbind();
    }

     for (auto const &child : children) {
         child->draw();
     }
}

// void Entity::setMaterial(std::shared_ptr<Material> material) {
// 	this->material = material;
//     mgl::ShaderProgram* material_shaders = material->getShaders();
//     ModelMatrixId = material_shaders->Uniforms[mgl::MODEL_MATRIX].index;
//     NormalMatrixId = material_shaders->Uniforms[mgl::NORMAL_MATRIX].index;
// }

void Entity::rotate(float angle, glm::vec3 axis) {
    transform.rotation = glm::rotate(transform.rotation, angle, axis);
    updateModelMatrix();
}

void Entity::rotateOnWorldAxis(float angle, glm::vec3 axis) {
    transform.rotation = glm::angleAxis(angle, axis) * transform.rotation;
    updateModelMatrix();
}


void Entity::loadModel(const std::string& path, Environment* environment) {
    // only gltf files are supported
    if (path.substr(path.find_last_of(".") + 1) != "gltf") {
        std::cout << "Error: File is not a gltf file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string directory = path.substr(0, path.find_last_of('/')); // important for textures
    Assimp::Importer importer;
    unsigned int AssimpFlags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes |
        aiProcess_OptimizeGraph | aiProcess_PreTransformVertices;
    //NOTE: PreTransformVertices is used to apply the model matrix to the vertices. Allows having to apply the model matrix when loading children of meshes.
    // Without this flag, we would have to have one mesh per entity, where child meshes would have to be child entities.
    // WILL NOT WORK WITH ANIMATIONS AND SKINNING/BONES
    const aiScene *scene = importer.ReadFile(path, AssimpFlags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        std::cout << "Error while loading:" << importer.GetErrorString()
                << std::endl;
        exit(EXIT_FAILURE);
    }

    processNode(scene->mRootNode, scene, environment, directory);
}

void Entity::processNode(aiNode *node, const aiScene *scene, Environment* environment, const std::string& directory) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        // mesh
        aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
        std::shared_ptr<mgl::Mesh> mesh = std::make_shared<mgl::Mesh>();
        mesh->processMesh(aimesh);

        // material
        // FIXME: For now, let's assume every material is a PBR material
        aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];
        std::shared_ptr<PBRMaterial> pbrMaterial = std::make_shared<PBRMaterial>();

        mgl::ShaderProgram* PBRShader = new mgl::ShaderProgram();
        PBRShader->addShader(GL_VERTEX_SHADER, "pbr-vs.glsl");
        PBRShader->addShader(GL_FRAGMENT_SHADER, "pbr-fs.glsl");

        PBRShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
        PBRShader->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
        PBRShader->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
        PBRShader->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
        PBRShader->addAttribute(mgl::BITANGENT_ATTRIBUTE, mgl::Mesh::BITANGENT);

        PBRShader->addUniform(mgl::MODEL_MATRIX);
        PBRShader->addUniform(mgl::NORMAL_MATRIX);
        PBRShader->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
        for (int i = 0; i < MAX_LIGHTS; i++) {
            PBRShader->addUniform("lights[" + std::to_string(i) + "].position");
            PBRShader->addUniform("lights[" + std::to_string(i) + "].color");
            PBRShader->addUniform("lights[" + std::to_string(i) + "].attenuation");
        }
        PBRShader->addUniform("material.AlbedoTexture");
        PBRShader->addUniform("material.NormalTexture");
        // PBRShader->addUniform("material.RoughnessTexture");
        // PBRShader->addUniform("material.MetallicTexture");
        PBRShader->addUniform("material.MetallicRoughnessTexture");
        // PBRShader->addUniform("material.AOTexture");
        PBRShader->addUniform("material.albedoColor");
        PBRShader->addUniform("material.metallic");
        PBRShader->addUniform("material.roughness");
        PBRShader->addUniform("material.opacity");
        PBRShader->addUniform("irradianceMap");
        PBRShader->addUniform("prefilterMap");
        PBRShader->addUniform("brdfLUT");
        PBRShader->create();

        pbrMaterial->addShaders(PBRShader);
        pbrMaterial->processMaterial(aimaterial, directory);
        pbrMaterial->setEnvironment(environment);

        GLint ModelMatrixId = PBRShader->Uniforms[mgl::MODEL_MATRIX].index;
        GLint NormalMatrixId = PBRShader->Uniforms[mgl::NORMAL_MATRIX].index;

        Model model;
        model.mesh = mesh;
        model.material = pbrMaterial;
        model.ModelMatrixId = ModelMatrixId;
        model.NormalMatrixId = NormalMatrixId;
        Models.push_back(model);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, environment, directory);
    }

}
