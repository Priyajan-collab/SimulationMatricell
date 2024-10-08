#include <iostream>
#include <vector>
#include <memory>
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

using namespace sf;
using namespace std;

const int numRows = 50;
const int numCols = 70;
const float cellSize = 20.0f;
int col = 0;
int row = 0;

struct Cell {
    Vector2f position;
    ImU32 color;

    Cell(float x, float y, ImU32 c)
        : position(x, y), color(c) {}
};

vector<vector<Cell>> grid;

void initializeGrid() {
    grid.clear();
    for (int row = 0; row < numRows; row++) {
        vector<Cell> rowCells;
        for (int col = 0; col < numCols; col++) {
            rowCells.emplace_back(col * cellSize, row * cellSize, IM_COL32(255, 255, 255, 255));
        }
        grid.push_back(rowCells);
    }
}

void drawGrid(ImDrawList* drawList, const ImVec2& offset) {
    for (const auto& rowCells : grid) {
        for (const auto& cell : rowCells) {
            drawList->AddRectFilled(ImVec2(cell.position.x + offset.x, cell.position.y + offset.y),
                                    ImVec2(cell.position.x + cellSize + offset.x, cell.position.y + cellSize + offset.y),
                                    cell.color);
            drawList->AddRect(ImVec2(cell.position.x + offset.x, cell.position.y + offset.y),
                              ImVec2(cell.position.x + cellSize + offset.x, cell.position.y + cellSize + offset.y),
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

    Draggable(float width, float height, float x, float y)
        : isDragging(false) {
        shape.setSize(Vector2f(width, height));
        shape.setPosition(x, y);
    }

    virtual void draw(RenderWindow& window) {
        window.draw(shape);
    }

    virtual bool contains(Vector2f mousePosition) const {
        return shape.getGlobalBounds().contains(mousePosition);
    }

    virtual void startDragging(Vector2f mousePosition) {
        isDragging = true;
        dragOffset = shape.getPosition() - mousePosition;
    }

    virtual void stopDragging() {
        isDragging = false;
    }

    void setColor(const Color& color) {
        shape.setFillColor(color);
    }

    virtual void updatePosition(Vector2f mousePosition) {
    Vector2f position;

    // Calculate grid cell coordinates
    int col = static_cast<int>((mousePosition.x + dragOffset.x - cellSize / 2.0f) / cellSize);
    int row = static_cast<int>((mousePosition.y + dragOffset.y - cellSize / 2.0f) / cellSize);

    if (row >= 0 && row < numRows && col >= 0 && col < numCols) {
        // Calculate position to place object at the center of the grid cell
        float centerX = col * cellSize + cellSize / 2.0f - shape.getSize().x / 2.0f;
        float centerY = row * cellSize + cellSize / 2.0f - shape.getSize().y / 2.0f;

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

class Component {
public:
    virtual ~Component() {}
    // virtual static const string& getImagePath() const = 0;
};


class Resistor : public Component, public ElementRender {
    static const string image;
    Texture resistorTexture;
    ImTextureID resistorTextureID;
    // ImTextureID batteryTextureID = (void*)(intptr_t)batteryTexture.getNativeHandle();
public:
    
    Resistor( ImVec2 pos, const std::string& uniqueID, float initialVar = 45.0f)
        : Component(), ElementRender( pos, uniqueID, initialVar) {
        
    }

    static const string& getImagePath() { return image; }
    
};

const string Resistor::image = "textures/ResistorIcon.png";


class Batt : public Component, public ElementRender {
public:
    static const string image;
    
    Batt( ImVec2 pos, const std::string& uniqueID, float initialVar = 45.0f)
        : Component(), ElementRender( pos, uniqueID, initialVar) {
        cout << "Battery is made" << endl;
    }

    static const string& getImagePath() { return image; }
};

const string Batt::image = "textures/CapacitorIcon.png";

class Inductor : public Component, public ElementRender {
public:
    static const string image;
    
    Inductor( ImVec2 pos, const std::string& uniqueID, float initialVar = 45.0f)
        : Component(), ElementRender( pos, uniqueID, initialVar) {
        cout << "Inductorery is made" << endl;
    }

    static const string& getImagePath() { return image; }
};

const string Inductor::image = "textures/InductorIcon.png";

class MenuList {
private:
    vector<Texture> textures;
    vector<ImTextureID> textureIDs;
    int selectedItem; // Track selected item index
    string components[6];
    bool itemPlaced;

public:
    MenuList() : selectedItem(-1), components{"Resistor", "Battery", "Inductor", "Capacitor", "Diode", "Transistor"} {
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

    int getSelectedComponent() const {
        return selectedItem;
    }

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

    Component* createComponent(const string& type, ImVec2 pos, const std::string& uniqueID, float initialVar) {
        if (type == "Resistor") {
            return new Resistor(pos, uniqueID, initialVar);
        } else if (type == "Battery") {
            return new Batt(pos, uniqueID, initialVar);
        }
        else if (type == "Inductor") {
            return new Inductor(pos, uniqueID, initialVar);
        }
        // Add more cases as needed
        return nullptr;
    }
    bool isItemPlaced() const {
        return itemPlaced;
    }

    void setItemPlaced(bool placed) {
        itemPlaced = placed;
    }
};


class CircuitElement {
public:
    bool is_connected = false;

    virtual int connect() const = 0;
};

// class Resistor{
//     static string image ;
//     public:
//         Resistor(){}

// };
// string Resistor::image = "textures/ResistorIcon.png";

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

    void stopDragging() override {
        isDragging = false;
    }

    void updatePosition(Vector2f mousePosition) override {
        Vector2f position;

        // Calculate grid cell coordinates
        int col = static_cast<int>((mousePosition.x + dragOffset.x - getRadius()) / cellSize);
        int row = static_cast<int>((mousePosition.y + dragOffset.y - getRadius()) / cellSize);

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

    void draw(RenderWindow& window) override {
        window.draw(*this);
    }

    int connect() const override {
        return 1;
    }
};

class Load : public CircuitElement {
public:
    void connect() {
        is_connected = true;
    }

    int connect() const override {
        return is_connected ? 1 : 0;
    }
};

class Wire : public CircuitElement {
public:
    int connect() const override {
        return 1;
    }

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

    int connect() const override {
        return is_switch_on ? 1 : 0;
    }
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
    vector<unique_ptr<Draggable>> components;
    components.emplace_back(make_unique<Draggable>(45, 20, 200, 200));

    RenderWindow window(VideoMode(1366, 768), "Simulation", Style::Close | Style::Resize);
    initializeGrid();

    ImGui::SFML::Init(window);

    Clock deltaClock;

        // Main setup
    MenuList menu;

    // Dynamically load textures from component classes
    vector<Texture> textures;
    vector<string> imagePaths = {
        Resistor::getImagePath(),
        Batt::getImagePath(),
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

   
    vector<unique_ptr<ElementRender>> elementRenders;

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
                        components.emplace_back(make_unique<Battery>(mousePosition.x, mousePosition.y, 20));
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
                        lines.back().points[1].position = Vector2f(event.mouseButton.x, event.mouseButton.y);
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
        window.clear(Color::Black);

        ImVec2 windowSize(window.getSize().x, window.getSize().y);

        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always); // Set ImGui window size to match SFML window size
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // Set ImGui window position to top-left corner

        ImGui::Begin("Main Layout", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

        // Left menu area
        ImGui::BeginChild("Menu", ImVec2(200, 0), true, ImGuiWindowFlags_NoResize); // Fixed width for the menu
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
        // If ImGui::IsMouseClicked(ImGuiMouseButton_Left) is true
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !menu.isItemPlaced()) {
            string selectedComponent = menu.getSelectedComponentName();
            ImTextureID textureID = menu.getSelectedTextureID();

            if (!selectedComponent.empty() && textureID) {
                auto component = menu.createComponent(selectedComponent, mousePos, selectedComponent,45.0f);
                if (component) {
                    auto elementRender = dynamic_cast<ElementRender*>(component);
                    if (elementRender) {
                        elementRender->setTexture(textureID);
                        elementRenders.push_back(unique_ptr<ElementRender>(elementRender));
                        menu.setItemPlaced(true);
                    }
                }
            }
        }

        
        // if (menu.getSelectedItemIndex() != -1) {
        //     ImTextureID texID = menu.getSelectedTextureID();
        //     if (texID != nullptr) { // Ensure the texture ID is valid
        //         bool alreadyExists = false;
        //         for (const auto& elem : elements) {
        //             if (elem.getTextureID() == texID) {
        //                 alreadyExists = true;
        //                 break;
        //             }
        //         }
        //         if (!alreadyExists) {
        //             elements.emplace_back(texID, defaultPosition, "Element_" + std::to_string(elementCounter++));
        //         }
        //     }
        // }

        // Draw each ElementRender
        for (auto& render : elementRenders) {
            render->drawElement();
        }
        
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
        ImGui::EndChild();
        ImGui::End();
        ImGui::SFML::Render(window);

        for (auto& component : components) {
            component->draw(window);
        }

        for (auto& line : lines) {
            window.draw(line.points, 2, Lines);
        }

        // Bulb.draw(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
