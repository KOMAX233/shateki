// task 7
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "common/shader.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TexCoords;

    Vertex(glm::vec3 position, glm::vec3 color, glm::vec2 texCoords)
        : Position(position), Color(color), TexCoords(texCoords) {}
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO;
    bool drawAsPoints;

    Mesh() = default;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool drawAsPoints = false);

    void Draw(GLuint shader);

private:
    GLuint VBO, EBO;

    void setupMesh();
};

#endif
