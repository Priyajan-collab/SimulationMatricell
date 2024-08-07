#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>

#include "customlib/grid/grid.hpp"

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
  bool isBulb = false;
  // Variable display members
  RectangleShape variableBox;
  Text variableText;

  DraggableElement(const Vector2f& position, Vector2f imgSize, string imagePath)
      : isDragging(false), imageSize(imgSize), rectSize(Vector2f(45, 15)) {
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
    dragRect.setFillColor(Color::Transparent);

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
  virtual void startRotating(Vector2f mousePosition) {
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

  virtual void updatePosition(Vector2f mousePosition) {
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

  void connectedA(bool a) {
    if (a) {
      on = true;
    } else {
      on = false;
    }
  }

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
  bool OR = false;
  bool i0 = false, i1 = false;

  ANDGATE(ImVec2 pos, Vector2f imgSize, float initialVar = 45.0f)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y), imgSize, image) {
    // circle.setPosition(
    //     Vector2f(node1.getPosition().x - 60, node1.getPosition().y - 3));
    // circle.setRadius(2);
    Color offColor(54, 69, 79);
    
    dragRect.setSize(Vector2f(50, 60));
    dragRect.setOrigin(dragRect.getSize().x / 2, dragRect.getSize().y / 2);
    dragRect.setPosition(imageSprite.getPosition().x + imageSize.x/2.0f - 15 ,imageSprite.getPosition().y + imageSize.y/2.0f - 10);
    input1.setRadius(10);
    input2.setRadius(10);
    output.setRadius(10);
    input1.setFillColor(offColor);
    input2.setFillColor(offColor);
    output.setFillColor(offColor);

    node1.setRadius(0);
    node2.setRadius(0);
    input1.setPosition(
        Vector2f(node1.getPosition().x - 30, node1.getPosition().y - 48));
    input2.setPosition(
        Vector2f(node1.getPosition().x - 30, node1.getPosition().y + 5));
    output.setPosition(Vector2f(node2.getPosition().x - 19, node2.getPosition().y - 21));

  };
  //Constructor for  OR GATE
  ANDGATE(ImVec2 pos,Vector2f imgSize, string image)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y),imgSize,  image) {
        OR = true;
    Color offColor(54, 69, 79);
    
    dragRect.setSize(Vector2f(50, 60));
    dragRect.setOrigin(dragRect.getSize().x / 2, dragRect.getSize().y / 2);
    dragRect.setPosition(imageSprite.getPosition().x + imageSize.x/2.0f - 15 ,imageSprite.getPosition().y + imageSize.y/2.0f - 10);
    input1.setRadius(10);
    input2.setRadius(10);
    output.setRadius(10);
    input1.setFillColor(offColor);
    input2.setFillColor(offColor);
    output.setFillColor(offColor);

    node1.setRadius(0);
    node2.setRadius(0);
    input1.setPosition(
        Vector2f(node1.getPosition().x - 33, node1.getPosition().y - 50));
    input2.setPosition(
        Vector2f(node1.getPosition().x - 33, node1.getPosition().y + 3));
    output.setPosition(Vector2f(node2.getPosition().x - 30, node2.getPosition().y - 23));
  };
  virtual void logic() {
    Color offColor(54, 69, 79);
    Color onColor(255, 49, 49);
    if (!i0) {
      input1.setFillColor(offColor);
    } else {
      input1.setFillColor(onColor);
    }
    if (!i1) {
      input2.setFillColor(offColor);
    } else {
      input2.setFillColor(onColor);
    }
    if (!i0 || !i1) {
      output.setFillColor(offColor);
    } else {
      output.setFillColor(onColor);
    }
  }
  static const string& getImagePath() { return image; }
  bool containsInput1(Vector2f mousePosition) const {
    return input1.getGlobalBounds().contains(mousePosition);
  }
  bool containsInput2(Vector2f mousePosition) const {
    return input2.getGlobalBounds().contains(mousePosition);
  }
  void updatePosition(Vector2f mousePosition) {
    mousepox = mousePosition;
    if (isDragging) {
      int col = static_cast<int>((mousePosition.x + dragOffset.x) / cellSize);
      int row = static_cast<int>((mousePosition.y + dragOffset.y) / cellSize);

      Vector2f newPosition(grid[row][col].position.x,
                           grid[row][col].position.y - 3.0f);

      Vector2f centerPos(newPosition.x + (imageSize.x) / 2.0f- 15,
                             newPosition.y + (imageSize.y) / 2.0f-10);
      node1.setPosition(
            Vector2f(newPosition.x, newPosition.y + imageSize.y / 2.0f));
        node2.setPosition(Vector2f(newPosition.x + imageSize.x,
                                   newPosition.y + imageSize.y / 2.0f));
      imageSprite.setPosition(newPosition);
      if(OR){
        input1.setPosition(
        Vector2f(node1.getPosition().x - 33, node1.getPosition().y - 50));
        input2.setPosition(
        Vector2f(node1.getPosition().x - 33, node1.getPosition().y + 3));
        output.setPosition(Vector2f(node2.getPosition().x - 30, node2.getPosition().y - 23));
      }
      else{
      input1.setPosition(
          Vector2f(node1.getPosition().x - 30, node1.getPosition().y - 48));
      input2.setPosition(
          Vector2f(node1.getPosition().x - 30, node1.getPosition().y + 5));
      output.setPosition(Vector2f(node2.getPosition().x - 19, node2.getPosition().y - 21));
      }
      dragRect.setPosition(centerPos);
    }
  }
  void startRotating(Vector2f mousePosition) {
    //Disabling rotation for gates
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
  ORGATE(ImVec2 pos, Vector2f imgSize, float intialVar) : ANDGATE(pos,imgSize,image) {};


  virtual void logic() {
    Color offColor(54, 69, 79);
    Color onColor(255, 49, 49);
    if (!i0) {
      input1.setFillColor(offColor);
    } else {
      input1.setFillColor(onColor);
    }
    if (!i1) {
      input2.setFillColor(offColor);
    } else {
      input2.setFillColor(onColor);
    }
    if (!i0 && !i1) {
      output.setFillColor(offColor);
    } else {
      output.setFillColor(onColor);
    }
  }
};
const string ORGATE::image = "textures/ORGATE.png";

class Resistor : public Component, public DraggableElement {
  static const string image;
  float resistance;
  float voltage;

 public:
  static int countResistor;
  int id_resistor = countResistor;
  Resistor(ImVec2 pos, Vector2f imgSize, float initialVar = 45.0f)
      : Component(),
        DraggableElement(Vector2f(pos.x, pos.y), imgSize, image),
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
    ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_Always);
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
        DraggableElement(Vector2f(pos.x, pos.y), imgSize, image),
        voltage(initialVar) {
  }

  static const string& getImagePath() { return image; }
  void TurnOn(bool toggle) {
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
        DraggableElement(Vector2f(pos.x, pos.y), imgSize, image),
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

class Capacitor: public Component, public DraggableElement{
  public:
    static const string image;

    Capacitor(ImVec2 pos, Vector2f imgSize, float initialVar = 45.0f)
      : Component(),
        DraggableElement(Vector2f(pos.x, pos.y),imgSize,  image) {
          cout << "Capacitor is made" << endl;
        }
    static const string& getImagePath() { return image; }
};
const string Capacitor::image = "textures/CapacitorIcon.png";

class Bulb : public Component, public DraggableElement {
 public:
  static const string image;
  bool toggle;

  Bulb(ImVec2 pos, Vector2f imgSize, float initialVar = 45.0f)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y), imgSize, image) {
        isBulb = true;
        // node1.setPosition(Vector2f(centerPos.x - imageSize.x / 2, centerPos.y));
        // node2.setPosition(Vector2f(centerPos.x + imageSize.x / 2, centerPos.y));
      };
  static const string& getImagePath() { return image; }

  void TurnOn(bool toggle) {
    if (toggle) {
      if (!imageTexture.loadFromFile("textures/BulbOn.png")) {
        throw runtime_error("Failed to load image texture");
      }
      imageSprite.setTexture(imageTexture);

    } else {
      if (!imageTexture.loadFromFile("textures/BulbOff.png")) {
        throw runtime_error("Failed to load image texture");
      }
      imageSprite.setTexture(imageTexture);
    }
  }

  void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(node1);
    window.draw(node2);
    window.draw(dragRect);
    id++;
  }
};

const string Bulb::image = "textures/BulbOff.png";

class Multimeter : public Component, public DraggableElement {
  public:
  static const string image;
  RectangleShape inputBox;
  RectangleShape inputBox2;
  float value;
  vector<Text> voltageTexts; 
  vector<float> voltageDrop;
   Font font;
   bool checkMulti = false;

  Multimeter(ImVec2 pos, Vector2f imgSize)
      : Component(), DraggableElement(Vector2f(pos.x, pos.y),imgSize, image) {

        dragRect.setSize(Vector2f(100, 100));
        dragRect.setOrigin(dragRect.getSize().x / 2, dragRect.getSize().y / 2);
        dragRect.setPosition(imageSprite.getPosition());
        inputBox.setSize(Vector2f(152,41));
        inputBox2.setSize(Vector2f(152,102));

        Color input1Color(149, 186, 194);
        Color input2Color(23, 23, 23);
        inputBox.setFillColor(input1Color);
        inputBox2.setFillColor(input2Color);

        if (!font.loadFromFile("notosans.ttf")) { // Specify the path to your font file
            cout << "Error loading font" << endl;
            // Handle the error appropriately
        }

        // Set the font for the main variable text
        variableText.setFont(font);
        variableText.setCharacterSize(17); // Set character size for the main variable text
        variableText.setColor(Color::Black);
        // variableBox.setOutlineThickness(10);
        variableText.setStyle(Text::Bold);

      };
      
  static const string& getImagePath() { return image; }

  void draw(RenderWindow& window) {
    window.draw(imageSprite);
    window.draw(dragRect);
    window.draw(inputBox);
    drawVariableBox(window);
    id++;
  }
  void drawVariableBox(RenderWindow& window) {
        ostringstream oss;
        oss.precision(1);  // Set precision to 1 decimal place
        oss << fixed << value;

        // Position for the main value box
        inputBox.setPosition(imageSprite.getPosition().x - 75, imageSprite.getPosition().y - 70);
        inputBox2.setPosition(imageSprite.getPosition().x - 75 , imageSprite.getPosition().y - 10);
        if(checkMulti){
          variableText.setString(oss.str() + " A");  // Use actual variable value
        }
        else{
          variableText.setString("0 A");  // Use actual variable value
        }
        variableText.setPosition(inputBox.getPosition().x + 6, inputBox.getPosition().y + 9);

        // Draw the main value box and text
        window.draw(inputBox);
        window.draw(inputBox2);
        window.draw(variableText);

        // Draw voltage drops
        float yOffset = 0;
        
        for (auto& text : voltageTexts) {
            text.setPosition(inputBox2.getPosition().x + 6, inputBox2.getPosition().y + 5 + yOffset);
            window.draw(text);
            yOffset += 20;  // Adjust spacing between texts
        }
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
            inputBox.setPosition(imageSprite.getPosition().x - 75,imageSprite.getPosition().y - 69);
            inputBox2.setPosition(imageSprite.getPosition().x - 100,imageSprite.getPosition().y - 30);
            dragRect.setPosition(imageSprite.getPosition());
          } 
        }
        void setValue(float val) {
        value = val;
        }

      void setVoltageDrop(int id, float volDrop) {
        
        voltageDrop.emplace_back(volDrop);

        // Create a new Text object for the voltage drop
        Text voltageText;
        ostringstream voltageOss;
        voltageOss.precision(1);  // Set precision to 1 decimal place
        voltageOss << fixed << volDrop;
        voltageText.setString("R" + to_string(id + 1)+ ": " + voltageOss.str() + " V");

        // You need to set the font, character size, and fill color for each text
        voltageText.setFont(font);
        voltageText.setCharacterSize(14);
        voltageText.setFillColor(Color::White);
        voltageText.setStyle(Text::Bold);

        voltageTexts.push_back(voltageText);  // Add the text to the vector
    }
    void clearVoltageTexts(){
      voltageTexts.clear();
    }
};

const string Multimeter::image = "textures/multimeter.png";


class MenuList {
 private:
  vector<Texture> textures;
  vector<ImTextureID> textureIDs;
  int selectedItem;  // Track selected item index
  string components[8];
  bool itemPlaced;

 public:
  MenuList()
      : selectedItem(-1),

        components{"Resistor", "Battery",   "Inductor", "Bulb", "Multimeter" ,"Capacitor","ANDGATE",
                   "ORGATE"} {

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
    
    for (size_t i = 0; i < 8; ++i) {
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
    if (selectedItem >= 0 && selectedItem < 8) {
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
      return new Resistor(pos, Vector2f(80, 40), initialVar);
    } else if (type == "Battery") {
      return new Battery(pos, Vector2f(80, 40), initialVar);
    } else if (type == "Inductor") {
      return new Inductor(pos, Vector2f(80, 40), initialVar);
    } else if (type == "Bulb") {
      return new Bulb(pos,Vector2f(65, 50), initialVar);
    }else if (type == "Multimeter") {
      return new Multimeter(pos,Vector2f(200,250));
    } else if (type == "Capacitor") {
      return new Capacitor(pos,Vector2f(80, 40), initialVar);
    } else if (type == "ANDGATE") {
      return new ANDGATE(pos,Vector2f(170, 105), initialVar);
    } else if (type == "ORGATE") {
      return new ORGATE(pos,Vector2f(180, 105), initialVar);

    }

    // Add more cases as needed
    return nullptr;
  }
  bool isItemPlaced() const { return itemPlaced; }

  void setItemPlaced(bool placed) { itemPlaced = placed; }
};

struct Line {
  Vertex points[2];

  Line(Vector2f start, Vector2f end) {
    points[0].position = start;
    points[1].position = end;
    points[0].color = Color::Red;
    points[1].color = Color::Red;
  }
  
};

int main() {
  int batteryNumber = 0;
  bool lineOn = false;
  bool switchOn = false;
  bool batteryAdd = false;
  bool item = false;
  int selectedItem = -1;
  bool isPressing = false;

  vector<Line> lines;
  // DraggableElement::font.loadFromFile("textures/notosans.ttf");
  if (!DraggableElement::font.loadFromFile("notosans.ttf")) {
    throw runtime_error("Failed to load font");
  }

  RenderWindow window(VideoMode(1366, 768), "Simulation",
                      Style::Close | Style::Resize);
  initializeGrid(numRows, cellSize, numCols, grid);

  ImGui::SFML::Init(window);

  ImGuiIO& io = ImGui::GetIO();
  ImFont* customFont =
      io.Fonts->AddFontFromFileTTF("notosans.ttf", 18.0f);

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
      Capacitor::getImagePath(),
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
        isPressing = true;
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
              lines.push_back(Line(mousePosition, mousePosition));
            }

            if (lineOn) {
              lines.push_back(Line(mousePosition, mousePosition));
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
        isPressing = false;
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

    for (auto& component : components) {
      component->TurnOn(switchOn);
    }
    if (ImGui::Button("Draw Line")) {
      lineOn = !lineOn;
    }
    if (ImGui::Button("Calculate")) {
      float totalResistance = 0.0f, totalVoltage = 0.0f;
      float voltageDrop;
      vector<float> tVD;
      vector<Resistor> resistors;
      Multimeter* multi;
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
       for (auto& component : components) {
          Multimeter* MultimeterPtr = dynamic_cast<Multimeter*>(component.get());
           if(MultimeterPtr){
            MultimeterPtr->clearVoltageTexts();
            MultimeterPtr->checkMulti = true;
            multi = MultimeterPtr;
            MultimeterPtr->setValue(totalVoltage / totalResistance);
          }
       }
      for (auto& resistor : resistors) {
        float voltageDrop =
            resistor.getVolatageDrop(totalResistance, totalVoltage);

            multi->setVoltageDrop(resistor.id_resistor,voltageDrop);
      }
      resistors.clear();
      
    }
    ImGui::EndChild();

    ImGui::SameLine();


    // Right content area with grid rendering
    ImGui::BeginChild("Content", ImVec2(0, 0), true, ImGuiWindowFlags_NoResize);
    ImGui::Text("Simulation Matricelle");
    ImGui::Spacing(); // Add spacing between the title and the content area

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
Mouse::isButtonPressed(Mouse::Left) && !component->isDragging && !isPressing) {
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