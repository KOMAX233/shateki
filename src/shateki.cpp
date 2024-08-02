#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "common/shader.hpp"
#include "VoronoiDiagram.h"
#include "Mesh.h"
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

    window = glfwCreateWindow(800, 600, "Shateki!!!!!", NULL, NULL);
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
    glEnable(GL_DEPTH_TEST);

    // Define square vertices and indices
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

    Mesh squareMesh(squareVertices, squareIndices);

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
        //cout << edge.VertexA.x << ", " << edge.VertexA.y << endl;
        edgeVertices.push_back(Vertex(glm::vec3(edge.VertexB.x, edge.VertexB.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        //cout << edge.VertexB.x << ", " << edge.VertexB.y << endl;
    }

    Mesh edgeMesh(edgeVertices, {});

    GLuint squareShader = LoadShaders("RectangleVertexShader.vertexshader", "RectangleFragmentShader.fragmentshader");
    GLuint pointShader = LoadShaders("PointVertexShader.vertexshader", "PointFragmentShader.fragmentshader");
    GLuint edgeShader = LoadShaders("EdgeVertexShader.vertexshader", "EdgeFragmentShader.fragmentshader");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);

        // Draw square
        glUseProgram(squareShader);
        glUniformMatrix4fv(glGetUniformLocation(squareShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(squareShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(squareShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        squareMesh.Draw(squareShader);

        // Draw points
        glDisable(GL_DEPTH_TEST);
        glUseProgram(pointShader);
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glPointSize(3.0f);
        pointMesh.Draw(pointShader);
        glEnable(GL_DEPTH_TEST);

        // Draw edges
        glDisable(GL_DEPTH_TEST);
        glUseProgram(edgeShader);
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        edgeMesh.Draw(edgeShader);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto point : points) {
        delete point;
    }

    glfwTerminate();
    return 0;
}
