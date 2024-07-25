#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

using namespace sf;
using namespace std;

// DraggableElement class definition
class DraggableElement {
protected:
    bool isDragging;
    Vector2f dragOffset;
    Vector2f imageSize;

public:
    RectangleShape dragRect; // Invisible rectangle for dragging
    Sprite imageSprite;      // Image to be dragged
    Texture imageTexture;   // Image texture

    DraggableElement(const string& imagePath, const Vector2f& imgSize, const Vector2f& rectSize, const Vector2f& position) 
        : isDragging(false),imageSize(imgSize) {

        // Load the image texture
        if (!imageTexture.loadFromFile(imagePath)) {
            throw runtime_error("Failed to load image texture");
        }

        // Set up the image sprite
        imageSprite.setTexture(imageTexture);
        Vector2u textureSize = imageTexture.getSize();

        // Calculate the scale factors
        float scaleX = imageSize.x / textureSize.x;
        float scaleY = imageSize.y / textureSize.y;

        // Apply the scale to the sprite
        imageSprite.setScale(scaleX, scaleY);

        // Set the position and origin of the sprite
        imageSprite.setPosition(position);
        imageSprite.setOrigin(imageSize / 2.0f);

        Vector2f centerPos(position.x + (imageSize.x)/2.0f,position.y + (imageSize.y)/2.0f);
        // Set up the drag rectangle
        dragRect.setSize(rectSize);
        dragRect.setFillColor(Color::Red); // Make the rectangle invisible
        dragRect.setOrigin(rectSize / 2.0f); // Set the origin to the center of the rectangle
        // Center the rectangle on the image
        dragRect.setPosition(centerPos);


        cout<<"Rectangle"<<endl;
        cout<<"X:"<<dragRect.getPosition().x<<"Y:"<<dragRect.getPosition().y<<endl;

        cout<<"Image"<<endl;
        cout<<"X:"<<imageSprite.getPosition().x<<"Y:"<<imageSprite.getPosition().y<<endl;
    }

    void draw(RenderWindow& window) {
        window.draw(imageSprite);   // Draw the image
        window.draw(dragRect);      // Draw the invisible rectangle for dragging
    }

    bool contains(Vector2f mousePosition) const {
        return dragRect.getGlobalBounds().contains(mousePosition);
    }

    void startDragging(Vector2f mousePosition) {
        isDragging = true;
        dragOffset = imageSprite.getPosition() - mousePosition;
    }

    void stopDragging() {
        isDragging = false;
    }

    void updatePosition(Vector2f mousePosition) {
        if (isDragging) {
            Vector2f newPosition = mousePosition + dragOffset;
            Vector2f centerPos(newPosition.x + (imageSize.x)/2.0f,newPosition.y + (imageSize.y)/2.0f);
            // Update the position of both the image and the rectangle
            imageSprite.setPosition(newPosition);
            dragRect.setPosition(centerPos);
        }
    }
};

int main() {
    RenderWindow window(VideoMode(800, 600), "Drag Image Example");
    ImGui::SFML::Init(window);

    DraggableElement draggable("textures/ResistorIcon.png", Vector2f(80, 40), Vector2f(10, 10), Vector2f(400, 300));

    Clock deltaClock;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        Vector2f mousePosition = window.mapPixelToCoords(Mouse::getPosition(window));

        if (Mouse::isButtonPressed(Mouse::Left)) {
            if (draggable.contains(mousePosition)) {
                draggable.startDragging(mousePosition);
            }
            draggable.updatePosition(mousePosition);
        } else {
            draggable.stopDragging();
        }

        window.clear(Color::White);
        draggable.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
