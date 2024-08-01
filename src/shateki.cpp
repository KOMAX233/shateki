#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/shader.hpp>
#include "VoronoiDiagram.h"

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

void drawSquare(GLuint programID, GLuint vao) {
    glUseProgram(programID);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void drawVoronoiEdges(GLuint programID, GLuint vaoEdge, GLsizei edgeCount) {
    glUseProgram(programID);
    glBindVertexArray(vaoEdge);
    glDrawArrays(GL_LINES, 0, edgeCount);
    glBindVertexArray(0);
    glUseProgram(0);
}

void drawPoints(GLuint programID, GLuint vaoPoint, GLuint vertexbufferPoint, const vector<glm::vec3>& points) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPoint);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    glUseProgram(programID);
    glBindVertexArray(vaoPoint);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, points.size());
    glBindVertexArray(0);
    glUseProgram(0);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Shateki!!!!!", NULL, NULL);
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

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glDisable(GL_DEPTH_TEST);  // Disable depth testing to ensure all elements are rendered
    glEnable(GL_CULL_FACE);

    GLuint vaoSquare, vaoPoint, vaoEdge;

    // Square
    glGenVertexArrays(1, &vaoSquare);
    glBindVertexArray(vaoSquare);

    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    static const GLuint g_index_buffer_data[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint vertexbufferSquare, elementbufferSquare;
    glGenBuffers(1, &vertexbufferSquare);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferSquare);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbufferSquare);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferSquare);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_index_buffer_data), g_index_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferSquare);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferSquare);
    glBindVertexArray(0);

    // Points
    glGenVertexArrays(1, &vaoPoint);
    glBindVertexArray(vaoPoint);

    GLuint vertexbufferPoint;
    glGenBuffers(1, &vertexbufferPoint);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPoint);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPoint);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    glBindVertexArray(0);

    // task 3
    // Generate Voronoi points
    vector<VoronoiPoint*> points;
    int numVoronoiPoints = 20;
    // move towards impact point 
    VoronoiPoint impactP = { 0, 0 };
    // generate random points and limit them within 0.1 range in the impact point
    for (auto point : generateRandomPoints(numVoronoiPoints, impactP.x - 0.1, impactP.x + 0.1, impactP.y - 0.1, impactP.y + 0.1)) {
        points.push_back(point);
    }

    // Create Voronoi diagram
    Voronoi voronoi;
    vector<VEdge> edges = voronoi.ComputeVoronoiGraph(points, -0.5, 0.5);

    // Convert edges to glm::vec3 for OpenGL
    std::vector<glm::vec3> edgeVertices;
    for (const auto& edge : edges) {
        edgeVertices.push_back(glm::vec3(edge.VertexA.x, edge.VertexA.y, 0.0f));
        edgeVertices.push_back(glm::vec3(edge.VertexB.x, edge.VertexB.y, 0.0f));
    }

    // Create and bind the VAO for the edges
    glGenVertexArrays(1, &vaoEdge);
    glBindVertexArray(vaoEdge);

    GLuint vertexbufferEdge;
    glGenBuffers(1, &vertexbufferEdge);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferEdge);

    glBufferData(GL_ARRAY_BUFFER, edgeVertices.size() * sizeof(glm::vec3), edgeVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );

    glBindVertexArray(0);

    GLuint rectangleProgramID = LoadShaders("RectangleVertexShader.vertexshader", "RectangleFragmentShader.fragmentshader");
    GLuint pointProgramID = LoadShaders("PointVertexShader.vertexshader", "PointFragmentShader.fragmentshader");
    GLuint EdgeProgramID = LoadShaders("EdgeVertexShader.vertexshader", "EdgeFragmentShader.fragmentshader");

    // Convert points to glm::vec3
    std::vector<glm::vec3> pointVertices;
    for (const auto& point : points) {
        pointVertices.push_back(glm::vec3(point->x, point->y, 0.0f));
    }

    // Main loop
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the square
        drawSquare(rectangleProgramID, vaoSquare);

        // Draw the points within the rectangle
        drawPoints(pointProgramID, vaoPoint, vertexbufferPoint, pointVertices);

        // Draw the Voronoi edges
        drawVoronoiEdges(EdgeProgramID, vaoEdge, edgeVertices.size());

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Clean up
    for (auto point : points) {
        delete point;
    }

    glfwTerminate();
    return 0;
}
