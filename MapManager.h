#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "Vector.h" // Include your Vector header
#include <unordered_map>
#include <vector>
#include <iostream>

class MapManager {
public:
    // Add a point to the map
    void addPoint(const Vector<2>& point) {
        if (connections.find(point) == connections.end()) {
            connections[point] = std::vector<Vector<2>>();
        }
    }

    // Connect two points
    void connectPoints(const Vector<2>& point1, const Vector<2>& point2) {
        connections[point1].push_back(point2);
        connections[point2].push_back(point1); // If connections are bidirectional
    }

    // Render the map (Pseudo-code)
    void render() const {
        for (const auto& [point, connectedPoints] : connections) {
            for (const auto& connectedPoint : connectedPoints) {
                drawLine(point, connectedPoint);
            }
        }
    }

private:
    std::unordered_map<Vector<2>, std::vector<Vector<2>>> connections;

    // Function to draw a line between two points (pseudo-code)
    void drawLine(const Vector<2>& start, const Vector<2>& end) const {
        // Implementation depends on your graphics library
        std::cout << "Drawing line from (" << start[0] << ", " << start[1] << ") "
                  << "to (" << end[0] << ", " << end[1] << ")\n";
    }
};

#endif // MAPMANAGER_H
