#include "camera.h"

#include <glad/glad.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

static void updateCameraVectors(Camera &cam);

//constructor with vectors
Camera::Camera(glm::vec3 position ,
       glm::vec3 up ,
       float yaw , float pitch ) 
: Front(0.0f, 0.0f, -1.0f), MovementSpeed(SPEED),
  MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
  Position = position;
  WorldUp = up;
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors(*this); //will set Front Right and Up for us
}

//constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX,
       float upY, float upZ, float yaw, float pitch)
: Front(0.0f, 0.0f, -1.0f), MovementSpeed(SPEED),
  MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
  Position = glm::vec3(posX, posY, posZ);
  WorldUp = glm::vec3(upX, upY, upX);
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors(*this); //will set Front Right and Up
}

//return the view matrix calcluted from Euler Angles and LookAt matrix
glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(Position,  Position + Front, Up);
}

// processes input received from any keyboard-like input system.
// Accepts input parameter in the form of camera defined ENUM
// (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity{deltaTime * MovementSpeed};
  switch (direction){
    case FORWARD:
      Position += Front * velocity;
      break;
    case BACKWARD:
      Position -= Front * velocity;
      break;
    case LEFT:
      Position -= Right * velocity;
      break;
    case RIGHT:
      Position += Right * velocity;
      break;
  }
}

// processes input received from a mouse input system.
// Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                          GLboolean constrainPitch) 
{
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;

  Yaw += xoffset;
  Pitch += yoffset;

  if (constrainPitch) {
    Pitch = (Pitch > 89.f) ? 89.f : Pitch; 
    Pitch = (Pitch < -89.f) ? -89.f : Pitch; 
  }

  updateCameraVectors(*this);
}

// processes input received from a mouse scroll-wheel event.
// Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
  Zoom -= yoffset;
  Zoom = (Zoom < 1.0f) ? 1.0f : Zoom;
  Zoom = (Zoom > 45.f) ? 45.f : Zoom;
};

// calculates the front, right, and up vector from the Camera's (updated)
// Euler Angles
static void updateCameraVectors(Camera& cam) {
  glm::vec3 front;
  front.x = cos(glm::radians(cam.Yaw)) * cos(glm::radians(cam.Pitch));
  front.y = sin(glm::radians(cam.Pitch));
  front.z = sin(glm::radians(cam.Yaw));
  cam.Front = glm::normalize(front);
  cam.Right = glm::normalize(glm::cross(cam.Front, cam.WorldUp));
  cam.Up = glm::normalize(glm::cross(cam.Right, cam.Front));
}
