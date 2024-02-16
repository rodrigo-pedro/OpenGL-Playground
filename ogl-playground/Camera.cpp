#include "Camera.h"

Camera::Camera(glm::vec3 position,
                           glm::vec3 up,
                           float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY)
  {
  Position = position;
  WorldUp = up;
  Yaw = yaw;
  Pitch = pitch;
  cameraViewOptions = {45.0f, 4.0f / 3.0f, 0.1f, 100.0f};
  InitialFov = cameraViewOptions.fov;

  updateCameraVectors();

  glGenBuffers(1, &UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(glm::vec3), 0, GL_STREAM_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BP, UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Camera::~Camera() {
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glDeleteBuffers(1, &UboId);
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessCameraMovement(float deltaTime) {
  float velocity = MovementSpeed * deltaTime;
  if (forwardMovement) Position += Front * velocity;
  if (backwardMovement) Position -= Front * velocity;
  if (leftMovement) Position -= Right * velocity;
  if (rightMovement) Position += Right * velocity;
  if (upMovement) Position += Up * velocity;
  if (downMovement) Position -= Up * velocity;
  if (speedUpMovement) MovementSpeed = SPEEDFAST;
  else MovementSpeed = SPEED;

  setViewMatrixUBO();
  setPositionUBO();
}

void Camera::ProcessKeyboardInput(Camera_Movement direction) {
  if (direction == FORWARD) forwardMovement = !forwardMovement;
  if (direction == BACKWARD) backwardMovement = !backwardMovement;
  if (direction == LEFT) leftMovement = !leftMovement;
  if (direction == RIGHT) rightMovement = !rightMovement;
  if (direction == UP) upMovement = !upMovement;
  if (direction == DOWN) downMovement = !downMovement;
  if (direction == SPEED_UP) speedUpMovement = !speedUpMovement;
}

// processes input received from a mouse input system. Expects the offset value
// in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                        GLboolean constrainPitch) {
  xoffset *= MouseSensitivity * cameraViewOptions.fov / InitialFov;
  yoffset *= MouseSensitivity * cameraViewOptions.fov / InitialFov;

  Yaw += xoffset;
  Pitch -= yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;
  }

  // update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input
// on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
  cameraViewOptions.fov -= yoffset * 3.0f;
  if (cameraViewOptions.fov < 1.0f) 
    cameraViewOptions.fov = 1.0f;
  if (cameraViewOptions.fov > InitialFov) 
    cameraViewOptions.fov = InitialFov;
  setProjectionMatrixUBO();
}

void Camera::updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));

        setViewMatrixUBO();
    }

void Camera::setViewMatrixUBO() {
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(GetViewMatrix()));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::SetCameraViewOptionsPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
  cameraViewOptions.fov = fov;
  cameraViewOptions.aspectRatio = aspectRatio;
  cameraViewOptions.nearPlane = nearPlane;
  cameraViewOptions.farPlane = farPlane;
  InitialFov = fov;

  setProjectionMatrixUBO();
}

void Camera::setProjectionMatrixUBO() {
  glm::mat4 projectionMatrix =  glm::perspective(
      glm::radians(cameraViewOptions.fov),
      cameraViewOptions.aspectRatio,
      cameraViewOptions.nearPlane,
      cameraViewOptions.farPlane
  );

  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(projectionMatrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::setPositionUBO() {
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3),
                  glm::value_ptr(Position));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::initUBO() {
  setViewMatrixUBO();
  setProjectionMatrixUBO();
  setPositionUBO();
}

void Camera::windowResize(int width, int height) {
  if (width != 0 && height != 0) {
    cameraViewOptions.aspectRatio = (float)width / (float)height;
    setProjectionMatrixUBO();
  }
}
