#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include "mgl/mgl.hpp"
#include "Material.h"
#include "Environment.h"
#include "utils.h"
#include "mgl/mglMesh.hpp"

class Entity {
public:
    Entity() {}
    //Entity(mgl::Mesh* mesh);
	virtual ~Entity() {}
    void draw();
    //void addMesh(mgl::Mesh* mesh);
    void setModelMatrix(const glm::mat4& matrix);
    void setModelMatrix(glm::vec3 position, glm::vec3 rotationAxis, float angle, glm::vec3 scale);
    void setModelMatrix(const Transform& newTransform);
    glm::mat4 getModelMatrix();
    // void setMaterial(std::shared_ptr<Material> material);
    void updateModelMatrix();
    void addChild(const std::shared_ptr<Entity>& child);
    std::vector<std::shared_ptr<Entity>> children;
    void rotate(float angle, glm::vec3 axis);
    void rotateOnWorldAxis(float angle, glm::vec3 axis);

    void loadModel(const std::string& path, Environment* environment);

    std::function<void()> preDrawCallback = nullptr;
    std::function<void()> postDrawCallback = nullptr;
protected:
    glm::mat4 ModelMatrix;
    glm::mat3 NormalMatrix;
    Transform transform;

    struct Model {
        std::shared_ptr<mgl::Mesh> mesh;
        std::shared_ptr<Material> material;
        GLint ModelMatrixId;
        GLint NormalMatrixId;
    };

    std::vector<Model> Models;

    Entity* parent = nullptr;
private:
    void processNode(aiNode *node, const aiScene *scene, Environment* environment, const std::string& directory);
};
