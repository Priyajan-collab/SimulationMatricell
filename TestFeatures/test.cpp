#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath> // For std::round

const int numRows = 10;
const int numCols = 10;
const float cellSize = 50.0f;

class Element {
protected:
    sf::Vector2f position;

public:
    Element(float x, float y) : position(x, y) {}

    virtual void snapToGrid() = 0;

    sf::Vector2f getPosition() const {
        return position;
    }

    void setPosition(sf::Vector2f pos) {
        position = pos;
    }
};

class Resistor : public Element {
public:
    Resistor(float x, float y) : Element(x, y) {}

    void snapToGrid() override {
        float snappedX = std::round(position.x / cellSize) * cellSize;
        float snappedY = std::round(position.y / cellSize) * cellSize;
        position = sf::Vector2f(snappedX, snappedY);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Element Snap to Grid");

    std::vector<Element*> elements;
    bool placingElement = false;
    Element* currentElement = nullptr;

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

                    // Example: create a resistor and snap it to grid
                    Resistor* resistor = new Resistor(mouseX, mouseY);
                    resistor->snapToGrid();
                    elements.push_back(resistor);
                    currentElement = resistor;
                    placingElement = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left && placingElement && currentElement) {
                    currentElement = nullptr;
                    placingElement = false;
                }
            }
        }

        if (placingElement && currentElement) {
            // Update element's position to follow mouse cursor
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            currentElement->setPosition(mousePos);
            currentElement->snapToGrid(); // Snap the position to grid
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

        // Draw elements
        for (auto& element : elements) {
            sf::CircleShape shape(20.0f); // Example shape, replace with actual element drawing
            shape.setPosition(element->getPosition());
            shape.setFillColor(sf::Color::Blue); // Example color
            window.draw(shape);
        }

        window.display();
    }

    for (auto& element : elements) {
        delete element;
    }
    elements.clear();

    return 0;
}
