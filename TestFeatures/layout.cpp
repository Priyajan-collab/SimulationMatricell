#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>

// DrawableTransformable Interface
class DrawableTransformable : public sf::Drawable, public sf::Transformable {
public:
    using sf::Transformable::setPosition;
};

// Button Class
class Button : public DrawableTransformable {
public:
    Button(const sf::Vector2f& size, const sf::Color& color, const std::string& text, sf::Font& font) {
        shape.setSize(size);
        shape.setFillColor(color);

        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::White);
        updateLabelPosition();
    }

    void setPosition(const sf::Vector2f& position) {
        DrawableTransformable::setPosition(position);
        shape.setPosition(position);
        updateLabelPosition();
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(shape, states);
        target.draw(label, states);
    }

private:
    void updateLabelPosition() {
        label.setPosition(
            shape.getPosition().x + (shape.getSize().x - label.getGlobalBounds().width) / 2,
            shape.getPosition().y + (shape.getSize().y - label.getGlobalBounds().height) / 2 - 5
        );
    }

    sf::RectangleShape shape;
    sf::Text label;
};

// LayoutManager Class
enum class LayoutType { Vertical, Horizontal };

class LayoutManager : public sf::Drawable {
public:
    LayoutManager(LayoutType type, float spacing)
        : type(type), spacing(spacing) {}

    void addDrawable(const std::shared_ptr<DrawableTransformable>& drawable, const sf::Vector2f& size) {
        drawables.push_back(drawable);
        sizes.push_back(size);
        updateLayout();
    }

    void setPosition(const sf::Vector2f& position) {
        this->position = position;
        updateLayout();
    }

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (const auto& drawable : drawables) {
            target.draw(*drawable, states);
        }
    }

private:
    void updateLayout() {
        sf::Vector2f currentPosition = position;

        for (size_t i = 0; i < drawables.size(); ++i) {
            drawables[i]->setPosition(currentPosition);
            if (type == LayoutType::Vertical) {
                currentPosition.y += sizes[i].y + spacing;
            } else if (type == LayoutType::Horizontal) {
                currentPosition.x += sizes[i].x + spacing;
            }
        }
    }

    LayoutType type;
    float spacing;
    sf::Vector2f position;
    std::vector<std::shared_ptr<DrawableTransformable>> drawables;
    std::vector<sf::Vector2f> sizes;
};

// Main Function
int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Layout Example");

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    // Create some buttons
    auto button1 = std::make_shared<Button>(sf::Vector2f(200, 50), sf::Color::Blue, "Button 1", font);
    auto button2 = std::make_shared<Button>(sf::Vector2f(200, 50), sf::Color::Green, "Button 2", font);
    auto button3 = std::make_shared<Button>(sf::Vector2f(200, 50), sf::Color::Red, "Button 3", font);

    // Create a vertical layout manager and add the buttons to it
    LayoutManager layout(LayoutType::Horizontal, 10.0f);
    layout.setPosition(sf::Vector2f(300, 100));
    layout.addDrawable(button1, sf::Vector2f(200, 50));
    layout.addDrawable(button2, sf::Vector2f(200, 50));
    layout.addDrawable(button3, sf::Vector2f(200, 50));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);
        window.draw(layout);
        window.display();
    }

    return 0;
}
