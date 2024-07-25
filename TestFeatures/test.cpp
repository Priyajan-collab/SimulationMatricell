#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

using namespace sf;
using namespace std;

// DraggableElement class definition
const float cellSize = 20.0f;
const int numRows = 50;
const int numCols = 70;
int col = 0;
int row = 0;

class ElementRender {
private:
    ImVec2 imagePos;
    float var;
    bool showInputWindow;
    bool isDragging;
    ImTextureID textureID;
    string id;

public:
    ElementRender(ImVec2 pos, const std::string& uniqueID, float initialVar = 45.0f)
        : imagePos(pos), var(initialVar), showInputWindow(false), isDragging(false), id(uniqueID) {}

    void setTexture(ImTextureID texID) {
        textureID = texID;
    }

    ImTextureID getTextureID() const { return textureID; }

    void drawElement() {
    // Draw the texture as an image button
    ImGui::SetCursorPos(imagePos);
     // Save current style
    ImVec4 prevButtonColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
    ImVec4 prevButtonHoveredColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
    ImVec4 prevButtonActiveColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    ImVec4 prevBorderColor = ImGui::GetStyle().Colors[ImGuiCol_Border];
    ImVec4 prevFrameBgColor = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];

    // Set colors to be transparent
    ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0, 0, 0, 0);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0, 0, 0, 0);
    ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0, 0, 0, 0);

    bool imageButtonClicked = ImGui::ImageButton(textureID, ImVec2(150, 50));

    // Restore previous style
    ImGui::GetStyle().Colors[ImGuiCol_Button] = prevButtonColor;
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = prevButtonHoveredColor;
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = prevButtonActiveColor;
    ImGui::GetStyle().Colors[ImGuiCol_Border] = prevBorderColor;
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = prevFrameBgColor;
    // bool imageButtonClicked = ImGui::ImageButton(textureID, ImVec2(150, 50), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));

    if (imageButtonClicked) {
        showInputWindow = !isDragging;
        isDragging = false;
    }

    // Handle dragging
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        isDragging = true;
        ImVec2 mouseDragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        imagePos.x += mouseDragDelta.x;
        imagePos.y += mouseDragDelta.y;
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }

    // Display variable
    ImVec2 variableDisplayPos = ImVec2(imagePos.x, imagePos.y + 55); // Adjust position as needed
    ImGui::SetCursorPos(variableDisplayPos);
    ImGui::BeginChild(("VariableDisplay_" + id).c_str(), ImVec2(158, 35), true);
    ImGui::Text("Variable Value: %.1f", var);
    ImGui::EndChild();

    // Handle input window
    if (showInputWindow) {
        ImGui::Begin(("InputWindow_" + id).c_str(), &showInputWindow);
        ImGui::Text("Enter new value for variable");
        ImGui::InputFloat("Variable", &var);
        if (ImGui::Button("Close")) {
            showInputWindow = false;
        }
        ImGui::End();
    }
}


};


class Draggable {
 protected:
  bool isDragging;
  Vector2f dragOffset;

 public:
  RectangleShape shape;

  Draggable() : isDragging(false) {}

  Draggable(float width, float height, float x, float y) : isDragging(false) {
    shape.setSize(Vector2f(width, height));
    shape.setPosition(x, y);
  }

  virtual void draw(RenderWindow& window) { window.draw(shape); }

  virtual bool contains(Vector2f mousePosition) const {
    return shape.getGlobalBounds().contains(mousePosition);
  }

  virtual void startDragging(Vector2f mousePosition) {
    isDragging = true;
    dragOffset = shape.getPosition() - mousePosition;
  }

  virtual void stopDragging() { isDragging = false; }

  void setColor(const Color& color) { shape.setFillColor(color); }

  virtual void updatePosition(Vector2f mousePosition) {
    Vector2f position;

    // Calculate grid cell coordinates
    int col = static_cast<int>(
        (mousePosition.x + dragOffset.x - cellSize / 2.0f) / cellSize);
    int row = static_cast<int>(
        (mousePosition.y + dragOffset.y - cellSize / 2.0f) / cellSize);

    if (row >= 0 && row < numRows && col >= 0 && col < numCols) {
      // Calculate position to place object at the center of the grid cell
      float centerX =
          col * cellSize + cellSize / 2.0f - shape.getSize().x / 2.0f;
      float centerY =
          row * cellSize + cellSize / 2.0f - shape.getSize().y / 2.0f;

      // Set the position accordingly
      position.x = centerX;
      position.y = centerY;
    }

    if (isDragging) {
      shape.setPosition(position - dragOffset);
    }
  }
  virtual ~Draggable() = default;
};

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
