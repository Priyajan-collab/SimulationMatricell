#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

const int numRows = 10;
const int numCols = 10;
const float cellSize = 50.0f;

class Resistor : public sf::Drawable {
private:
    std::vector<sf::Vertex> zigzag;
    sf::RectangleShape leftTerminal;
    sf::RectangleShape rightTerminal;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(&zigzag[0], zigzag.size(), sf::LinesStrip, states);
        target.draw(leftTerminal, states);
        target.draw(rightTerminal, states);
    }

public:
    Resistor(float x, float y) {
        float zigzagWidth = 30.0f;
        float zigzagHeight = 10.0f;
        float zigzagSegments = 6;

        // Create zigzag pattern
        for (int i = 0; i <= zigzagSegments; ++i) {
            float px = x + i * (zigzagWidth / zigzagSegments);
            float py = y + ((i % 2 == 0) ? 0 : zigzagHeight);
            zigzag.emplace_back(sf::Vertex(sf::Vector2f(px, py), sf::Color::Blue));
        }

        leftTerminal.setSize(sf::Vector2f(10.0f, 5.0f));
        leftTerminal.setFillColor(sf::Color::Black);
        leftTerminal.setPosition(x - 10.0f, y + zigzagHeight / 2.0f - 2.5f);

        rightTerminal.setSize(sf::Vector2f(10.0f, 5.0f));
        rightTerminal.setFillColor(sf::Color::Black);
        rightTerminal.setPosition(x + zigzagWidth, y + zigzagHeight / 2.0f - 2.5f);
    }

    void setPosition(sf::Vector2f pos) {
        float x = pos.x;
        float y = pos.y;
        float zigzagWidth = 30.0f;
        float zigzagHeight = 10.0f;
        float zigzagSegments = 6;

        // Update zigzag pattern
        for (int i = 0; i <= zigzagSegments; ++i) {
            float px = x + i * (zigzagWidth / zigzagSegments);
            float py = y + ((i % 2 == 0) ? 0 : zigzagHeight);
            zigzag[i].position = sf::Vector2f(px, py);
        }

        leftTerminal.setPosition(x - 10.0f, y + zigzagHeight / 2.0f - 2.5f);
        rightTerminal.setPosition(x + zigzagWidth, y + zigzagHeight / 2.0f - 2.5f);
    }

    void snapToGrid() {
        sf::Vector2f position = zigzag[0].position;
        float snappedX = std::round(position.x / cellSize) * cellSize;
        float snappedY = std::round(position.y / cellSize) * cellSize;
        setPosition(sf::Vector2f(snappedX, snappedY));
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Resistor Snap to Grid");

    std::vector<Resistor> resistors;
    bool placingResistor = false;
    Resistor* currentResistor = nullptr;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    float mouseX = static_cast<float>(event.mouseButton.x);
                    float mouseY = static_cast<float>(event.mouseButton.y);

                    // Create a resistor and snap it to grid
                    resistors.emplace_back(mouseX, mouseY);
                    resistors.back().snapToGrid();
                    currentResistor = &resistors.back();
                    placingResistor = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left && placingResistor && currentResistor) {
                    currentResistor->snapToGrid();
                    currentResistor = nullptr;
                    placingResistor = false;
                }
            }
        }

        if (placingResistor && currentResistor) {
            // Update resistor's position to follow mouse cursor
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            currentResistor->setPosition(mousePos);
            currentResistor->snapToGrid(); // Snap the position to grid
        }

        window.clear(sf::Color::White);

        // Draw grid (optional for visualization)
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(j * cellSize, i * cellSize);
                cell.setFillColor(sf::Color::Transparent);
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(1.0f);
                window.draw(cell);
            }
        }

        // Draw resistors
        for (const auto& resistor : resistors) {
            window.draw(resistor);
        }

        window.display();
    }

    return 0;
}
