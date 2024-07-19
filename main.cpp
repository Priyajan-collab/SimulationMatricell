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


const int numRows = 30;
const int numCols = 40;
const float cellSize = 20.0f;

struct Cell {
    RectangleShape shape;

    Cell() {
        shape.setSize(Vector2f(cellSize, cellSize));
        shape.setFillColor(Color::White);
        shape.setOutlineThickness(1.0f);
        shape.setOutlineColor(Color::Black);
    }
};


vector<vector<Cell>> grid(numRows, vector<Cell>(numCols));

class Draggable {
protected:
    bool isDragging;
    Vector2f dragOffset;

public:

    RectangleShape shape;

    Draggable(int width, int height, float x, float y)
        : isDragging(false) {
        shape.setSize(Vector2f(width, height));
        shape.setPosition(x, y);
    }

    void draw(RenderWindow& window) {
        window.draw(shape);
    }

    Vector2f getPosition() {
        return shape.getPosition();
    }

    void startDragging(Vector2f mousePosition) {
        isDragging = true;
        dragOffset = shape.getPosition() - mousePosition;
    }

    void stopDragging() {
        isDragging = false;
    }

    void setColor(const Color& color) {
        shape.setFillColor(color);
    }

    void updatePosition(Vector2f mousePosition) {
        if (isDragging) {
            shape.setPosition(mousePosition + dragOffset);
        }
    }
};

class CircuitElement {
public:
    bool is_connected = false;

    virtual int connect() const = 0;
};

class Battery : public CircuitElement, public CircleShape {
public:
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


void circuitConnection(bool switchToggle) {
    int a, b, c;
    unique_ptr<Battery[]> battery(new Battery[1]);
    unique_ptr<Load[]> load(new Load[1]);
    unique_ptr<Wire[]> wire(new Wire[2]);
    unique_ptr<Switch[]> switch_toggle(new Switch[1]);

    a = wire[0].connect() - battery[0].connect();
    switch_toggle[0].toggle(a, switchToggle);
    load[0].connect();
    b = wire[1].connectWith(load[0]);
    c = battery[0].connect() - b;

    if (c == 1) {
        cout << "The circuit is on" << endl;
    }
    else {
        cout << "The circuit is off" << endl;
    }
}

void handleHover(RenderWindow& window, int& row, int& col) {
    Vector2i mousePos = Mouse::getPosition(window);
    col = mousePos.x / cellSize;
    row = mousePos.y / cellSize;
    cout << "Col: " << col << " Row: " << row << endl;

}

void initializeGrid() {
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            grid[row][col].shape.setPosition(col * cellSize, row * cellSize);
        }
    }
}

void drawGrid(RenderWindow& window) {

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            window.draw(grid[row][col].shape);
        }
    }
}

class Button {
public:
    Button() {} // Default constructor

    Button(string text, Vector2f size, int charSize, Color bgColor, Color textColor) {
        this->text.setString(text);
        this->text.setFillColor(textColor);
        this->text.setCharacterSize(charSize);

        buttonShape.setSize(size);
        buttonShape.setFillColor(bgColor);
    }

    void setFont(Font& font) {
        text.setFont(font);
    }

    void setBackColor(Color color) {
        buttonShape.setFillColor(color);
    }

    void setTextColor(Color color) {
        text.setFillColor(color);
    }

    void setPosition(Vector2f pos) {
       buttonShape.setPosition(pos);
        // Center text within the button
        float xPos = pos.x + (buttonShape.getSize().x - text.getLocalBounds().width) / 2;
        float yPos = pos.y + (buttonShape.getSize().y - text.getLocalBounds().height) / 2 - text.getCharacterSize() / 4;
        text.setPosition(xPos, yPos);
    }

    void drawTo(RenderWindow& window) {
        window.draw(buttonShape);
        window.draw(text);
    }

    bool isMouseOver(RenderWindow& window) {
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        FloatRect bounds = buttonShape.getGlobalBounds();
        return bounds.contains(mousePos);
    }

    bool isMouseClicked(RenderWindow& window) {
        if (isMouseOver(window) && Mouse::isButtonPressed(Mouse::Left)) {
            return true;
        }
        return false;
    }

private:
    RectangleShape buttonShape;
    Text text;
};

class MenuList {
private:
    bool isOpen; // Changed from int to bool to track open/close state
    vector<Texture> textures;
    vector<Sprite> sprites;
    vector<ImTextureID> textureIDs;
    int selectedItem; // Track selected item index

public:
    MenuList() : isOpen(false), selectedItem(-1) {} // Default constructor, menu is initially closed

    void setTextures(const vector<Texture>& texs) {
        textures = texs;

        sprites.clear();
        textureIDs.clear();

        for (const auto& texture : textures) {
            Sprite sprite;
            sprite.setTexture(texture);
            sprites.push_back(sprite);
            textureIDs.push_back(reinterpret_cast<void*>(sprites.back().getTexture()->getNativeHandle()));
        }
    }

    void drawMenu() {
        if (isOpen) {
            ImGui::Begin("Menu", &isOpen); // Pass the address of isOpen

            ImGui::Columns(2, nullptr, false); // 2 columns, auto-width, no border

            for (size_t i = 0; i < textureIDs.size(); ++i) {
                ImGui::Image(textureIDs[i], ImVec2(100, 50)); // Fixed size of 100x50

                if (ImGui::IsItemClicked()) {
                    selectedItem = i;
                }

                ImGui::NextColumn(); // Move to next column
            }

            ImGui::End();
        }
    }

    void toggle() {
        isOpen = !isOpen;
    }

    bool isOpened() const {
        return isOpen;
    }

    int getSelectedItemIndex() const {
        return selectedItem;
    }

    Sprite& getSelectedSprite() {
        if (selectedItem != -1) {
            return sprites[selectedItem];
        } else {
            // Returning a default sprite here if no sprite is loaded
            static Sprite defaultSprite;
            return defaultSprite;
        }
    }
};


class Components {
public:
    RectangleShape rect;
    Sprite spriterect;
    Texture texturerect;
    Vector2i MousePos;
    bool isDragging;

    Components() : isDragging(false) {}

    void setSpriteTexture(const Texture& texture) {
        spriterect.setTexture(texture);

        // Scale the sprite to match the rectangle shape
        FloatRect spriteBounds = spriterect.getLocalBounds();
        Vector2f scaleFactors(
            rect.getSize().x / spriteBounds.width,
            rect.getSize().y / spriteBounds.height
        );
        spriterect.setScale(scaleFactors);
        spriterect.setPosition(rect.getPosition());
    }

    void draw(RenderWindow& window) {
       // window.draw(rect); // Drawing the rectangle make the white background of the rec visible
        window.draw(spriterect);
    }

    void init(Vector2f size, Vector2f position) {
        // Initialize the rectangle shape
        rect.setSize(size);
        rect.setPosition(position);
    }

    void updatePosition(RenderWindow& window) {
        if (isDragging) {
            MousePos = Mouse::getPosition(window);
            rect.setPosition(static_cast<Vector2f>(MousePos));
            spriterect.setPosition(rect.getPosition());
        }
    }

    void startDragging() {
        isDragging = true;
    }

    void stopDragging() {
        isDragging = false;
    }

    bool isMouseOver(RenderWindow& window) {
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        FloatRect bounds = rect.getGlobalBounds();
        return bounds.contains(mousePos);
    }
};






int main() {
    int batteryNumber = 0;
    bool lineOn = false;
    bool switchOn = true;
    bool batteryAdd = false;

    Draggable Bulb(45, 20, 200, 200);
    vector<Battery> batteries;
    vector<Line> lines;

    RenderWindow window(VideoMode(numCols * cellSize, numRows * cellSize), "Simulation", Style::Close | Style::Resize);
    initializeGrid();

    ImGui::SFML::Init(window);

     
     //button ko
     Font font;
    if (!font.loadFromFile("ThisCafe.ttf")) {
        return -1;
    }

     Button btn1("Icons", {200, 50}, 20, Color::Green, Color::Black);
    btn1.setFont(font);
    btn1.setPosition({10, 10});

    MenuList menu;
    vector<Texture> textures;
    vector<string> imagePaths = {
        "textures/ResistorIcon.png",
        "textures/CapacitorIcon.png",
        "textures/InductorIcon.png",
         "textures/ResistorIcon.png",
        "textures/CapacitorIcon.png",
        "textures/InductorIcon.png",
        "textures/DiodeIcon.png"
    };

    // Load textures
    for (const auto& path : imagePaths) {
        Texture texture;
        if (texture.loadFromFile(path)) {
            textures.push_back(texture);
        } else {
            cout << "Error in loading image: " << path << endl;
        }
    }

    // Set textures for menu
    menu.setTextures(textures);

    Clock deltaClock;

    vector<Components> components;
    Components* selectedComponent = nullptr; // Pointer to track the selected component
    int lastSelectedItemIndex = -1; // Track the last selected item index
    bool isPlacingComponent = false; // Flag to indicate if placing a component

    while (window.isOpen()) {
        Event event;
        circuitConnection(switchOn);

        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == Event::Closed) {
                window.close();
            }
             if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                if (selectedComponent && selectedComponent->isMouseOver(window)) {
                    selectedComponent->startDragging();
                } else if (btn1.isMouseClicked(window)) {
                    menu.toggle();
                }
            }

             if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
                if (selectedComponent) {
                    selectedComponent->stopDragging();
                    isPlacingComponent = true; // Set to true when placing
                }
            }
            else if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
                    if (Bulb.shape.getGlobalBounds().contains(mousePosition)) {
                        Bulb.startDragging(mousePosition);
                    }

                    if (lineOn) {
                        lines.push_back(Line(mousePosition, mousePosition));
                    }

                    if (batteryAdd) {
                        batteries.push_back(Battery());
                        batteries.back().setRadius(20);
                        batteries.back().setFillColor(Color::Black);
                        batteries.back().setPosition(mousePosition);
                        batteryAdd = false;
                    }
                }
            }
            else if (event.type == Event::MouseButtonReleased) {
                 Bulb.stopDragging();
                if (event.mouseButton.button == Mouse::Left) {
                   
                    if (lineOn) {
                        lines.back().points[1].position = Vector2f(event.mouseButton.x, event.mouseButton.y);
                        lineOn = false;
                    }
                }
            }
        }

        if (Mouse::isButtonPressed(Mouse::Left)) {
            Vector2f mousePos(Mouse::getPosition(window));
            
            if (lineOn && !lines.empty()) {
                lines.back().points[1].position = mousePos;
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Checkbox("Switch", &switchOn);
        if (ImGui::Button("Add Battery")) {
            batteryAdd = true;
            batteryNumber++;
        }
        if (ImGui::Button("Draw Line")) {
            lineOn =!lineOn;
        }

        //button ko
          if (btn1.isMouseOver(window)) {
            btn1.setBackColor(Color::White);
        } else {
            btn1.setBackColor(Color::Green);
        }

        if (btn1.isMouseClicked(window)) {
            menu.toggle(); // Toggle menu visibility
        }


        window.clear(Color::Black);
        drawGrid(window);
        btn1.drawTo(window);
        menu.drawMenu();

        

        if (switchOn) {
            Bulb.setColor(Color::Red);
        }
        else {
            Bulb.setColor(Color::Black);
        }

        for (auto& battery : batteries) {
            window.draw(battery);
        }

        for (auto& line : lines) {
            window.draw(line.points, 2, Lines);
        }


         int selectedItemIndex = menu.getSelectedItemIndex();
        if (selectedItemIndex != -1) {
            if (selectedItemIndex != lastSelectedItemIndex) {//creates a new element if a new item is selected
                
                Components component;
                 component.init(Vector2f(100.f, 50.f), Vector2f(300.f, 150.f));//sets the intial size and postion
             
                component.setSpriteTexture(textures[selectedItemIndex]);
                components.push_back(component);
                selectedComponent = &components.back(); // Set as the  new selected component
                lastSelectedItemIndex = selectedItemIndex; // Update the last selected index
                isPlacingComponent = false; // Reset the flag
            }
        } else {
            // If no item is selected, delete the current component
            if (selectedComponent) {
                // Cleanup or reset the selected component if necessary
                selectedComponent = nullptr;
                lastSelectedItemIndex = -1; // Reset the last selected index
            }
        }


        for (auto& component : components) {
            if (&component == selectedComponent) {
                if (isPlacingComponent) {
                    component.stopDragging();
                    isPlacingComponent = false;
                }
                component.updatePosition(window);
            }
            component.draw(window);
        }


        ImGui::SFML::Render(window);
        int row = 0, col = 0;
        handleHover(window, row, col);
        Bulb.draw(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

