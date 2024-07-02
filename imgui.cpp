#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <iostream>  // Include for outputting error messages

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Hello World");

    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Failed to initialize ImGui-SFML" << std::endl;
        return -1;  // Exit the program if initialization fails
    }

    bool circleExists = true;
    float circleRadius = 200.f;
    int circleSegment = 100;
    float circleColor[3] = {(float)204/255,(float)77/255,(float)5/255};

    sf::CircleShape shape(circleRadius, circleSegment);
    shape.setFillColor(sf::Color(
        (int)(circleColor[0] * 255),
        (int)(circleColor[1] * 255),
        (int)(circleColor[2] * 255)
    ));

    shape.setOrigin(circleRadius,circleRadius);
    shape.setPosition(400, 400);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);  // Use the new function signature
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Window Titile");
        ImGui::Text("Hello World");
        ImGui::Checkbox("Circle",&circleExists);
        ImGui::SliderFloat("Radius", &circleRadius, 100.0f,300.0f);
        ImGui::SliderInt("Slides",&circleSegment,3,100);
        ImGui::ColorEdit3("Color",circleColor);
        ImGui::End();

        shape.setRadius(circleRadius);
        shape.setOrigin(circleRadius,circleRadius);
        shape.setPointCount(circleSegment);
        shape.setFillColor(sf::Color(
        (int)(circleColor[0] * 255),
        (int)(circleColor[1] * 255),
        (int)(circleColor[2] * 255)
    ));
        window.clear(sf::Color(18, 33, 43));
        if(circleExists){
            window.draw(shape);
        }
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}