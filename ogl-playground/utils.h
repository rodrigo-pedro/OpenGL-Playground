#pragma once
#include <GL/glew.h>
#include <glm/gtc/quaternion.hpp>

#define LENGTH_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

const auto PERSPECTIVE = 0;
const auto ORTHOGRAPHIC = 1;

const auto DEFAULT_ORTHO = 5.0f;

#define TOGGLE_CAMERA_TYPE(type) (type == PERSPECTIVE ? ORTHOGRAPHIC : PERSPECTIVE)

using Vertex = struct {
    GLfloat XYZW[4];
    GLfloat RGBA[4];
};

using Transform = struct {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

using Light = struct {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 attenuation; // kc, kl, kq
};

const GLuint POSITION = 0, COLOR = 1, UBO_BP = 0;

const int MAX_LIGHTS = 10;

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, SPEED_UP };
