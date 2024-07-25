
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include <memory>

#include "customlib/grid/grid.hpp"
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
  bool isRotating;
  Vector2f dragOffset;
  float rotateOffset;
  bool on;

  Vector2f imageSize;
  Vector2f rectSize;
  float radius = 10;
  float space = 40;

 public:
  RectangleShape dragRect;  // Invisible rectangle for dragging
  CircleShape node1;
  CircleShape node2;
  Sprite imageSprite;    // Image to be dragged
  Texture imageTexture;  // Image texture
  static int id;
  static int quadrant;
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
    node1.setRadius(radius);
    node2.setRadius(radius);
    node1.setFillColor(Color::Yellow);
    // node1.setOutlineThickness(1);
    node2.setFillColor(Color::Yellow);
    dragRect.setOrigin(rectSize /
                       2.0f);  // Set the origin to the center of the rectangle
    node1.setOrigin(rectSize /
                    5.0f);  // Set the origin to the center of the rectangle
    node2.setOrigin(rectSize /
                    2.0f);  // Set the origin to the center of the rectangle
    node1.setPosition(Vector2f(centerPos.x + space, centerPos.y - 10));
    node2.setPosition(Vector2f(centerPos.x - space, centerPos.y - 5));
    dragRect.setPosition(centerPos);

    //
  }

  void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(node1);
    window.draw(node2);
    window.draw(dragRect);
    id++;
  }

  bool contains(Vector2f mousePosition) const {
    return dragRect.getGlobalBounds().contains(mousePosition);
  }
  bool containsNode(Vector2f mousePosition) const {
    return node1.getGlobalBounds().contains(mousePosition) ||
           node2.getGlobalBounds().contains(mousePosition);
  }

  void startDragging(Vector2f mousePosition) {
    isDragging = true;
    dragOffset = imageSprite.getPosition() - mousePosition;
  }

  void startRotating(Vector2f mousePosition) {
      int x = imageSprite.getRotation();
      cout<<quadrant<<endl;
      imageSprite.setRotation(x + 90);

      if (quadrant == 3){
        Vector2f pos(imageSprite.getPosition());
        Vector2f centerPos(pos.x + (imageSize.x) / 2.0f,
                          pos.y + (imageSize.y) / 2.0f);
        dragRect.setPosition(centerPos);
        quadrant = 0;
      }
      else if(quadrant == 0 ){
        Vector2f pos(imageSprite.getPosition());
        Vector2f centerPos(pos.x - (imageSize.y) / 2.0f,
                          pos.y + (imageSize.x) / 2.0f);
        dragRect.setPosition(centerPos);
        quadrant ++;
      }else if(quadrant==1){
        Vector2f pos(imageSprite.getPosition());
        Vector2f centerPos(pos.x - (imageSize.x) / 2.0f,
                          pos.y - (imageSize.y) / 2.0f);
        dragRect.setPosition(centerPos);
        quadrant++;
      }else if(quadrant==2){
        Vector2f pos(imageSprite.getPosition());
        Vector2f centerPos(pos.x + (imageSize.y) / 2.0f,
                          pos.y - (imageSize.x) / 2.0f);
        dragRect.setPosition(centerPos);
        quadrant ++;
      }
  }

  void stopDragging() { isDragging = false; }
  void stopRotating() { isRotating = false; }

  void updatePosition(Vector2f mousePosition) {
    if (isDragging) {
      int col = static_cast<int>((mousePosition.x + dragOffset.x ) / cellSize);
      int row = static_cast<int>((mousePosition.y + dragOffset.y) / cellSize);

      Vector2f newPosition (grid[row][col].position.x , grid[row][col].position.y - 3.0f) ;
      // if(quadrant == 0){
      Vector2f centerPos;
      if (quadrant == 0){
        centerPos = Vector2f(newPosition.x + (imageSize.x) / 2.0f,
                          newPosition.y + (imageSize.y) / 2.0f);
      }
      else if(quadrant == 1 ){
        centerPos = Vector2f(newPosition.x - (imageSize.y) / 2.0f,
                          newPosition.y + (imageSize.x) / 2.0f);
      }else if(quadrant==2){
        
        centerPos = Vector2f(newPosition.x - (imageSize.x) / 2.0f,
                          newPosition.y - (imageSize.y) / 2.0f);

      }else if(quadrant==3){
        
        centerPos = Vector2f(newPosition.x + (imageSize.y) / 2.0f,
                          newPosition.y - (imageSize.x) / 2.0f);
      }
        imageSprite.setPosition(newPosition);
        node1.setPosition(Vector2f(centerPos.x + space, centerPos.y - 10));
        node2.setPosition(Vector2f(centerPos.x - space, centerPos.y - 5));

        dragRect.setPosition(centerPos);
      // }

    }
  }
  void updateRotation() {}
  int connectedA(int a) {
    if (a) {
      return 1;
      on = true;
    } else {
      return 0;
    }
  }
  int connectedB() { return 1; }
};
int DraggableElement::id = 0;
int DraggableElement::quadrant = 0;

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
      : Component(), DraggableElement(Vector2f(pos.x, pos.y), image) {}
  static const string& getImagePath() { return image; }
  // wire is connected to resistor
};

const string Resistor::image = "textures/ResistorIcon.png";

class Battery : public Component, public DraggableElement {
 public:
  static const string image;

  Battery(ImVec2 pos, float initialVar = 45.0f)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y), image) {
    cout << "Battery is made" << endl;
  }

  static const string& getImagePath() { return image; }
};

const string Battery::image = "textures/BatteryIcon.png";

class Inductor : public Component, public DraggableElement {
 public:
  static const string image;

  Inductor(ImVec2 pos, float initialVar = 45.0f)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y), image) {
    cout << "Inductorery is made" << endl;
  }

  static const string& getImagePath() { return image; }
};
const string Inductor::image = "textures/InductorIcon.png";

class Bulb : public Component, public DraggableElement {
 public:
  static const string image;

  Bulb(ImVec2 pos, float initialVar = 45.0f)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y), image) {
    cout << "Bulb is made" << endl;
  };
  static const string& getImagePath() { return image; }
  void TurnOn() {
    if (on) {
      node1.setFillColor(Color::Red);
    }
  }
};
const string Bulb::image = "textures/ball.png";

class MenuList {
 private:
  vector<Texture> textures;
  vector<ImTextureID> textureIDs;
  int selectedItem;  // Track selected item index
  string components[7];
  bool itemPlaced;
  static int id;

 public:
  MenuList()
      : selectedItem(-1),
        components{"Resistor", "Battery",    "Inductor", "Bulb", "Capacitor",
                   "Diode",    "Transistor"} {
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
    for (size_t i = 0; i < 7; ++i) {  // Ensure we use the correct size
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
    } else if (type == "Bulb") {
      return new Bulb(pos, initialVar);
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
// not Necessary
// class Wire : public CircuitElement {
//  public:
//   int connect() const override { return 1; }

//   int connectWith(const CircuitElement& element) const {
//     return element.is_connected ? 1 : 0;
//   }
// };

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
  int connectedA(int a) {
    if (a) {
      return 1;
    } else {
      return 0;
    }
  }
  int connectedB() { return 1; }
};

// void circuitConnection(bool switchToggle) {
//   int a, b, c;
//   unique_ptr<Battery[]> battery(new Battery[1]);
//   unique_ptr<Load[]> load(new Load[1]);
//   unique_ptr<Wire[]> wire(new Wire[2]);
//   unique_ptr<Switch[]> switch_toggle(new Switch[1]);

//   a = wire[0].connect() - battery[0].connect();
//   switch_toggle[0].toggle(a, switchToggle);
//   load[0].connect();
//   b = wire[1].connectWith(load[0]);
//   c = battery[0].connect() - b;

//   if (c == 1) {
//     // cout << "The circuit is on" << endl;
//   } else {
//     // cout << "The circuit is off" << endl;
//   }
// }
class connection {
 public:
  connection() {}
};

void connectElements() { cout << "trying to connect" << endl; }
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
      Inductor::getImagePath(), Bulb::getImagePath()
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

              if (Mouse::isButtonPressed(Mouse::Right)) {
                component->startRotating(mousePosition);
              }
            }
            // line yeta bata connect hunxa
            if (component->containsNode(mousePosition)) {
              connectElements();
              lines.push_back(Line(mousePosition, mousePosition));
              for (auto& line : lines) {
                line.connectedA(component->connectedB());
              }
            }
          }

          if (lineOn) {
            lines.push_back(Line(mousePosition, mousePosition));
          }
        } else if (event.mouseButton.button == Mouse::Right) {
          Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
          for (auto& component : components) {
            if (component->contains(mousePosition)) {
              if (Mouse::isButtonPressed(Mouse::Right)) {
                component->startRotating(mousePosition);
              }
            }
          }
        }
      } else if (event.type == Event::MouseButtonReleased) {
        if (event.mouseButton.button == Mouse::Left) {
          Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
          for (auto& component : components) {
            component->stopDragging();
            // line yeta ayera end hunxa
            if (component->containsNode(mousePosition)) {
              lines.back().points[1].position =
                  Vector2f(event.mouseButton.x, event.mouseButton.y);
              for (auto& line : lines) {
                component->connectedA(line.connectedB());
              }
            }
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