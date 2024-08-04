#ifndef DESTRUCTIBLE_OBJECT_H
#define DESTRUCTIBLE_OBJECT_H

#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class DestructibleObject {
public:
    Mesh mesh;
    glm::vec3 position;
    glm::vec3 velocity;

    DestructibleObject(const Mesh& mesh, const glm::vec3& position, const glm::vec3& velocity)
        : mesh(mesh), position(position), velocity(velocity) {}

    void Update(float deltaTime) {
        velocity.y -= 0.01f * deltaTime; // Apply gravity
        position += velocity * deltaTime;
    }

    void Draw(GLuint shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
        glUseProgram(shader);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        mesh.Draw(shader);
    }
};

#endif
