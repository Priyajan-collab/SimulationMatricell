
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <memory>

#include "../include/grid/grid.hpp"
#include "imgui-SFML.h"

using namespace sf;
using namespace std;
const float cellSize = 20.0f;
const int numRows = 50;
const int numCols = 70;
int col = 0;
int row = 0;
vector<vector<Cell>> grid;

void drawGrid(ImDrawList* drawList, const ImVec2& offset) {
  for (const auto& rowCells : grid) {
    for (const auto& cell : rowCells) {
      drawList->AddRectFilled(
          ImVec2(cell.position.x + offset.x, cell.position.y + offset.y),
          ImVec2(cell.position.x + cellSize + offset.x,
                 cell.position.y + cellSize + offset.y),
          cell.color);
      drawList->AddRect(
          ImVec2(cell.position.x + offset.x, cell.position.y + offset.y),
          ImVec2(cell.position.x + cellSize + offset.x,
                 cell.position.y + cellSize + offset.y),
          IM_COL32(128, 128, 128, 255));
    }
  }
}

class DraggableElement {
 protected:
  bool isDragging;
  Vector2f dragOffset;
  Vector2f imageSize;
  Vector2f rectSize;

 public:
  RectangleShape dragRect;  // Invisible rectangle for dragging
  Sprite imageSprite;       // Image to be dragged
  Texture imageTexture;     // Image texture
  static int id;
  DraggableElement(const Vector2f& position, string imagePath)
      : isDragging(false),
        imageSize(Vector2f(80, 40)),
        rectSize(Vector2f(10, 10)) {
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

    imageSprite.setPosition(position);
    imageSprite.setOrigin(imageSize / 2.0f);

    Vector2f centerPos(position.x + (imageSize.x) / 2.0f,
                       position.y + (imageSize.y) / 2.0f);

    dragRect.setSize(rectSize);
    dragRect.setFillColor(Color::Red);
    dragRect.setOrigin(rectSize /
                       2.0f);  // Set the origin to the center of the rectangle

    dragRect.setPosition(centerPos);

    cout << "Rectangle" << endl;
    cout << "X:" << dragRect.getPosition().x << "Y:" << dragRect.getPosition().y
         << endl;

    cout << "Image" << endl;
    cout << "X:" << imageSprite.getPosition().x
         << "Y:" << imageSprite.getPosition().y << endl;
  }

  void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(dragRect);
    id++;
  }

  bool contains(Vector2f mousePosition) const {
    return dragRect.getGlobalBounds().contains(mousePosition);
  }

  void startDragging(Vector2f mousePosition) {
    isDragging = true;
    dragOffset = imageSprite.getPosition() - mousePosition;
  }

  void stopDragging() { isDragging = false; }

  void updatePosition(Vector2f mousePosition) {
    if (isDragging) {
      Vector2f newPosition = mousePosition + dragOffset;
      Vector2f centerPos(newPosition.x + (imageSize.x) / 2.0f,
                         newPosition.y + (imageSize.y) / 2.0f);
      // Update the position of both the image and the rectangle
      imageSprite.setPosition(newPosition);
      dragRect.setPosition(centerPos);
    }
  }
};
int DraggableElement::id = 0;

class Component {
 public:
  virtual ~Component() {}
  // virtual static const string& getImagePath() const = 0;
};

class Resistor : public Component, public DraggableElement {
  static const string image;
  Texture resistorTexture;
  ImTextureID resistorTextureID;
  // ImTextureID batteryTextureID =
  // (void*)(intptr_t)batteryTexture.getNativeHandle();
 public:
  // Resistor( ImVec2 pos, const std::string& uniqueID, float initialVar
  // = 45.0f)
  //     : Component(), ElementRender( pos, uniqueID, initialVar) {

  // }
  Resistor(ImVec2 pos, float initialVar = 45.0f)
      : Component(), DraggableElement(pos, image) {}
  static const string& getImagePath() { return image; }
};

const string Resistor::image = "textures/ResistorIcon.png";

class Battery : public Component, public DraggableElement {
 public:
  static const string image;

  Battery(ImVec2 pos, float initialVar = 45.0f)
      : Component(), DraggableElement(pos, image) {
    cout << "Battery is made" << endl;
  }

  static const string& getImagePath() { return image; }
};

const string Battery::image = "textures/BatteryIcon.png";

class Inductor : public Component, public DraggableElement {
 public:
  static const string image;

  Inductor(ImVec2 pos, float initialVar = 45.0f)
      : Component(), DraggableElement(pos, image) {
    cout << "Inductorery is made" << endl;
  }

  static const string& getImagePath() { return image; }
};

const string Inductor::image = "textures/InductorIcon.png";

class MenuList {
 private:
  vector<Texture> textures;
  vector<ImTextureID> textureIDs;
  int selectedItem;  // Track selected item index
  string components[6];
  bool itemPlaced;
  static int id;

 public:
  MenuList()
      : selectedItem(-1), components{"Resistor",  "Battery", "Inductor",
                                     "Capacitor", "Diode",   "Transistor"} {
    // Initialize textures and textureIDs here if necessary
  }

  void setTextures(const vector<Texture>& texs) {
    textures = texs;
    textureIDs.clear();
    for (const auto& texture : textures) {
      textureIDs.push_back(reinterpret_cast<void*>(texture.getNativeHandle()));
    }
  }

  void drawMenu() {
    ImGui::BeginChild("MenuList", ImVec2(200, 0), true);
    for (size_t i = 0; i < 6; ++i) {  // Ensure we use the correct size
      if (ImGui::Button(components[i].c_str(), ImVec2(100, 50))) {
        selectedItem = i;
        itemPlaced = false;
        cout << "Selected index: " << i << endl;
        std::cout << "Selected: " << components[selectedItem] << std::endl;
      }
    }
    ImGui::EndChild();
  }

  int getSelectedComponent() const { return selectedItem; }

  string getSelectedComponentName() const {
    if (selectedItem >= 0 && selectedItem < 6) {
      return components[selectedItem];
    } else {
      return "";
    }
  }

  ImTextureID getSelectedTextureID() const {
    if (selectedItem >= 0 && selectedItem < textureIDs.size()) {
      return textureIDs[selectedItem];
    } else {
      return nullptr;
    }
  }

  Component* createComponent(const string& type, ImVec2 pos, float initialVar) {
    if (type == "Resistor") {
      return new Resistor(pos, initialVar);
    } else if (type == "Battery") {
      return new Battery(pos, initialVar);
    } else if (type == "Inductor") {
      return new Inductor(pos, initialVar);
    }
    id++;
    // Add more cases as needed
    return nullptr;
  }
  bool isItemPlaced() const { return itemPlaced; }

  void setItemPlaced(bool placed) { itemPlaced = placed; }
};
int MenuList::id = 0;

class CircuitElement {
 public:
  bool is_connected = false;

  virtual int connect() const = 0;
};

class Load : public CircuitElement {
 public:
  void connect() { is_connected = true; }

  int connect() const override { return is_connected ? 1 : 0; }
};

class Wire : public CircuitElement {
 public:
  int connect() const override { return 1; }

  int connectWith(const CircuitElement& element) const {
    return element.is_connected ? 1 : 0;
  }
};

class Switch : public CircuitElement {
 public:
  bool is_switch_on = true;

  int& toggle(int& state, bool click) {
    if (!click) {
      is_switch_on = false;
      state = 0;
    }
    return state;
  }

  int connect() const override { return is_switch_on ? 1 : 0; }
};

struct Line {
  Vertex points[2];

  Line(Vector2f start, Vector2f end) {
    points[0].position = start;
    points[1].position = end;
    points[0].color = Color::Black;
    points[1].color = Color::Black;
  }
};

// void circuitConnection(bool switchToggle) {
//     int a, b, c;
//     unique_ptr<Battery[]> battery(new Battery[1]);
//     unique_ptr<Load[]> load(new Load[1]);
//     unique_ptr<Wire[]> wire(new Wire[2]);
//     unique_ptr<Switch[]> switch_toggle(new Switch[1]);

//     a = wire[0].connect() - battery[0].connect();
//     switch_toggle[0].toggle(a, switchToggle);
//     load[0].connect();
//     b = wire[1].connectWith(load[0]);
//     c = battery[0].connect() - b;

//     if (c == 1) {
//         // cout << "The circuit is on" << endl;
//     }
//     else {
//         // cout << "The circuit is off" << endl;
//     }
// }

int main() {
  int batteryNumber = 0;
  bool lineOn = false;
  bool switchOn = true;
  bool batteryAdd = false;
  bool item = false;
  int selectedItem = -1;

  vector<Line> lines;

  RenderWindow window(VideoMode(1366, 768), "Simulation",
                      Style::Close | Style::Resize);
  initializeGrid(numRows, cellSize, numCols, grid);

  ImGui::SFML::Init(window);

  Clock deltaClock;

  MenuList menu;

  // Dynamically load textures from component classes
  vector<Texture> textures;
  vector<string> imagePaths = {
      Resistor::getImagePath(), Battery::getImagePath(),
      Inductor::getImagePath()
      // Add other component paths as needed
  };

  for (const auto& path : imagePaths) {
    Texture texture;
    if (texture.loadFromFile(path)) {
      textures.push_back(texture);
    } else {
      cout << "Error in loading image: " << path << endl;
    }
  }
  menu.setTextures(textures);

  // vector<unique_ptr<ElementRender>> elementRenders;
  vector<unique_ptr<DraggableElement>> components;

  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      if (event.type == Event::Closed) {
        window.close();
      } else if (event.type == Event::MouseButtonPressed) {
        if (event.mouseButton.button == Mouse::Left) {
          Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);

          for (auto& component : components) {
            if (component->contains(mousePosition)) {
              component->startDragging(mousePosition);
            }
          }

          if (lineOn) {
            lines.push_back(Line(mousePosition, mousePosition));
          }
        }
      } else if (event.type == Event::MouseButtonReleased) {
        if (event.mouseButton.button == Mouse::Left) {
          for (auto& component : components) {
            component->stopDragging();
          }
          // Bulb.stopDragging();
          if (lineOn) {
            lines.back().points[1].position =
                Vector2f(event.mouseButton.x, event.mouseButton.y);
          }
        }
      }
    }

    if (Mouse::isButtonPressed(Mouse::Left)) {
      Vector2f mousePos(Mouse::getPosition(window));
      for (auto& component : components) {
        component->updatePosition(mousePos);
      }

      if (lineOn && !lines.empty()) {
        lines.back().points[1].position = mousePos;
      }
    }

    ImGui::SFML::Update(window, deltaClock.restart());
    window.clear(Color::Black);

    ImVec2 windowSize(window.getSize().x, window.getSize().y);

    ImGui::SetNextWindowSize(
        windowSize,
        ImGuiCond_Always);  // Set ImGui window size to match SFML window size
    ImGui::SetNextWindowPos(
        ImVec2(0, 0),
        ImGuiCond_Always);  // Set ImGui window position to top-left corner

    ImGui::Begin("Main Layout", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove);

    // Left menu area
    ImGui::BeginChild("Menu", ImVec2(200, 0), true,
                      ImGuiWindowFlags_NoResize);  // Fixed width for the menu
    ImGui::Text("Menu");

    menu.drawMenu();

    ImGui::Checkbox("Switch", &switchOn);
    if (ImGui::Button("Add Battery")) {
      batteryAdd = true;
      batteryNumber++;
    }
    if (ImGui::Button("Draw Line")) {
      lineOn = !lineOn;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right content area with grid rendering
    ImGui::BeginChild("Content", ImVec2(0, 0), true, ImGuiWindowFlags_NoResize);

    // Draw the grid inside the ImGui child window
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos();

    drawGrid(drawList, offset);

    // Ensure we get the mouse position relative to the window
    ImVec2 mousePos = ImGui::GetMousePos();
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !menu.isItemPlaced()) {
      string selectedComponent = menu.getSelectedComponentName();
      ImTextureID textureID = menu.getSelectedTextureID();

      if (!selectedComponent.empty() && textureID) {
        auto component =
            menu.createComponent(selectedComponent, mousePos, 45.0f);
        if (component) {
          auto elementRender = dynamic_cast<DraggableElement*>(component);
          if (elementRender) {
            // elementRender->setTexture(textureID);
            components.push_back(unique_ptr<DraggableElement>(elementRender));
            menu.setItemPlaced(true);
          }
        }
      }
    }

    ImGui::EndChild();
    ImGui::End();
    ImGui::SFML::Render(window);

    for (auto& component : components) {
      component->draw(window);
    }

    for (auto& line : lines) {
      window.draw(line.points, 2, Lines);
    }

    window.display();
  }

  ImGui::SFML::Shutdown();
  return 0;
}