#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;
const int ANIMATION_FRAMES = 120;
const float PI = 3.14159f;

// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Functions can be from lecture or from https://easings.net/#
        // ====== ====== ======
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // Linear
            if (keyPressed->code == sf::Keyboard::Key::Num1) {
                tween = [](float a, float b, float t) { return a + (b - a) * t; };
                // Ease in quadratic
            } else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                tween = [](float a, float b, float t) { return a + (b - a) * t * t; };
                // Sine/cosine ease in-out
            } else if (keyPressed->code == sf::Keyboard::Key::Num3) {
                tween = [](float a, float b, float t) {
                    float eased = (std::sin((t - 0.5f) * PI) + 1.0f) / 2.0f;
                    return a + (b - a) * eased;
                };
                // Smooth blend alternate function
            } else if (keyPressed->code == sf::Keyboard::Key::Num4) {
                tween = [](float a, float b, float t) {
                    float easeIn = t * t * t;
                    float easeOut = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
                    float eased = (1.0f - t) * easeIn + t * easeOut;
                    return a + (b - a) * eased;
                };
                // Ease out quadratic
            } else if (keyPressed->code == sf::Keyboard::Key::Num5) {
                tween = [](float a, float b, float t) {
                    float eased = 1.0f - (1.0f - t) * (1.0f - t);
                    return a + (b - a) * eased;
                };
                // Ease in-out quadratic
            } else if (keyPressed->code == sf::Keyboard::Key::Num6) {
                tween = [](float a, float b, float t) {
                    float eased = t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
                    return a + (b - a) * eased;
                };
                // Ease in cubic
            } else if (keyPressed->code == sf::Keyboard::Key::Num7) {
                tween = [](float a, float b, float t) { return a + (b - a) * t * t * t; };
                // Ease out cubic
            } else if (keyPressed->code == sf::Keyboard::Key::Num8) {
                tween = [](float a, float b, float t) {
                    float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
                    return a + (b - a) * eased;
                };
                // Ease in-out cubic
            } else if (keyPressed->code == sf::Keyboard::Key::Num9) {
                tween = [](float a, float b, float t) {
                    float eased = t < 0.5f ? 4.0f * t * t * t
                                           : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
                    return a + (b - a) * eased;
                };
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    static int frame = 0;
    float animationTime = static_cast<float>(frame % ANIMATION_FRAMES) / ANIMATION_FRAMES;
    frame++;

    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======
    const float circleRadius = 30.0f;
    float circleX = tween(circleRadius, WINDOW_WIDTH - circleRadius, animationTime);
    float circleY = WINDOW_HEIGHT / 3.0f;

    sf::CircleShape circle(circleRadius);
    circle.setOrigin({circleRadius, circleRadius});
    circle.setPosition({circleX, circleY});
    circle.setFillColor(sf::Color::Cyan);
    window.draw(circle);

    // ====== ====== ======
    // TODO: (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======
    const float graphLeft = 100.0f;
    const float graphTop = 550.0f;
    const float graphSize = 200.0f;
    const float graphBottom = graphTop + graphSize;

    sf::VertexArray axes(sf::PrimitiveType::Lines, 4);
    axes[0] = sf::Vertex{{graphLeft, graphBottom}, sf::Color::White};
    axes[1] = sf::Vertex{{graphLeft + graphSize, graphBottom}, sf::Color::White};
    axes[2] = sf::Vertex{{graphLeft, graphBottom}, sf::Color::White};
    axes[3] = sf::Vertex{{graphLeft, graphTop}, sf::Color::White};
    window.draw(axes);

    sf::VertexArray curve(sf::PrimitiveType::LineStrip);
    for (int i = 0; i <= 100; i++) {
        float t = i / 100.0f;
        float value = tween(0.0f, 1.0f, t);
        float x = graphLeft + t * graphSize;
        float y = graphBottom - value * graphSize;
        curve.append(sf::Vertex{{x, y}, sf::Color::Green});
    }
    window.draw(curve);

    float currentValue = tween(0.0f, 1.0f, animationTime);
    sf::CircleShape graphDot(6.0f);
    graphDot.setOrigin({6.0f, 6.0f});
    graphDot.setPosition(
        {graphLeft + animationTime * graphSize, graphBottom - currentValue * graphSize});
    graphDot.setFillColor(sf::Color::Red);
    window.draw(graphDot);

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
