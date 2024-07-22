#include "SDLWindow.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

SDLWindow::SDLWindow(const char* title, double width, double height)
    : window(nullptr), quit(false), renderer(nullptr), SCREEN_WIDTH(width), SCREEN_HEIGHT(height) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(-1);
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        static_cast<int>(SCREEN_WIDTH), static_cast<int>(SCREEN_HEIGHT),
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(-1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(-1);
    }

    srand(static_cast<unsigned>(time(0)));
}

SDLWindow::~SDLWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLWindow::start() {
    createPoints();
    createNet();
    while (!quit) {
        handleEvents();
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Set draw color to black
        SDL_RenderClear(renderer);

        printPoints();

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }
}
void SDLWindow::printPoints() {
    if (cities.empty()) {
        std::cerr << "No cities to draw." << std::endl;
        return;
    }

    // Draw city points
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // Set draw color to white
    for (const auto& [position, rect] : cities) {
        SDL_RenderFillRect(renderer, &rect);
    }

    if (net.empty()) {
        std::cerr << "No net to draw." << std::endl;
        return;
    }

    // Draw net points
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Set draw color to green
    for (const auto& [position, rect] : net) {
        SDL_RenderFillRect(renderer, &rect);
    }

    // Create a vector to store the angle and radius for sorting
    struct PolarPoint {
        Vector<2> vector;
        float angle;
        float radius;
    };
    std::vector<PolarPoint> polarPoints;

    // Calculate the center point
    Vector<2> center = calculateCenter();

    // Calculate angle and radius for each point and store in the vector
    for (const auto& [cityPos, _] : cities) {
        Vector<2> closestNetPoint = findClosestPoint(cityPos, net);
        float dx = closestNetPoint[0] - center[0];
        float dy = closestNetPoint[1] - center[1];
        float angle = std::atan2(dy, dx);
        float radius = std::sqrt(dx * dx + dy * dy);
        polarPoints.push_back({cityPos, angle, radius});
    }

    // Sort the points by angle and then by radius
    std::sort(polarPoints.begin(), polarPoints.end(), [](const PolarPoint& a, const PolarPoint& b) {
        if (std::fabs(a.angle - b.angle) < 0.001) { // If angles are very close, sort by radius
            return a.radius < b.radius;
        }
        return a.angle < b.angle;
    });

    // Draw lines between sorted points and connect the last point to the first
    Vector<2> previousVector;
    bool isPreviousVectorSet = false;
    Vector<2> firstVector;

    for (const auto& polarPoint : polarPoints) {
        if (isPreviousVectorSet) {
            drawLine(previousVector, polarPoint.vector);
        } else {
            firstVector = polarPoint.vector;
        }

        previousVector = polarPoint.vector;
        isPreviousVectorSet = true;
    }

    // Connect the last vector with the first vector
    if (isPreviousVectorSet) {
        drawLine(previousVector, firstVector);
    }
}

Vector<2> SDLWindow::calculateCenter() {
    float middlePointX = 0.0f;
    float middlePointY = 0.0f;

    for (const auto& entry : cities) {
        middlePointX += entry.first[0];
        middlePointY += entry.first[1];
    }

    middlePointX /= static_cast<float>(cities.size());
    middlePointY /= static_cast<float>(cities.size());

    return Vector<2>{middlePointX, middlePointY};
}



Vector<2> SDLWindow::findClosestPoint(const Vector<2>& source, const std::unordered_map<Vector<2>, SDL_Rect>& targets) {
    Vector<2> closestPoint;
    float minDistance = std::numeric_limits<float>::max();

    for (const auto& [targetPos, _] : targets) {
        if (source == targetPos) continue; // Skip the source point itself
        float distance = calculateDistance(source, targetPos);
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = targetPos;
        }
    }

    return closestPoint;
}

void SDLWindow::createNet() {
    std::vector<Vector<2>> keys;

    for (const auto& entry : cities) {
        keys.push_back(entry.first);
    }

    if (keys.empty()) {
        std::cerr << "No points available to calculate the middle point." << std::endl;
    }

    float middlePointX = 0.0f;
    float middlePointY = 0.0f;

    for (const Vector<2>& vector : keys) {
        middlePointX += vector[0];
        middlePointY += vector[1];
    }

    middlePointX /= static_cast<float>(keys.size());
    middlePointY /= static_cast<float>(keys.size());

    Vector<2> middlePoint = Vector<2>{middlePointX, middlePointY};

    createNetPoints(middlePoint);
}

float SDLWindow::calculateDistance(const Vector<2>& point1, const Vector<2>& point2) {
    float dx = point1[0] - point2[0];
    float dy = point1[1] - point2[1];
    return std::sqrt(dx * dx + dy * dy);
}

void SDLWindow::createNetPoints(Vector<2> vec) {
    const int pointSize = 5;
    const int numPoints = numberOfPoints * 2;
    const int radiusIncrement = numberOfPoints;

    float angleIncrement = 2 * M_PI / numPoints;

    SDL_Rect pointRect = { static_cast<int>(vec[0]) - pointSize / 2, // Center the point
                           static_cast<int>(vec[1]) - pointSize / 2,
                           pointSize,
                           pointSize };
    net[vec] = pointRect;

    float radius = 0;
    int k = 0;
    for (int j = 0; j < radiusIncrement; ++j) {
        radius += 50;
        for (int i = 0; i < numPoints; ++i) {
            float angle = i * angleIncrement;
            float x = vec[0] + radius * std::cos(angle);
            float y = vec[1] + radius * std::sin(angle);
            SDL_Rect pointRect = { static_cast<int>(x) - pointSize / 2, // Center the point
                                   static_cast<int>(y) - pointSize / 2,
                                   pointSize,
                                   pointSize };
            Vector<2> newPoint = Vector<2>{x, y};
            net[newPoint] = pointRect;
            distanceMap[newPoint] = k + radius;
            ++k;
        }
    }
}

void SDLWindow::createPoints() {
    double coverPercentage = 0.99;
    double upperBoundWidth = SCREEN_WIDTH * coverPercentage;
    double upperBoundHeight = SCREEN_HEIGHT * coverPercentage;

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Set draw color to black
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // Set draw color to white

    const int pointSize = 8;

    for (int i = 0; i < numberOfPoints; ++i) {
        double randomX = (rand() / (double)RAND_MAX) * upperBoundWidth;
        double randomY = (rand() / (double)RAND_MAX) * upperBoundHeight;

        Vector<2> position = Vector<2>{randomX, randomY};
        SDL_Rect pointRect = { static_cast<int>(randomX), static_cast<int>(randomY), pointSize, pointSize };

        cities[position] = pointRect;

        SDL_RenderFillRect(renderer, &pointRect);
    }

    SDL_RenderPresent(renderer);
}

void SDLWindow::drawLine(Vector<2> vec1, Vector<2> vec2) {
    float vec1x = vec1[0];
    float vec1y = vec1[1];
    float vec2x = vec2[0];
    float vec2y = vec2[1];

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF); // Set draw color to red

    SDL_RenderDrawLine(renderer, static_cast<int>(vec1x), static_cast<int>(vec1y),
                       static_cast<int>(vec2x), static_cast<int>(vec2y));
}

void SDLWindow::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            quit = true;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_q) {
                quit = true;
            }
        }
    }
}
