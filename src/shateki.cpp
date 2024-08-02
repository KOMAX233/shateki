#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "common/shader.hpp"
#include "common/controls.hpp"
#include "VoronoiDiagram.h"
#include "Mesh.h"
#include "DestructibleObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

GLFWwindow* window;

vector<VoronoiPoint*> generateRandomPoints(int numPoints, float minX, float maxX, float minY, float maxY) {
    srand(time(0));
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
    for (size_t i = 1; i < pointList.size() - 1; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Back polygon indices (triangulation)
    size_t offset = pointList.size();
    for (size_t i = 1; i < pointList.size() - 1; ++i) {
        indices.push_back(offset);
        indices.push_back(offset + i + 1);
        indices.push_back(offset + i);
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

	// 2D plane vertices and indices
    vector<Vertex> squareVertices = {
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f))
    };

    vector<unsigned int> squareIndices = {
        0, 1, 2,
        2, 3, 0
    };
    // Define square vertices and indices (3D cube)
    // vector<Vertex> squareVertices = {
    //     Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
    //     Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
    //     Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
    //     Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
    //     Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
    //     Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
    //     Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
    //     Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f))
    // };

    // vector<unsigned int> squareIndices = {
    //     0, 1, 2, 2, 3, 0, // Back face
    //     4, 5, 6, 6, 7, 4, // Front face
    //     0, 1, 5, 5, 4, 0, // Bottom face
    //     2, 3, 7, 7, 6, 2, // Top face
    //     0, 3, 7, 7, 4, 0, // Left face
    //     1, 2, 6, 6, 5, 1  // Right face
    // };

    Mesh squareMesh(squareVertices, squareIndices);
    DestructibleObject originalSquare(squareMesh, glm::vec3(0.0f, 0.0f, 0.0f));

    // cout << "Original Square Vertices:" << endl;
    // printVertices(squareVertices);

    // Define point vertices
    vector<Vertex> pointVertices;
    // task 3
    // Generate Voronoi points
	// generate random points and limit them within 0.1 range in the impact point
    // move towards impact point 
    VoronoiPoint impactP = { 0, 0 };
	// generate random points and limit them within 0.1 range in the impact point
	vector<VoronoiPoint*> points = generateRandomPoints(5, impactP.x - 0.1, impactP.x + 0.1, impactP.y - 0.1, impactP.y + 0.1);
    for (const auto& point : points) {
        pointVertices.push_back(Vertex(glm::vec3(point->x, point->y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
    }

    Mesh pointMesh(pointVertices, {}, true);

    // Generate Voronoi edges
    Voronoi voronoi;
    vector<VEdge> edges = voronoi.ComputeVoronoiGraph(points, -0.5, 0.5);

    vector<Vertex> edgeVertices;
    for (const auto& edge : edges) {
        edgeVertices.push_back(Vertex(glm::vec3(edge.VertexA.x, edge.VertexA.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edgeVertices.push_back(Vertex(glm::vec3(edge.VertexB.x, edge.VertexB.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
    }
    
	// Create destructible objects for each site in the Voronoi diagram
    vector<DestructibleObject> destructibleObjects;
	for (const auto& edge : edges) {
        vector<glm::vec2> siteVertices;
        siteVertices.push_back(glm::vec2(edge.VertexA.x, edge.VertexA.y));
		siteVertices.push_back(glm::vec2(edge.VertexB.x, edge.VertexB.y));
		
        Mesh siteMesh = GenerateMesh(siteVertices, 0.1f);
        destructibleObjects.push_back(DestructibleObject(siteMesh, glm::vec3(0.0f, 0.0f, 0.0f)));

        //cout << "New Destructible Object Vertices:" << endl;
        //printVertices(meshVertices);
    }

    // Creating the edges for the original square's front and back faces
    for (const auto& edge : edges) {
        edgeVertices.push_back(Vertex(glm::vec3(edge.VertexA.x, edge.VertexA.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edgeVertices.push_back(Vertex(glm::vec3(edge.VertexB.x, edge.VertexB.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edgeVertices.push_back(Vertex(glm::vec3(edge.VertexA.x, edge.VertexA.y, -0.1f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edgeVertices.push_back(Vertex(glm::vec3(edge.VertexB.x, edge.VertexB.y, -0.1f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
    }

    Mesh edgeMesh(edgeVertices, {});

    GLuint squareShader = LoadShaders("RectangleVertexShader.vertexshader", "RectangleFragmentShader.fragmentshader");
    GLuint pointShader = LoadShaders("PointVertexShader.vertexshader", "PointFragmentShader.fragmentshader");
    GLuint edgeShader = LoadShaders("EdgeVertexShader.vertexshader", "EdgeFragmentShader.fragmentshader");

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // // Draw square
        // glUseProgram(squareShader);
        // glUniformMatrix4fv(glGetUniformLocation(squareShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        // glUniformMatrix4fv(glGetUniformLocation(squareShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        // glUniformMatrix4fv(glGetUniformLocation(squareShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        // squareMesh.Draw(squareShader);

        // // Draw points
        // glDisable(GL_DEPTH_TEST);
        // glUseProgram(pointShader);
        // glUniformMatrix4fv(glGetUniformLocation(pointShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        // glUniformMatrix4fv(glGetUniformLocation(pointShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        // glUniformMatrix4fv(glGetUniformLocation(pointShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        // glPointSize(3.0f);
        // pointMesh.Draw(pointShader);
        // glEnable(GL_DEPTH_TEST);

        // // Draw edges
        // glDisable(GL_DEPTH_TEST);
        // glUseProgram(edgeShader);
        // glUniformMatrix4fv(glGetUniformLocation(edgeShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        // glUniformMatrix4fv(glGetUniformLocation(edgeShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        // glUniformMatrix4fv(glGetUniformLocation(edgeShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        // edgeMesh.Draw(edgeShader);
        // glEnable(GL_DEPTH_TEST);

        // Draw the original square
        originalSquare.Draw(squareShader, ViewMatrix, ProjectionMatrix);

		// Draw the destructible objects
        for (auto& destructibleObject : destructibleObjects) {
            destructibleObject.Draw(edgeShader, ViewMatrix, ProjectionMatrix);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto point : points) {
        delete point;
    }

    glfwTerminate();
    return 0;
}
