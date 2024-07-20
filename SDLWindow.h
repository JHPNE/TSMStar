// SDLWindow.h
#pragma once

#include <SDL.h>
#include <unordered_map>
#include "Vector.h"
#include <vector>

#include "MapManager.h"

class SDLWindow {
public:
    SDLWindow(const char* title, double width, double height);
    ~SDLWindow();
    void start();
    // contains directions
    std::vector<Vector<2>> vectorList;

private:

    void createPoints();
    void createNet();
    void printPoints();
    void drawLine(Vector<2> vec1, Vector<2> vec2);
    void handleEvents();
    void recursiveMapGeneration(Vector<2>);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool quit;
    double SCREEN_WIDTH;
    double SCREEN_HEIGHT;
    std::unordered_map<Vector<2>, SDL_Rect> cities;
    std::unordered_map<Vector<2>, SDL_Rect> net;
};

