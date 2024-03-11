#include "Timeline.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <deque>
#include <iostream>
#include <cmath>
#include <fstream>

float mapToRange(float theta) {
    theta = fmod(theta, 2 * M_PI);
    if (fabs(theta) <= M_PI) { //If theta is from -pi to pi
        return theta;
    }
    else if (theta > M_PI) { //if theta is greater than pi
        return theta - 2 * M_PI;
    }
    return theta + 2 * M_PI; //if theta is less than -pi
}


int main() {

    Timeline global;
    int tic = 10;
    Timeline frameTime(&global, tic);
    //CurrentTic starts higher than lastTic so the program starts immediately.
    int64_t currentTic = 0;
    int64_t lastTic = -1;

    sf::RenderWindow window;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(640, 480, desktop.bitsPerPixel), "Game Window", sf::Style::Default);

    sf::Vector2f target;

    std::deque<sf::CircleShape> clickCircles;

    sf::VertexArray lines(sf::Lines, 100);
    int numLines = 0;
    int numArrows = 0;
    sf::VertexArray arrows(sf::Lines, 400);

    sf::Vector2f lastVertex;
    while (window.isOpen()) {
        //Get the current tic.
        currentTic = frameTime.getTime();
        //Only process if we've gone to the next tic (or further).
        if (currentTic > lastTic) {
            sf::Event event;
            //If the X button is pressed, close the window
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    //Left click signifies new node and line being drawn.
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        target = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2f direction;
                        bool collision = false;
                        //Update mouse click circles.
                        for (int i = 0; i < clickCircles.size(); i++) {
                            if (clickCircles[i].getGlobalBounds().contains(target)) {
                                target = clickCircles[i].getPosition();
                                collision = true;
                            }
                        }
                        if (!collision) {
                            sf::CircleShape c;
                            c.setRadius(5.0);
                            c.setOrigin(5.0, 5.0);
                            c.setFillColor(sf::Color::Green);
                            c.setPosition(target);
                            clickCircles.push_back(c);
                        }
                        if (numLines == 0) {
                            lines[numLines++] = target;
                            lines[numLines++] = target;
                            lastVertex = target;
                        }
                        else {
                            direction = lastVertex;
                            direction = target - direction;
                            lines[numLines++] = lastVertex;
                            lines[numLines++] = target;
                            lastVertex = target;
                        }
                        float theta = atan2(direction.y, direction.x);
                        if (theta != 0) {
                            theta = mapToRange(theta - M_PI);
                            float leftTail = mapToRange(theta + .20);
                            float rightTail = mapToRange(theta - .20);

                            sf::Vector2f leftVector(cos(leftTail), sin(leftTail));
                            sf::Vector2f rightVector(cos(rightTail), sin(rightTail));
                            leftVector *= 20.f;
                            rightVector *= 20.f;
                            leftVector += target;
                            rightVector += target;

                            arrows[0 + numArrows * 4] = target;
                            arrows[1 + numArrows * 4] = leftVector;
                            arrows[2 + numArrows * 4] = target;
                            arrows[3 + numArrows * 4] = rightVector;
                            numArrows++;
                        }
                    }
                    //Right click signifies changing origin point for line. 
                    //This is a connected graph, so must be on an existing node to take effect.
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        target = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                        bool collision = false;
                        //Update mouse click circles.
                        for (int i = 0; i < clickCircles.size(); i++) {
                            if (clickCircles[i].getGlobalBounds().contains(target)) {
                                target = clickCircles[i].getPosition();
                                collision = true;
                            }
                        }
                        //Does not support overlapping collisions.
                        if (collision) {
                            lastVertex = target;
                        }
                    }
                }
            }

            //Draw to window.
            window.clear(sf::Color(0, 128, 128));
            for (int i = 0; i < clickCircles.size(); i++) {
                window.draw(clickCircles[i]);
            }
            window.draw(lines);
            window.draw(arrows);
            window.display();
        }

        //We've processed a tic, wait for the next one.
        lastTic = currentTic;
    }

    //Export graph to a file.
    std::ofstream output ("output.txt", std::ofstream::out);
    for (int i = 2; i < numLines; i++) {
        if (i % 2 == 0) {
            output << "Vertex A: " << lines[i].position.x << "," << lines[i].position.y << "  ";
        }
        else {
            output << "Vertex B: " << lines[i].position.x << "," << lines[i].position.y << "   ";
            output << "Weight: " << sqrt(pow((lines[i].position - lines[i-1].position).x, 2) + pow((lines[i].position - lines[i-1].position).y, 2)) << std::endl;;
        }
    }
    output.close();

    return EXIT_SUCCESS;
}