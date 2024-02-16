#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "glm/glm.hpp"
#include "mgl/mgl.hpp"
#include "utils.h"

using CameraViewOptions = struct CameraViewOptions {
  float fov, aspectRatio, nearPlane, farPlane;
};


// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SPEEDFAST = 10.0f;
const float SENSITIVITY = 0.1f;

class Camera {
 public:
  GLuint UboId;
  // camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // euler Angles
  float Yaw;
  float Pitch;
  // camera options
  float MovementSpeed;
  float MouseSensitivity;
  float InitialFov;

  bool forwardMovement = false;
  bool backwardMovement = false;
  bool leftMovement = false;
  bool rightMovement = false;
  bool upMovement = false;
  bool downMovement = false;
  bool speedUpMovement = false;

  CameraViewOptions cameraViewOptions;


  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
               float pitch = PITCH);

  ~Camera();

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  glm::mat4 GetViewMatrix();

  // processes input received from any keyboard-like input system. Accepts input
  // parameter in the form of camera defined ENUM (to abstract it from windowing
  // systems)
  void ProcessCameraMovement(float deltaTime);

  void ProcessKeyboardInput(Camera_Movement direction);

  // processes input received from a mouse input system. Expects the offset
  // value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset,
                            GLboolean constrainPitch = true);

  // processes input received from a mouse scroll-wheel event. Only requires
  // input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset);

  void SetCameraViewOptionsPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
  
  void initUBO();

  void windowResize(int width, int height);

 private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors();
  void setViewMatrixUBO();
  void setProjectionMatrixUBO();
  void setPositionUBO();
};
