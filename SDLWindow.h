#pragma once

#include <map>
#include <SDL.h>
#include <unordered_map>
#include "Vector.h"
#include <vector>

class SDLWindow {
public:
    SDLWindow(const char* title, double width, double height);
    ~SDLWindow();
    void start();

private:
    void createPoints();
    void createNet();
    void printPoints();
    void drawLine(Vector<2> vec1, Vector<2> vec2);
    void handleEvents();
    void createNetPoints(Vector<2>);
    float calculateDistance(const Vector<2>& point1, const Vector<2>& point2);
    Vector<2> findClosestPoint(const Vector<2>& source, const std::unordered_map<Vector<2>, SDL_Rect>& targets);
    Vector<2> calculateCenter();


    SDL_Window* window;
    SDL_Renderer* renderer;
    // Change value to play around
    int numberOfPoints = 30;
    bool quit;
    double SCREEN_WIDTH;
    double SCREEN_HEIGHT;
    std::unordered_map<Vector<2>, SDL_Rect> cities;
    std::unordered_map<Vector<2>, SDL_Rect> net;
    std::map<Vector<2>, int> distanceMap;
    std::map<int, Vector<2>> orderedMap;
    std::vector<Vector<2>> vectorList;
};
