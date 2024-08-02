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

    DestructibleObject(const Mesh& mesh, const glm::vec3& position)
        : mesh(mesh), position(position) {}

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