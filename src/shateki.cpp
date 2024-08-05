#include <iostream>
#include <vector>
#include <ctime>
#define _USE_MATH_DEFINES
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
float fps = 120.0f;

// task 3
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
        // cout << "Region " << i << " with site (" << site->p.x << ", " << site->p.y << "):" << endl;
        const jcv_graphedge* edge = site->edges;
        while (edge) {
            // cout << "  Edge from (" << edge->pos[0].x << ", " << edge->pos[0].y << ") to ("
            //     << edge->pos[1].x << ", " << edge->pos[1].y << ")" << endl;
            edge = edge->next;
        }
    }
}

// task 8
float getMass(const std::vector<glm::vec2>& vertices) {
    float area = 0.0f;
    int n = vertices.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        area += vertices[i].x * vertices[j].y;
        area -= vertices[j].x * vertices[i].y;
    }
    float mass = std::abs(area) / 2.0f;
    return mass;
}

Mesh GenerateMesh(const std::vector<glm::vec2>& pointList, float scale) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // task 5
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

    // task 7
    // front polygon
    if (pointList.size() > 2) {
        for (size_t i = 1; i < pointList.size() - 1; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    // back polygon
    size_t offset = pointList.size();
    if (pointList.size() > 2) {
        for (size_t i = 1; i < pointList.size() - 1; ++i) {
            indices.push_back(offset);
            indices.push_back(offset + i + 1);
            indices.push_back(offset + i);
        }
    }

    // task 6
    // Gap polygons indices
    for (size_t i = 0; i < pointList.size(); ++i) {
        size_t j = (i + 1) % pointList.size();

        size_t frontOffset = 0;
        size_t backOffset = pointList.size();

        indices.push_back(frontOffset + i);
        indices.push_back(backOffset + i);
        indices.push_back(backOffset + j);

        indices.push_back(frontOffset + i);
        indices.push_back(backOffset + j);
        indices.push_back(frontOffset + j);
    }

    return Mesh(vertices, indices);
}

bool isOutsideBoundingBox(const glm::vec3& position, float boxSize) {
    return position.x < -boxSize || position.x > boxSize ||
        position.y < -boxSize || position.y > boxSize ||
        position.z < -boxSize || position.z > boxSize;
}

void drawAimingDot(GLuint shader) {
    glm::vec2 aimingDotPosition(0.0f, 0.0f);
    glm::vec2 dotSize(0.005f, 0.005f);
    std::vector<Vertex> dotVertices = {
        Vertex(glm::vec3(aimingDotPosition.x - dotSize.x, aimingDotPosition.y - dotSize.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(aimingDotPosition.x + dotSize.x, aimingDotPosition.y - dotSize.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(aimingDotPosition.x + dotSize.x, aimingDotPosition.y + dotSize.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(aimingDotPosition.x - dotSize.x, aimingDotPosition.y + dotSize.y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f))
    };

    std::vector<unsigned int> dotIndices = {
        0, 1, 2, 2, 3, 0
    };

    Mesh dotMesh(dotVertices, dotIndices);

    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    glm::mat4 ProjectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

    dotMesh.Draw(shader);
}

// task 3
void createVoronoiFromImpact(vector<DestructibleObject>& destructibleObjects, const glm::vec3& impactPoint, float halfSquareSize, const Mesh& squareMesh, const glm::vec3& bulletVelocity, float bulletMass, Mesh& pointMesh, Mesh& edgeMesh) {
    // generate 50 random points as sites
    float pointRange = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        // task 2
        pointRange = 0.5f;
    }
    vector<VoronoiPoint*> points = generateRandomPoints(50, impactPoint.x - pointRange, impactPoint.x + pointRange, impactPoint.y - pointRange, impactPoint.y + pointRange);
    
    vector<Vertex> pointVertices;
    for (const auto& point : points) {
        pointVertices.push_back(Vertex(glm::vec3(point->x, point->y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
    }
    pointMesh = Mesh(pointVertices, {}, true);

    jcv_rect bounding_box;
    bounding_box.min.x = -halfSquareSize;
    bounding_box.min.y = -halfSquareSize;
    bounding_box.max.x = halfSquareSize;
    bounding_box.max.y = halfSquareSize;

    std::vector<jcv_point> jcv_points(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        jcv_points[i].x = points[i]->x;
        jcv_points[i].y = points[i]->y;
    }

    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(static_cast<int>(jcv_points.size()), &jcv_points[0], &bounding_box, nullptr, &diagram);

    printVoronoiRegions(diagram);

    vector<Vertex> edgeVertices;
    const jcv_edge* edge = jcv_diagram_get_edges(&diagram);
    while (edge) {
        edgeVertices.push_back(Vertex(glm::vec3(edge->pos[0].x, edge->pos[0].y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edgeVertices.push_back(Vertex(glm::vec3(edge->pos[1].x, edge->pos[1].y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        edge = edge->next;
    }

    edgeMesh = Mesh(edgeVertices, {});

    destructibleObjects.clear();
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
        // bool moveLeft = true, moveRight = true, moveUp = true, moveDown = true;
        // for (const auto& vertex : regionVertices) {
        //     if (vertex.x >= 0) moveLeft = false;
        //     if (vertex.x <= 0) moveRight = false;
        //     if (vertex.y <= 0) moveUp = false;
        //     if (vertex.y >= 0) moveDown = false;
        // }

        // float moveX = 0.0f, moveY = 0.0f;
        // if (moveLeft) moveX = -0.01f;
        // if (moveRight) moveX = 0.01f;
        // if (moveUp) moveY = 0.01f;
        // if (moveDown) moveY = -0.01f;

        // for (auto& vertex : regionVertices) {
        //     vertex.x += moveX;
        //     vertex.y += moveY;
        // }

        Mesh regionMesh = GenerateMesh(regionVertices, 0.1f);
        glm::vec3 position(0.0f, 0.0f, 0.0f);

        float fragmentMass = getMass(regionVertices);
        glm::vec3 velocity = bulletVelocity * (bulletMass / fragmentMass) * 0.1f; // Scaling down the velocity for more realistic effect

        destructibleObjects.push_back(DestructibleObject(regionMesh, position, velocity, fragmentMass, true)); // Set affectedByGravity to true

        // Print mass of each fragment
        // cout << "Mass of fragment " << i << ": " << fragmentMass << endl;
    }

    jcv_diagram_free(&diagram);

    for (auto point : points) {
        delete point;
    }
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
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        2, 3, 7, 7, 6, 2,
        0, 3, 7, 7, 4, 0,
        1, 2, 6, 6, 5, 1
    };

    vector<glm::vec2> square2DVertices = {
        glm::vec2(-halfSquareSize, -halfSquareSize),
        glm::vec2(halfSquareSize, -halfSquareSize),
        glm::vec2(halfSquareSize, halfSquareSize),
        glm::vec2(-halfSquareSize, halfSquareSize)
    };

    Mesh squareMesh = GenerateMesh(square2DVertices, 0.1f);

    DestructibleObject Square3D(squareMesh, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), getMass(square2DVertices), false);

    // task 1
    vector<glm::vec2> bulletVertices = {
        glm::vec2(-halfSquareSize / 100, -halfSquareSize / 100),
        glm::vec2(halfSquareSize / 100, -halfSquareSize / 100),
        glm::vec2(halfSquareSize / 100, halfSquareSize / 100),
        glm::vec2(-halfSquareSize / 100, halfSquareSize / 100)
    };

    Mesh bulletMesh = GenerateMesh(bulletVertices, -halfSquareSize / 100);

    vector<DestructibleObject> bullets;

    GLuint squareShader = LoadShaders("../shaders/RectangleVertexShader.vertexshader", "../shaders/RectangleFragmentShader.fragmentshader");
    GLuint pointShader = LoadShaders("../shaders/PointVertexShader.vertexshader", "../shaders/PointFragmentShader.fragmentshader");
    GLuint edgeShader = LoadShaders("../shaders/EdgeVertexShader.vertexshader", "../shaders/EdgeFragmentShader.fragmentshader");
    GLuint sphereShader = LoadShaders("../shaders/EdgeVertexShader.vertexshader", "../shaders/EdgeFragmentShader.fragmentshader");
    GLuint dotShader = LoadShaders("../shaders/2dVertexShader.vertexshader", "../shaders/2dFragmentShader.fragmentshader");

    bool voronoiGenerated = false;
    vector<DestructibleObject> destructibleObjects; // To store the destructible objects
    float boundingBoxSize = 10.0f; // Bounding box size
    bool collisionDetected = false; // Track collision

    // task 3
    // Define point and edge mesh outside the loop
    Mesh pointMesh;
    Mesh edgeMesh;

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::vec3 cameraPosition = getCameraPosition();
        glm::vec3 bulletPosition = cameraPosition + glm::normalize(getCameraDirection()) * 1.0f;
        glm::vec3 cameraDirection = getCameraDirection();
        glm::mat4 ModelMatrix = glm::mat4(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // task 1
        // Check for left mouse click to launch a new bullet
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glm::vec3 bulletVelocity = cameraDirection * 10.0f; // Adjust the speed as necessary
            bullets.push_back(DestructibleObject(bulletMesh, bulletPosition, bulletVelocity, 0.05f, false));
            for (auto& destructibleObject : destructibleObjects) {
                destructibleObject.affectedByGravity = true;
            }
            Square3D.affectedByGravity = true;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            destructibleObjects.clear();
            voronoiGenerated = false;
            Square3D = DestructibleObject(squareMesh, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), getMass(square2DVertices), false);
            collisionDetected = false;
            setCameraPosition(glm::vec3(0, 0, 3));
            setCameraDirection(3.14f, 0.0f);
        }

        // task 1
        // Update and draw bullets
        for (auto it = bullets.begin(); it != bullets.end();) {
            it->Update(1.0f / fps);
            if (isOutsideBoundingBox(it->position, boundingBoxSize)) {
                it = bullets.erase(it);
            }
            else {
                it->Draw(sphereShader, ViewMatrix, ProjectionMatrix);
                ++it;
            }
        }

        drawAimingDot(dotShader);

        // task 1
        // Check for collision between bullets and the original square
        if (!collisionDetected) {
            for (auto& bullet : bullets) {
                if (Square3D.CheckCollision(bullet)) {
                    collisionDetected = true; // Stop checking further collisions
                    voronoiGenerated = true;
                    createVoronoiFromImpact(destructibleObjects, bullet.position, halfSquareSize, squareMesh, bullet.velocity, bullet.mass, pointMesh, edgeMesh);
                    break;
                }
            }
        }

        // task 8
        // Update positions of destructible objects
        float deltaTime = 1.0f / fps;
        for (auto it = destructibleObjects.begin(); it != destructibleObjects.end();) {
            it->Update(deltaTime);
            if (isOutsideBoundingBox(it->position, boundingBoxSize)) {
                it = destructibleObjects.erase(it); // Remove fragment if outside bounding box
            }
            else {
                it->Draw(squareShader, ViewMatrix, ProjectionMatrix);
                ++it;
            }
        }

        // task 7
        // Draw square
        glUseProgram(squareShader);
        glUniformMatrix4fv(glGetUniformLocation(squareShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(squareShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(squareShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            squareMesh.Draw(squareShader);
        }

        // task 3
        // Draw points
        glDisable(GL_DEPTH_TEST);
        glUseProgram(pointShader);
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(pointShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        glPointSize(3.0f);
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            pointMesh.Draw(pointShader);
        }
        glEnable(GL_DEPTH_TEST);

        // task 3
        // Draw edges
        glDisable(GL_DEPTH_TEST);
        glUseProgram(edgeShader);
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(edgeShader, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            edgeMesh.Draw(edgeShader);
        }
        glEnable(GL_DEPTH_TEST);

        // task 7
        if (!voronoiGenerated) {
            Square3D.Draw(squareShader, ViewMatrix, ProjectionMatrix);
        }
        else {
            for (auto& destructibleObject : destructibleObjects) {
                destructibleObject.Draw(squareShader, ViewMatrix, ProjectionMatrix);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}