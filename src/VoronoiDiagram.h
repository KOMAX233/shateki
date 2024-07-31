#pragma once

#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


using namespace std;
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

struct Point {
    double x, y;
};

// Define a region structure to
// represent Voronoi cells
struct Region {
    Point site;
    vector<Point> vertices;
};

// Event structure to handle events in the
// sweep line algorithm
struct GFG {
    Point point;
    int index;
    bool isSite;
    bool operator<(const GFG& other) const {
        if (point.x == other.point.x) {
            return (isSite && !other.isSite);
        }
        return point.x < other.point.x;
    }
};

// Beachline arc structure
struct Arc {
    Point site;
    Arc* prev;
    Arc* next;
    Point* circleEvent;
};

// Function to construct Voronoi Diagram using
// Sweep Line algorithm
vector<Region> voronoiSweepLine(vector<Point>& points)
{
    int n = points.size();
    vector<Region> regions(n);
    // Sort points by their x-coordinates
    sort(points.begin(), points.end(),
        [](const Point& a, const Point& b) {
            return a.x < b.x;
        });
    set<GFG> eventQueue;
    // Initialize the event queue with the input points
    for (int i = 0; i < n; ++i) {
        eventQueue.insert({ points[i], i, true });
    }
    while (!eventQueue.empty()) {
        GFG currentEvent = *eventQueue.begin();
        eventQueue.erase(eventQueue.begin());

        if (currentEvent.isSite) {
        }
        else {
            // Handle circle event
            // Update Voronoi regions as the
            // sweep line encounters circle events
        }
    }
    return regions;
}

