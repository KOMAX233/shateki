#include "VoronoiDiagram.h"

void VoronoiDiagram::construct() {
    vector<Point> points = { {-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}, {0.0, 0.0} };
    computeVoronoi(points);
    outputRegions();
}

void VoronoiDiagram::computeVoronoi(vector<Point>& points) {
    int n = points.size();
    voronoiRegions.resize(n);

    for (int i = 0; i < n; ++i) {
        voronoiRegions[i].site = points[i];
    }

    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
        });

    priority_queue<GFG> eventQueue;
    for (int i = 0; i < n; ++i) {
        eventQueue.push({ points[i], i, true, {} });
    }

    set<GFG> beachline;

    while (!eventQueue.empty()) {
        GFG currentEvent = eventQueue.top();
        eventQueue.pop();

        if (currentEvent.isSite) {
            handleSiteEvent(currentEvent, eventQueue, beachline);
        }
        else {
            handleCircleEvent(currentEvent, eventQueue, beachline);
        }
    }
}

void VoronoiDiagram::handleSiteEvent(const GFG& event, priority_queue<GFG>& eventQueue, set<GFG>& beachline) {
    // Implementation of site event handling
}

void VoronoiDiagram::handleCircleEvent(const GFG& event, priority_queue<GFG>& eventQueue, set<GFG>& beachline) {
    if (!event.circleEvent.valid) {
        return;
    }

    auto it = beachline.find(event);
    if (it != beachline.end()) {
        beachline.erase(it);
    }

    Point newVertex = { event.circleEvent.center.x, event.circleEvent.center.y };

    for (auto& region : voronoiRegions) {
        if (region.site.x == newVertex.x && region.site.y == newVertex.y) {
            region.vertices.push_back(newVertex);
        }
    }
}

CircleEvent VoronoiDiagram::computeCircumcircle(const Point& a, const Point& b, const Point& c) {
    double A = b.x - a.x;
    double B = b.y - a.y;
    double C = c.x - a.x;
    double D = c.y - a.y;
    double E = A * (a.x + b.x) + B * (a.y + b.y);
    double F = C * (a.x + c.x) + D * (a.y + c.y);
    double G = 2 * (A * (c.y - b.y) - B * (c.x - b.x));

    if (abs(G) < 1e-6) {
        return { {0, 0}, 0, false };
    }

    Point center = { (D * E - B * F) / G, (A * F - C * E) / G };
    double radius = sqrt((center.x - a.x) * (center.x - a.x) + (center.y - a.y) * (center.y - a.y));
    return { center, radius, true };
}

void VoronoiDiagram::outputRegions() {
    for (const auto& region : voronoiRegions) {
        cout << "Voronoi Region for site (" << region.site.x << ", " << region.site.y << "):" << endl;
        for (const auto& vertex : region.vertices) {
            cout << "Vertex (" << vertex.x << ", " << vertex.y << ")" << endl;
        }
        cout << endl;
    }
}

void VoronoiDiagram::renderVoronoi() {
    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 0.0);
    for (const auto& region : voronoiRegions) {
        for (const auto& vertex : region.vertices) {
            glVertex3f(vertex.x, vertex.y, 0.0f);
        }
    }
    glEnd();
}
