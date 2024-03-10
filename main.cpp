#include "Timeline.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <deque>



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

    sf::VertexArray lines(sf::Lines, 50);
    int numLines = 0;


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
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        target = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                        //Update mouse click circles.
                        sf::CircleShape c;
                        c.setRadius(5.0);
                        c.setOrigin(5.0, 5.0);
                        c.setFillColor(sf::Color::Green);
                        c.setPosition(target);
                        clickCircles.push_back(c);
                        if (clickCircles.size() > 5) {
                            clickCircles.pop_front();
                        }
                        if (numLines == 0) {
                            lines[numLines++] = target;
                            lines[numLines++] = target;
                        }
                        else {
                            lines[numLines++] = lines[numLines - 1];
                            lines[numLines++] = target;
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
            window.display();
        }
        

        //We've processed a tic, wait for the next one.
        lastTic = currentTic;
    }
    return EXIT_SUCCESS;
}