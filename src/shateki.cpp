#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common/shader.hpp"
#include "common/controls.hpp"
#include "Mesh.h"
#include "DestructibleObject.h"

#define JC_VORONOI_IMPLEMENTATION
#define JC_VORONOI_CLIP_IMPLEMENTATION
#include "VoronoiDiagram.h"
#include "VoronoiDiagramClip.h"

using namespace std;

GLFWwindow* window;

struct VoronoiPoint {
    float x, y;
    VoronoiPoint(float x, float y) : x(x), y(y) {}
};

vector<VoronoiPoint*> generateRandomPoints(int numPoints, float minX, float maxX, float minY, float maxY) {
    srand(static_cast<unsigned>(time(0)));
    vector<VoronoiPoint*> points;
    for (int i = 0; i < numPoints; ++i) {
        float x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        float y = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
        points.push_back(new VoronoiPoint(x, y));
    }
    return points;
}

void printVertices(const vector<Vertex>& vertices) {
    for (const auto& vertex : vertices) {
        cout << "Vertex Position: (" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z << ")" << endl;
    }
}

void printVoronoiRegions(const jcv_diagram& diagram) {
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    for (int i = 0; i < diagram.numsites; ++i) {
        const jcv_site* site = &sites[i];
        cout << "Region " << i << " with site (" << site->p.x << ", " << site->p.y << "):" << endl;
        const jcv_graphedge* edge = site->edges;
        while (edge) {
            cout << "  Edge from (" << edge->pos[0].x << ", " << edge->pos[0].y << ") to ("
                << edge->pos[1].x << ", " << edge->pos[1].y << ")" << endl;
            edge = edge->next;
        }
    }
}

Mesh GenerateMesh(const std::vector<glm::vec2>& pointList, float scale) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Front polygon at z=scale
    for (const auto& point : pointList) {
        Vertex v(glm::vec3(point.x, point.y, scale), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
        vertices.push_back(v);
    }

    // Back polygon at z=-scale
    for (const auto& point : pointList) {
        Vertex v(glm::vec3(point.x, point.y, -scale), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
        vertices.push_back(v);
    }

    // Front polygon indices (triangulation)
    if (pointList.size() > 2) {
        for (size_t i = 1; i < pointList.size() - 1; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    // Back polygon indices (triangulation)
    size_t offset = pointList.size();
    if (pointList.size() > 2) {
        for (size_t i = 1; i < pointList.size() - 1; ++i) {
            indices.push_back(offset);
            indices.push_back(offset + i + 1);
            indices.push_back(offset + i);
        }
    }

    // Gap polygons indices
    for (size_t i = 0; i < pointList.size(); ++i) {
        size_t j = (i + 1) % pointList.size();

        size_t frontOffset = 0;
        size_t backOffset = pointList.size();

        // Two triangles for each quad
        indices.push_back(frontOffset + i);
        indices.push_back(backOffset + i);
        indices.push_back(backOffset + j);

        indices.push_back(frontOffset + i);
        indices.push_back(backOffset + j);
        indices.push_back(frontOffset + j);
    }

    return Mesh(vertices, indices);
}

int main() {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "Shateki!!!!!", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW\n";
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // 2D plane vertices and indices for the initial square
    float squareSize = 1.0f;
    float halfSquareSize = squareSize / 2.0f;
    vector<Vertex> squareVertices = {
        Vertex(glm::vec3(-halfSquareSize, -halfSquareSize, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(halfSquareSize, -halfSquareSize, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(halfSquareSize, halfSquareSize, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-halfSquareSize, halfSquareSize, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(-halfSquareSize, -halfSquareSize, -0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(halfSquareSize, -halfSquareSize, -0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(halfSquareSize, halfSquareSize, -0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-halfSquareSize, halfSquareSize, -0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f))
    };

    vector<unsigned int> squareIndices = {
        0, 1, 2, 2, 3, 0, // front
        4, 5, 6, 6, 7, 4, // back
        0, 1, 5, 5, 4, 0, // bottom
        2, 3, 7, 7, 6, 2, // top
        0, 3, 7, 7, 4, 0, // left
        1, 2, 6, 6, 5, 1  // right
    };

    Mesh squareMesh = GenerateMesh({
        glm::vec2(-halfSquareSize, -halfSquareSize),
        glm::vec2(halfSquareSize, -halfSquareSize),
        glm::vec2(halfSquareSize, halfSquareSize),
        glm::vec2(-halfSquareSize, halfSquareSize)
        }, 0.1f);

    DestructibleObject originalSquare(squareMesh, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    // Define point vertices
    vector<Vertex> pointVertices;
    VoronoiPoint impactP = { 0, 0 };
    vector<VoronoiPoint*> points = generateRandomPoints(5, impactP.x - 0.1, impactP.x + 0.1, impactP.y - 0.1, impactP.y + 0.1);
    for (const auto& point : points) {
        pointVertices.push_back(Vertex(glm::vec3(point->x, point->y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
    }

    Mesh pointMesh(pointVertices, {}, true);

    // Define the bounding box (square)
    jcv_rect bounding_box;
    bounding_box.min.x = -halfSquareSize;
    bounding_box.min.y = -halfSquareSize;
    bounding_box.max.x = halfSquareSize;
    bounding_box.max.y = halfSquareSize;

    // Convert points to jcv_point
    std::vector<jcv_point> jcv_points(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        jcv_points[i].x = points[i]->x;
        jcv_points[i].y = points[i]->y;
    }

    // Generate Voronoi diagram
    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(static_cast<int>(jcv_points.size()), &jcv_points[0], &bounding_box, nullptr, &diagram);

    // Print Voronoi regions
    printVoronoiRegions(diagram);

    // Generate Voronoi edges
    vector<Vertex> edgeVertices;
    const jcv_edge* edge = jcv_diagram_get_edges(&diagram);
    while (edge) {
        edgeVertices.push_back(Vertex(glm::vec3(edge->pos[0].x, edge->pos[0].y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edgeVertices.push_back(Vertex(glm::vec3(edge->pos[1].x, edge->pos[1].y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edge = edge->next;
    }

    Mesh edgeMesh(edgeVertices, {});

    GLuint squareShader = LoadShaders("RectangleVertexShader.vertexshader", "RectangleFragmentShader.fragmentshader");
    GLuint pointShader = LoadShaders("PointVertexShader.vertexshader", "PointFragmentShader.fragmentshader");
    GLuint edgeShader = LoadShaders("EdgeVertexShader.vertexshader", "EdgeFragmentShader.fragmentshader");

    bool voronoiGenerated = false;
    vector<DestructibleObject> destructibleObjects; // To store the destructible objects

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Check for left mouse click to generate Voronoi diagram
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !voronoiGenerated) {
            voronoiGenerated = true;

            // Generate Voronoi diagram and create new destructible objects
            jcv_diagram_generate(static_cast<int>(jcv_points.size()), &jcv_points[0], &bounding_box, nullptr, &diagram);
            destructibleObjects.clear(); // Clear old objects

            const jcv_site* sites = jcv_diagram_get_sites(&diagram);
            for (int i = 0; i < diagram.numsites; ++i) {
                const jcv_site* site = &sites[i];
                vector<glm::vec2> regionVertices;
                const jcv_graphedge* edge = site->edges;
                while (edge) {
                    regionVertices.push_back(glm::vec2(edge->pos[0].x, edge->pos[0].y));
                    regionVertices.push_back(glm::vec2(edge->pos[1].x, edge->pos[1].y));
                    edge = edge->next;
                }

                // test splitted new objects movement
                // make each piece a little bit away from each other
                // Determine movement based on region's vertices
                bool moveLeft = true, moveRight = true, moveUp = true, moveDown = true;
                for (const auto& vertex : regionVertices) {
                    if (vertex.x >= 0) moveLeft = false;
                    if (vertex.x <= 0) moveRight = false;
                    if (vertex.y <= 0) moveUp = false;
                    if (vertex.y >= 0) moveDown = false;
                }

                float moveX = 0.0f, moveY = 0.0f;
                if (moveLeft) moveX = -0.01f;
                if (moveRight) moveX = 0.01f;
                if (moveUp) moveY = 0.01f;
                if (moveDown) moveY = -0.01f;

                for (auto& vertex : regionVertices) {
                    vertex.x += moveX;
                    vertex.y += moveY;
                }

                Mesh regionMesh = GenerateMesh(regionVertices, 0.1f);
                glm::vec3 position(0.0f, 0.0f, 0.0f);

                // Calculate initial velocity for each piece
                glm::vec3 velocity = glm::vec3(moveX, moveY, 0.0f);

                destructibleObjects.push_back(DestructibleObject(regionMesh, position, velocity));
            }
        }

        // Update positions of destructible objects
        float deltaTime = 1.0f / 120.0f; // 120 FPS
        for (auto& destructibleObject : destructibleObjects) {
            destructibleObject.Update(deltaTime);
        }

        // Draw points
        glDisable(GL_DEPTH_TEST);
        glUseProgram(pointShader);
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        glPointSize(3.0f);
        pointMesh.Draw(pointShader);
        glEnable(GL_DEPTH_TEST);

        // Draw edges
        glDisable(GL_DEPTH_TEST);
        glUseProgram(edgeShader);
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        edgeMesh.Draw(edgeShader);
        glEnable(GL_DEPTH_TEST);

        // Draw the original square or the destructible objects
        if (!voronoiGenerated) {
            originalSquare.Draw(squareShader, ViewMatrix, ProjectionMatrix);
        }
        else {
            // Draw the destructible objects
            for (auto& destructibleObject : destructibleObjects) {
                destructibleObject.Draw(edgeShader, ViewMatrix, ProjectionMatrix);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto point : points) {
        delete point;
    }

    jcv_diagram_free(&diagram);
    glfwTerminate();
    return 0;
}
