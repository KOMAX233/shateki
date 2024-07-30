#pragma once

#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <glm/glm.hpp>
#include "VoronoiDiagram.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


using namespace std;

struct Point {
    double x, y;
};

struct Region {
    Point site;
    vector<Point> vertices;
};

struct CircleEvent {
    Point center;
    double radius;
    bool valid;
};

struct GFG {
    Point point;
    int index;
    bool isSite;
    CircleEvent circleEvent;
    bool operator<(const GFG& other) const {
        if (point.x == other.point.x) {
            return (isSite && !other.isSite);
        }
        return point.x < other.point.x;
    }
};

class VoronoiDiagram {
public:
    VoronoiDiagram() = default;

    void construct();
    void computeVoronoi(vector<Point>& points);
    void outputRegions();
    void renderVoronoi();

private:
    vector<Region> voronoiRegions;
    void handleSiteEvent(const GFG& event, priority_queue<GFG>& eventQueue, set<GFG>& beachline);
    void handleCircleEvent(const GFG& event, priority_queue<GFG>& eventQueue, set<GFG>& beachline);
    CircleEvent computeCircumcircle(const Point& a, const Point& b, const Point& c);
};
