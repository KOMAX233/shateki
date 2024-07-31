#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <common/shader.hpp>
#include "VoronoiDiagram.h"

using namespace std;

GLFWwindow* window;

// Function to generate random points
vector<VoronoiPoint*> generateRandomPoints(int numPoints, float minX, float maxX, float minY, float maxY) {
    vector<VoronoiPoint*> points;
    // Use current time as seed for random generator 
    srand(time(0));
    for (int i = 0; i < numPoints; ++i) {
        float x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        float y = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
        points.push_back(new VoronoiPoint(x, y));
        cout << x << "," << y << endl;
    }
    return points;
}

// Function to draw the square
void drawSquare(GLuint programID, GLuint vao) {
    glUseProgram(programID);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

// Function to draw Voronoi edges
void drawVoronoiEdges(GLuint programID, const vector<VEdge>& edges) {
    glUseProgram(programID);
    glBegin(GL_LINES);
    for (const auto& edge : edges) {
        glVertex2d(edge.VertexA.x, edge.VertexA.y);
        glVertex2d(edge.VertexB.x, edge.VertexB.y);
    }
    glEnd();
    glUseProgram(0);
}

// Function to draw red points within the rectangle
void drawPoints(GLuint programID, GLuint vaoPoint, GLuint vertexbufferPoint, const std::vector<glm::vec3>& points) {
    // Update the buffer data with the points
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPoint);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    // Use the program
    glUseProgram(programID);

    // Bind the VAO for the points
    glBindVertexArray(vaoPoint);

    glPointSize(10.0f); // Set point size
    glDrawArrays(GL_POINTS, 0, points.size()); // Draw the points

    // Unbind the VAO
    glBindVertexArray(0);

    // Unuse the program
    glUseProgram(0);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW\n";
        return -1;
    }


    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Shateki!!!!!", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // Generate Voronoi points
    vector<VoronoiPoint*> points;
    int numVoronoiPoints = 5;
    for (auto point : generateRandomPoints(numVoronoiPoints, -0.5f, 0.5f, -0.5f, 0.5f)) {
        points.push_back(point);
    }

    // Create Voronoi diagram
    Voronoi voronoi;
    vector<VEdge> edges = voronoi.ComputeVoronoiGraph(points, -0.5, 0.5);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw Voronoi diagram
        glBegin(GL_LINES);
        for (const auto& edge : edges) {
            glVertex2d(edge.VertexA.x, edge.VertexA.y);
            glVertex2d(edge.VertexB.x, edge.VertexB.y);
        }
        glEnd();

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
