#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getCameraPosition();
glm::vec3 getCameraDirection();

void setCameraPosition(const glm::vec3& newPosition);
void setCameraDirection(float newHorizontalAngle, float newVerticalAngle);

#endif