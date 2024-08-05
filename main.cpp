#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>

#include "../customlib/grid/grid.hpp"
#include "imgui-SFML.h"

using namespace sf;
using namespace std;

const float cellSize = 20.0f;
const int numRows = 50;
const int numCols = 70;
int col = 0;
int row = 0;
bool complete = false;
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
  bool isRotating;
  Vector2f dragOffset;
  float rotateOffset;


  Vector2f imageSize;
  Vector2f rectSize;
  float radius = 5;
  float space = 40;

  // Static Font Declaration

 public:
  bool isDragging;
  bool on = false;
  bool onagain = false;
  static Font font;

  RectangleShape dragRect;  // Invisible rectangle for dragging
  CircleShape node1;
  CircleShape node2;
  Sprite imageSprite;    // Image to be dragged
  Texture imageTexture;  // Image texture
  static int id;
  static int quadrant;
  bool showInputBox = false;
  static int number_of_obj;
  int id_component;
  string label;
  Vector2f mousepox;

  // Variable display members
  RectangleShape variableBox;
  Text variableText;

  DraggableElement(const Vector2f& position,Vector2f imgSize,  string imagePath)
        : isDragging(false),
          imageSize(imgSize),
          rectSize(Vector2f(45, 15)) {
    // Load the image texture
    number_of_obj++;
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

    // Initialize node1 and node2 positions
    node1.setRadius(radius);
    node2.setRadius(radius);
    node1.setFillColor(Color(0, 0, 0, 180));
    node2.setFillColor(Color(0, 0, 0, 180));

    // Set origin for nodes
    node1.setOrigin(radius, radius);
    node2.setOrigin(radius, radius);

    node1.setPosition(Vector2f(centerPos.x - imageSize.x / 2, centerPos.y));
    node2.setPosition(Vector2f(centerPos.x + imageSize.x / 2, centerPos.y));

    dragRect.setOrigin(rectSize /
                       2.0f);  // Set the origin to the center of the rectangle
    dragRect.setPosition(centerPos);

    // Initialize variable box and text
    variableBox.setSize(Vector2f(60, 25));
    variableBox.setFillColor(
        Color(0, 0, 0, 180));  // Semi-transparent background
    variableBox.setOutlineColor(Color::White);
    variableBox.setOutlineThickness(1);

    variableText.setFont(font);
    variableText.setCharacterSize(14);
    variableText.setFillColor(Color::White);
  }

  virtual void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(node1);
    window.draw(node2);
    window.draw(dragRect);
    // Update and draw the variable box
    drawVariableBox(window);
    id++;
  }

  virtual void handleInputBox() {}

  virtual void drawVariableBox(RenderWindow& window) {}

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
    cout << quadrant << endl;
    imageSprite.setRotation(x + 90);

    if (quadrant == 3) {
      Vector2f pos(imageSprite.getPosition());
      Vector2f centerPos(pos.x + (imageSize.x) / 2.0f,
                         pos.y + (imageSize.y) / 2.0f);
      node1.setPosition(
          Vector2f(centerPos.x - imageSize.x / 2.0f, centerPos.y));
      node2.setPosition(
          Vector2f(centerPos.x + imageSize.x / 2.0f, centerPos.y));
      dragRect.setPosition(centerPos);
      quadrant = 0;
    } else if (quadrant == 0) {
      Vector2f pos(imageSprite.getPosition());
      Vector2f centerPos(pos.x - (imageSize.y) / 2.0f,
                         pos.y + (imageSize.x) / 2.0f);

      node1.setPosition(Vector2f(pos.x - imageSize.y / 2.0f, pos.y));
      node2.setPosition(
          Vector2f(pos.x - imageSize.y / 2.0f, pos.y + imageSize.x));
      dragRect.setPosition(centerPos);
      quadrant++;
    } else if (quadrant == 1) {
      Vector2f pos(imageSprite.getPosition());
      Vector2f centerPos(pos.x - (imageSize.x) / 2.0f,
                         pos.y - (imageSize.y) / 2.0f);
      node1.setPosition(Vector2f(pos.x, pos.y - imageSize.y / 2.0f));
      node2.setPosition(
          Vector2f(pos.x - imageSize.x, pos.y - imageSize.y / 2.0f));
      dragRect.setPosition(centerPos);
      quadrant++;
    } else if (quadrant == 2) {
      Vector2f pos(imageSprite.getPosition());
      Vector2f centerPos(pos.x + (imageSize.y) / 2.0f,
                         pos.y - (imageSize.x) / 2.0f);
      node1.setPosition(Vector2f(pos.x + imageSize.y / 2.0f, pos.y));
      node2.setPosition(
          Vector2f(pos.x + imageSize.y / 2.0f, pos.y - imageSize.x));
      dragRect.setPosition(centerPos);
      quadrant++;
    }
  }

  void stopDragging() { isDragging = false; }
  void stopRotating() { isRotating = false; }

  void updatePosition(Vector2f mousePosition) {
    mousepox = mousePosition;
    if (isDragging) {
      int col = static_cast<int>((mousePosition.x + dragOffset.x) / cellSize);
      int row = static_cast<int>((mousePosition.y + dragOffset.y) / cellSize);

      Vector2f newPosition(grid[row][col].position.x,
                           grid[row][col].position.y);

      Vector2f centerPos;
      if (quadrant == 0) {
        centerPos = Vector2f(newPosition.x + (imageSize.x) / 2.0f,
                             newPosition.y + (imageSize.y) / 2.0f);
        node1.setPosition(
            Vector2f(newPosition.x, newPosition.y + imageSize.y / 2.0f));
        node2.setPosition(Vector2f(newPosition.x + imageSize.x,
                                   newPosition.y + imageSize.y / 2.0f));
      } else if (quadrant == 1) {
        centerPos = Vector2f(newPosition.x - (imageSize.y) / 2.0f,
                             newPosition.y + (imageSize.x) / 2.0f);
        node1.setPosition(
            Vector2f(newPosition.x - imageSize.y / 2.0f, newPosition.y));
        node2.setPosition(Vector2f(newPosition.x - imageSize.y / 2.0f,
                                   newPosition.y + imageSize.x));
      } else if (quadrant == 2) {
        centerPos = Vector2f(newPosition.x - (imageSize.x) / 2.0f,
                             newPosition.y - (imageSize.y) / 2.0f);
        node1.setPosition(
            Vector2f(newPosition.x, newPosition.y - imageSize.y / 2.0f));
        node2.setPosition(Vector2f(newPosition.x - imageSize.x,
                                   newPosition.y - imageSize.y / 2.0f));

      } else if (quadrant == 3) {
        centerPos = Vector2f(newPosition.x + (imageSize.y) / 2.0f,
                             newPosition.y - (imageSize.x) / 2.0f);
        node1.setPosition(
            Vector2f(newPosition.x + imageSize.y / 2.0f, newPosition.y));
        node2.setPosition(Vector2f(newPosition.x + imageSize.y / 2.0f,
                                   newPosition.y - imageSize.x));
      }
      imageSprite.setPosition(newPosition);

      dragRect.setPosition(centerPos);
    }
  }

  void updateRotation() {}

  int connectedA(int a) { on = a; }
  void connectedB(bool b) { onagain = b; }
  virtual void TurnOn(bool) {};
};

// Static member definition
Font DraggableElement::font;

int DraggableElement::id = 0;
int DraggableElement::quadrant = 0;
int DraggableElement::number_of_obj = 0;
class Component {
 public:
  virtual ~Component() {}
  // virtual static const string& getImagePath() const = 0;
};
class ANDGATE : public DraggableElement, public Component {
 public:
  static const string image;
  CircleShape input1, input2, output;
  bool i0 = false, i1 = false;

  ANDGATE(ImVec2 pos,Vector2f imgSize, float initialVar = 45.0f)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y),imgSize,  image) {
    // circle.setPosition(
    //     Vector2f(node1.getPosition().x - 60, node1.getPosition().y - 3));
    // circle.setRadius(2);
    input1.setRadius(radius);
    input2.setRadius(radius);
    output.setRadius(radius);
    input1.setFillColor(Color::Yellow);
    input2.setFillColor(Color::Yellow);
    output.setFillColor(Color::Yellow);

    node1.setRadius(0);
    node2.setRadius(0);
    input1.setPosition(
        Vector2f(node1.getPosition().x - 10, node1.getPosition().y - 25));
    input2.setPosition(
        Vector2f(node1.getPosition().x - 10, node1.getPosition().y + 20));
    output.setPosition(Vector2f(node2.getPosition().x, node2.getPosition().y));
    // circle.setFillColor(Color::White);

    label = "ANDGATE", cout << "ANDGATE is made" << endl;
  };
  virtual void logic() {
    if (!i0) {
      input1.setFillColor(Color::Yellow);
    } else {
      input1.setFillColor(Color::Red);
    }
    if (!i1) {
      input2.setFillColor(Color::Yellow);
    } else {
      input2.setFillColor(Color::Red);
    }
    if (!i0 || !i1) {
      output.setFillColor(Color::Yellow);
    } else {
      output.setFillColor(Color::Red);
    }
  }
  static const string& getImagePath() { return image; }
  bool containsInput1(Vector2f mousePosition) const {
    return input1.getGlobalBounds().contains(mousePosition);
  }
  bool containsInput2(Vector2f mousePosition) const {
    return input2.getGlobalBounds().contains(mousePosition);
  }

  void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(node1);
    window.draw(node2);
    window.draw(dragRect);
    window.draw(input1);
    window.draw(input2);
    window.draw(output);
    id++;
  }
};
const string ANDGATE::image = "textures/ANDGATE.png";

class ORGATE : public ANDGATE {
 public:
  static const string image;

  string label = "ORGATE";
  ORGATE(ImVec2 pos, Vector2f imgSize, float intialVar) : ANDGATE(pos,imgSize, intialVar) {};

  virtual void logic() {
    if (!i0) {
      input1.setFillColor(Color::Yellow);
    } else {
      input1.setFillColor(Color::Red);
    }
    if (!i1) {
      input2.setFillColor(Color::Yellow);
    } else {
      input2.setFillColor(Color::Red);
    }
    if (!i0 && !i1) {
      output.setFillColor(Color::Yellow);
    } else {
      output.setFillColor(Color::Red);
    }
  }
};
const string ORGATE::image = "textures/ball.png";

class Resistor : public Component, public DraggableElement {
  static const string image;
  float resistance;
  float voltage;

 public:

  static int countResistor;
  int id_resistor = countResistor;
   Resistor(ImVec2 pos, Vector2f imgSize,float initialVar = 45.0f)
      : Component(),
        DraggableElement(Vector2f(pos.x, pos.y),imgSize, image),
        resistance(initialVar) {
    countResistor++;
        }

  static const string& getImagePath() { return image; }

  void drawVariableBox(RenderWindow& window) {
    ostringstream oss;
    oss.precision(1);  // Set precision to 1 decimal place
    oss << fixed << resistance;

    // Set the position for the variable box and text
    Vector2f boxPosition =
        imageSprite.getPosition() + Vector2f(10, imageSize.y / 2 + 10);
    variableBox.setPosition(boxPosition);

    // Update text content and position
    variableText.setString(oss.str() + " Ohm");  // Use actual variable value
    variableText.setPosition(boxPosition.x + 5, boxPosition.y + 5);

    // Draw box and text
    window.draw(variableBox);
    window.draw(variableText);
  }
  void handleInputBox() {
    // Set focus to the input box if it's shown
    if (showInputBox) {
      ImGui::SetNextWindowFocus();
    }
    ImGui::SetNextWindowSize(ImVec2(150, 80), ImGuiCond_Always);
    // Begin the ImGui window
    ImGui::Begin("Set Resistance", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    // Input field for float values
    ImGui::InputFloat(" ", &resistance);
    // Close button
    if (ImGui::Button("Close")) {
      showInputBox = false;  // Hide the input box
    }
    ImGui::End();
  }
  float getResistance() { return resistance; }
  float getVolatageDrop(float totalResistance, float totalVoltage) {
    float value = totalVoltage * ((resistance) / (totalResistance));
    return value;
  }
};

int Resistor::countResistor = 0;
const string Resistor::image = "textures/ResistorIcon.png";

class Battery : public Component, public DraggableElement {
  float voltage;

 public:
  static const string image;

  Battery(ImVec2 pos, Vector2f imgSize, float initialVar = 45.0f)
      : Component(),
        DraggableElement(Vector2f(pos.x, pos.y),imgSize,  image),
        voltage(initialVar) {
    cout << "Battery is made" << endl;
  }

  static const string& getImagePath() { return image; }
  void TurnOn() {
    cout << "hey" << endl;
    node1.setFillColor(Color::Black);
    node2.setFillColor(Color::Black);
  }

  void drawVariableBox(RenderWindow& window) {
    ostringstream oss;
    oss.precision(1);  // Set precision to 1 decimal place
    oss << std::fixed << voltage;

    // Set the position for the variable box and text
    Vector2f boxPosition =
        imageSprite.getPosition() + Vector2f(10, imageSize.y / 2 + 10);
    variableBox.setPosition(boxPosition);

    // Update text content and position
    variableText.setString(oss.str() + " V");  // Use actual variable value
    variableText.setPosition(boxPosition.x + 5, boxPosition.y + 5);

    // Draw box and text
    window.draw(variableBox);
    window.draw(variableText);
  }
  void handleInputBox() {
    // Set focus to the input box if it's shown
    if (showInputBox) {
      ImGui::SetNextWindowFocus();
    }
    ImGui::SetNextWindowSize(ImVec2(150, 80), ImGuiCond_Always);
    // Begin the ImGui window
    ImGui::Begin("Set Voltage", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    // Input field for float values
    ImGui::InputFloat(" ", &voltage);
    // Close button
    if (ImGui::Button("Close")) {
      showInputBox = false;  // Hide the input box
    }
    ImGui::End();
  }
  float getVoltage() { return voltage; }
};

const string Battery::image = "textures/BatteryIcon.png";

class Inductor : public Component, public DraggableElement {
  float inductance;

 public:
  static const string image;

  Inductor(ImVec2 pos, Vector2f imgSize, float initialVar = 45.0f)
      : Component(),
        DraggableElement(Vector2f(pos.x, pos.y),imgSize,  image),
        inductance(initialVar) {
    cout << "Inductor is made" << endl;
  }

  static const string& getImagePath() { return image; }

  void drawVariableBox(RenderWindow& window) {
    ostringstream oss;
    oss.precision(1);  // Set precision to 1 decimal place
    oss << std::fixed << inductance;

    // Set the position for the variable box and text
    Vector2f boxPosition =
        imageSprite.getPosition() + Vector2f(10, imageSize.y / 2 + 10);
    variableBox.setPosition(boxPosition);

    // Update text content and position
    variableText.setString(oss.str() + " H");  // Use actual variable value
    variableText.setPosition(boxPosition.x + 5, boxPosition.y + 5);

    // Draw box and text
    window.draw(variableBox);
    window.draw(variableText);
  }
  void handleInputBox() {
    // Set focus to the input box if it's shown
    if (showInputBox) {
      ImGui::SetNextWindowFocus();
    }
    ImGui::SetNextWindowSize(ImVec2(150, 80), ImGuiCond_Always);
    // Begin the ImGui window
    ImGui::Begin("Set Inductance", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    // Input field for float values
    ImGui::InputFloat(" ", &inductance);
    // Close button
    if (ImGui::Button("Close")) {
      showInputBox = false;  // Hide the input box
    }
    ImGui::End();
  }
  float getInductance() { return inductance; }
};
const string Inductor::image = "textures/InductorIcon.png";

class Bulb : public Component, public DraggableElement {
 public:
  static const string image;
  CircleShape circle;

  Bulb(ImVec2 pos, Vector2f imgSize, float initialVar = 45.0f)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y),imgSize,  image) {
    circle.setPosition(
        Vector2f(node1.getPosition().x - 55, node1.getPosition().y - 3));
    circle.setRadius(radius);
    // circle.setFillColor(Color::White);

    label = "bulb", cout << "Bulb is made" << endl;
  };
  static const string& getImagePath() { return image; }

  void TurnOn(bool toggle) {
    if (toggle) {
      circle.setFillColor(Color::Red);
      cout << "bulb is on" << endl;
    } else {
      circle.setFillColor(Color::Black);
    }
  }

  void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(node1);
    window.draw(node2);
    window.draw(dragRect);
    window.draw(circle);
    id++;
  }
};

const string Bulb::image = "textures/bulb.png";

class Multimeter : public Component, public DraggableElement {
  public:
  static const string image;
  RectangleShape inputBox;

  Multimeter(ImVec2 pos, Vector2f imgSize)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y),imgSize, image) {
        inputBox.setSize(Vector2f(180,50));
        inputBox.setPosition(Vector2f(imageSprite.getPosition()));
        inputBox.setFillColor(Color::Red);
      };
  static const string& getImagePath() { return image; }

  void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(dragRect);
    window.draw(inputBox);
    id++;
  }

   void updatePosition(Vector2f mousePosition) {
        mousepox = mousePosition;
        if (isDragging) {
            int col = static_cast<int>((mousePosition.x + dragOffset.x) / cellSize);
            int row = static_cast<int>((mousePosition.y + dragOffset.y) / cellSize);

            // Vector2f newPosition(grid[row][col].position.x,
            //                      grid[row][col].position.y - 3.0f);
            Vector2f newPosition(grid[row][col].position.x,
                                 grid[row][col].position.y );

            Vector2f centerPos;
            centerPos = Vector2f(newPosition.x + (imageSize.x) / 2.0f,
                                     newPosition.y + (imageSize.y) / 2.0f);
            imageSprite.setPosition(newPosition);
            inputBox.setPosition(imageSprite.getPosition());
            dragRect.setPosition(centerPos);
          }
        }
};

const string Multimeter::image = "textures/multimeter.png";

class MenuList {
 private:
  vector<Texture> textures;
  vector<ImTextureID> textureIDs;
  int selectedItem;  // Track selected item index
  string components[10];
  bool itemPlaced;

 public:
  MenuList()
      : selectedItem(-1),
        components{"Resistor", "Battery",   "Inductor", "Bulb", "Multimeter","ANDGATE",
                   "ORGATE",   "Capacitor", "Diode",    "Transistor"} {
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
    ImGui::NewLine();
    ImGui::BeginChild("MenuList", ImVec2(0, 400), true);
    // Define categories for clarity and easier maintenance
    bool printedElectronicDevicesHeader = false;
    bool printedLogicGatesHeader = false;
    
    for (size_t i = 0; i < 10; ++i) {
        // Correct string comparison
        if (components[i] == "Resistor" && !printedElectronicDevicesHeader) {
            
            ImGui::Text("Electronic Devices: ");
            ImGui::NewLine();
            printedElectronicDevicesHeader = true;
        }
        else if (components[i] == "ANDGATE" && !printedLogicGatesHeader) {
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::Text("Logic Gates: ");
            ImGui::NewLine();
            printedLogicGatesHeader = true;
        }
        
        // Create buttons
        if (ImGui::Button(components[i].c_str(), ImVec2(75, 30))) {
            selectedItem = i;
            itemPlaced = false;
            std::cout << "Selected index: " << i << std::endl;
            std::cout << "Selected: " << components[selectedItem] << std::endl;
        }

        // Move to the next line after every two buttons
        if ((i % 2) == 0) {
            ImGui::SameLine();
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
      return new Resistor(pos,Vector2f(80, 40), initialVar);
    } else if (type == "Battery") {
      return new Battery(pos,Vector2f(80, 40), initialVar);
    } else if (type == "Inductor") {
      return new Inductor(pos,Vector2f(80, 40), initialVar);
    } else if (type == "Bulb") {
      return new Bulb(pos,Vector2f(80, 40), initialVar);
    }else if (type == "Multimeter") {
      return new Multimeter(pos,Vector2f(200,250));
    } 
    else if (type == "ANDGATE") {
      return new ANDGATE(pos,Vector2f(80, 40), initialVar);
    } else if (type == "ANDGATE") {

      return new ORGATE(pos,Vector2f(80, 40), initialVar);
    }

    // Add more cases as needed
    return nullptr;
  }
  bool isItemPlaced() const { return itemPlaced; }

  void setItemPlaced(bool placed) { itemPlaced = placed; }
};

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
    points[0].color = Color::Red;
    points[1].color = Color::Red;
  }
  // int connectedA(int a) {
  //   if (a) {
  //     return 1;
  //   } else {
  //     return 0;
  //   }
  // }
  // int connectedB() { return 1; }
};

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

  if (!DraggableElement::font.loadFromFile("notosans.ttf")) {
    throw runtime_error("Failed to load font");
  }

  RenderWindow window(VideoMode(1366, 768), "Simulation",
                      Style::Close | Style::Resize);
  initializeGrid(numRows, cellSize, numCols, grid);

  ImGui::SFML::Init(window);

  ImGuiIO& io = ImGui::GetIO();
  ImFont* customFont = io.Fonts->AddFontFromFileTTF("notosans.ttf", 18.0f);

  ImGui::SFML::UpdateFontTexture();

  Clock deltaClock;

  MenuList menu;

  // Dynamically load textures from component classes
  vector<Texture> textures;
  vector<string> imagePaths = {
      Resistor::getImagePath(),
      Battery::getImagePath(),
      Inductor::getImagePath(),
      Bulb::getImagePath(),
      Multimeter::getImagePath(),
      ANDGATE::getImagePath(),
      ORGATE::getImagePath()
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

  vector<unique_ptr<DraggableElement>> components;
  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == Event::Closed) {
        window.close();
      }

      else if (event.type == Event::MouseButtonPressed) {
        if (event.mouseButton.button == Mouse::Left) {
          Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);

          for (auto& component : components) {
            if (component->contains(mousePosition)) {
              component->startDragging(mousePosition);

              if (Mouse::isButtonPressed(Mouse::Right)) {
                component->startRotating(mousePosition);
              }
            }
            ANDGATE* ptrand = dynamic_cast<ANDGATE*>(component.get());
            if (ptrand) {
              if (ptrand->containsInput1(mousePosition)) {
                ptrand->i0 = !ptrand->i0;
                ptrand->logic();
                cout << ptrand->i0;
              }
              if (ptrand->containsInput2(mousePosition)) {
                ptrand->i1 = !ptrand->i1;
                ptrand->logic();
              }
            }
            // line yeta bata connect hunxa
            if (component->containsNode(mousePosition)) {
              cout << "contatins : " << component->id_component << endl;
              connectElements();
              lines.push_back(Line(mousePosition, mousePosition));
            }

            if (lineOn) {
              lines.push_back(Line(mousePosition, mousePosition));
              Bulb* ptrbulb = dynamic_cast<Bulb*>(component.get());
              if (ptrbulb) {
                ptrbulb->connectedA(true);
                ptrbulb->connectedB(true);
              }
            }
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

    ImGui::PushFont(customFont);

    ImGui::Begin("Main Layout", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove);

    // Left menu area
    ImGui::BeginChild("Menu", ImVec2(200, 0), true,
                      ImGuiWindowFlags_NoResize);  // Fixed width for the menu
    ImGui::Text("Menu");

    menu.drawMenu();

    ImGui::Checkbox("Switch", &switchOn);
    if (ImGui::Button("switch")) {
    }
    // if (switchOn) {
    //   for (auto& component : components) {
    //     if (component->on && component->onagain) {
    //       component->TurnOn();
    //     }
    //   }
    // }
    if (ImGui::Button("Draw Line")) {
      lineOn = !lineOn;
    }
    if (ImGui::Button("Calculate")) {
      float totalResistance = 0.0f, totalVoltage = 0.0f;
      float voltageDrop;
      vector<float> tVD;
      vector<Resistor> resistors;
      for (auto& component : components) {
        Resistor* resistorPtr = dynamic_cast<Resistor*>(component.get());
        Battery* batteryPtr = dynamic_cast<Battery*>(component.get());

        if (batteryPtr) {
          totalVoltage += batteryPtr->getVoltage();
        }
        if (resistorPtr) {
          resistors.push_back(*resistorPtr);
          totalResistance += resistorPtr->getResistance();
        }
      }
      for (auto& resistor : resistors) {
        float voltageDrop =
            resistor.getVolatageDrop(totalResistance, totalVoltage);
        std::cout << "id:" << resistor.id_resistor
                  << "   ::voltage drop::" << voltageDrop
                  << ",,resistor ko resistance" << resistor.getResistance()
                  << std::endl;
      }
      resistors.clear();
      cout << "Total Current Flowing: " << totalVoltage / totalResistance
           << endl;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right content area with grid rendering
    ImGui::BeginChild("Content", ImVec2(0, 0), true, ImGuiWindowFlags_NoResize);

    // Draw the grid inside the ImGui child window
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos();

    drawGrid(drawList, offset);
    ;
    for (auto& component : components) {
      if (component->label == "bulb") {
        if (switchOn && component->on && component->onagain) {
          component->TurnOn(true);
        } else if (!switchOn | !component->on | !component->onagain) {
          component->TurnOn(false);
        }
      }
    }

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
            components.push_back(unique_ptr<DraggableElement>(elementRender));
            menu.setItemPlaced(true);
          }
        }
      }
    }
    Vector2f mousePosition =
        window.mapPixelToCoords(Mouse::getPosition(window));
    for (auto& component : components) {
      if (component->variableBox.getGlobalBounds().contains(mousePosition) &&
          Mouse::isButtonPressed(Mouse::Left) && !component->isDragging) {
        component->showInputBox = true;
      }
      if (component->showInputBox) {
        component->handleInputBox();
      }
    }
    ImGui::EndChild();
    ImGui::End();
    ImGui::PopFont();
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