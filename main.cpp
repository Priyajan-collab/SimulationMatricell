
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

class CircuitElement {
 public:
  bool is_connected = false;

  virtual int connect() const = 0;
};

class Battery : public CircuitElement, public Draggable, public CircleShape {
 public:
  Battery(float x, float y, float r) : CircleShape(r) {
    setPosition(x, y);
    setFillColor(Color::Red);
  }

  bool contains(Vector2f mousePosition) const override {
    return getGlobalBounds().contains(mousePosition);
  }

  void startDragging(Vector2f mousePosition) override {
    isDragging = true;
    dragOffset = getPosition() - mousePosition;
  }

  void stopDragging() override { isDragging = false; }

  void updatePosition(Vector2f mousePosition) override {
    Vector2f position;

    // Calculate grid cell coordinates
    int col = static_cast<int>((mousePosition.x + dragOffset.x - getRadius()) /
                               cellSize);
    int row = static_cast<int>((mousePosition.y + dragOffset.y - getRadius()) /
                               cellSize);

    if (row >= 0 && row < numRows && col >= 0 && col < numCols) {
      // Calculate position to place object at the center of the grid cell
      float centerX = col * cellSize + cellSize / 2.0f - getRadius();
      float centerY = row * cellSize + cellSize / 2.0f - getRadius();

      // Set the position accordingly
      position.x = centerX;
      position.y = centerY;
    }

    if (isDragging) {
      setPosition(position - dragOffset);
    }
  }

  void draw(RenderWindow& window) override { window.draw(*this); }

  int connect() const override { return 1; }
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

  vector<Line> lines;
  vector<unique_ptr<Draggable>> components;
  components.emplace_back(make_unique<Draggable>(45, 20, 200, 200));

  RenderWindow window(VideoMode(1366, 768), "Simulation",
                      Style::Close | Style::Resize);
  initializeGrid(numRows, cellSize, numCols, grid);

  ImGui::SFML::Init(window);

  Clock deltaClock;

  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      if (event.type == Event::Closed) {
        window.close();
      } else if (event.type == Event::MouseButtonPressed) {
        if (event.mouseButton.button == Mouse::Left) {
          Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
          // if (Bulb.shape.getGlobalBounds().contains(mousePosition)) {
          //     Bulb.startDragging(mousePosition);
          // }
          for (auto& component : components) {
            if (component->contains(mousePosition)) {
              component->startDragging(mousePosition);
            }
          }

          if (lineOn) {
            lines.push_back(Line(mousePosition, mousePosition));
          }

          if (batteryAdd) {
            components.emplace_back(
                make_unique<Battery>(mousePosition.x, mousePosition.y, 20));
            batteryAdd = false;
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
      // Bulb.updatePosition(mousePos);
      for (auto& component : components) {
        component->updatePosition(mousePos);
      }

      if (lineOn && !lines.empty()) {
        lines.back().points[1].position = mousePos;
      }
    }

    ImGui::SFML::Update(window, deltaClock.restart());

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

    ImGui::EndChild();
    ImGui::End();

    window.clear(Color::Black);
    for (auto& component : components) {
      Draggable* bulb = dynamic_cast<Draggable*>(component.get());
      if (bulb) {
        if (switchOn) {
          bulb->setColor(Color::Red);
        } else {
          bulb->setColor(Color::Black);
        }
      }
    }

    ImGui::SFML::Render(window);
    // Bulb.draw(window);
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
