#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
using namespace sf;
using namespace std;
 int s = 0;


class Draggable {
protected:
	bool isDragging;
	Vector2f dragOffset;

public:
	CircleShape shape;

	Draggable(int radius, float x, float y)
		: isDragging(false) {
		shape.setRadius(radius);
		shape.setPosition(x, y);
	}

	virtual void draw(RenderWindow& window) {
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
	virtual void updatePosition(Vector2f mousePosition) {
		if (isDragging) {
			shape.setPosition(mousePosition + dragOffset);
		}
	}
};

class CircuitElement {
public:
	bool is_connected = false;
	int connected(int x) {
		if (x)
		{
			return 1;
		}
		else {
			return 0;
		};
	}
};

class Battery :public CircuitElement
{
public:
	int operator - (CircuitElement ce) {
		return 1;
	}
};
class Load :public CircuitElement
{
protected:

public:

	void connected(int x) {
		if (x)
		{
			is_connected = true;
		}

	}

	int operator - (CircuitElement ce) {
		return 1;
	}

};
class Wire :public CircuitElement
{
public:
	int operator - (CircuitElement ce) {
		return 1;
	}
	int operator + (CircuitElement ce) {
		if (ce.is_connected) {
			return 1;
		}
		else {
			return 0;
		}
	}
};
class Switch :public CircuitElement
{
public:
	bool is_switch_on = true;
	int& toggle(int& a, bool click) {
		if (click == false) {
			is_switch_on = false;
			a = 0;
			return a;
		}
		else {
			return a;
		}


	}
};

static void circuit_connection(bool switchToggle) {
	int a, b, c,nb=1,nl=1,nw=2,ns=1;
	Battery* battery;
	Load* load;
	Wire* wire;
	Switch* switch_toggle;
	switch_toggle = new Switch[ns];
	wire = new Wire[nw];
	load = new Load[nl];
	battery = new Battery[nb];
	//Battery b1;
	//Load l1;
	//Wire w1, w2;
	//Switch s1;
	a = (*wire) - *battery; // wire is connected to the battery
	(*switch_toggle).toggle(a, switchToggle);
	(*load).connected(a); // One end of load is connected to the wire
	b = (*(wire+1)) + *load; //wire is connected to other end of the load
	c = (*battery).connected(b);//the wire is connected to other end of battery
	if (c == 1) {
		//cout << "The circuit is on" << endl;
	}
	else {
		//cout << "circuit is  off" << endl;
	}

}

int main()

{
	bool lineOn = false;
	bool once = true;
	bool switchOn = true;
	bool batteryadd = false;
	Draggable Bulb(20, 200, 200);

	RenderWindow window(VideoMode(512, 512), "Simulation", Style::Close | Style::Resize);
	//to draw a line
	Vertex line[2];
	line[0].position = Vector2f(0, 0);
	Vector2f& refposition1 = line[0].position;
	line[1].position = Vector2f(0, 0);
	Vector2f& refposition2 = line[1].position;
	//Vector2f& initialPositionOfLine = line[0].position;
	//Vector2f& finalPositionOfLine = line[1].position;
	line[0].color = Color::Black;
	line[1].color = Color::Black;
	//creating logic to add multiple batteries using <vector>
	

	Clock deltaClock;
	while (window.isOpen()) {
		Event event;
		circuit_connection(switchOn);
		

		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == Event::Closed) {
				window.close();
			}
			else if (event.type == Event::MouseButtonPressed) {
				if (event.mouseButton.button == Mouse::Left) {
					Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
					// for (auto& resistor : resistors) {
					if (Bulb.shape.getGlobalBounds().contains(mousePosition)) {
						Bulb.startDragging(mousePosition);
						// }
					}
					else if (event.type == Event::MouseButtonPressed) {
						if (event.mouseButton.button == Mouse::Left) {
							Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
							// for (auto& resistor : resistors) {
								if (Bulb.shape.getGlobalBounds().contains(mousePosition)) {
									Bulb.startDragging(mousePosition);
								// }
							}
						}    
					}
					else if (event.type == Event::MouseButtonReleased) {
						if (event.mouseButton.button == Mouse::Left) {
							// for (auto& resistor : resistors) {
								Bulb.stopDragging();
							// }
						}
					}
				}
				//to draw line
				

				if (event.mouseButton.button == Mouse::Left && lineOn) {
					Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
					//gets initial point to draw line
					Vector2f bulbPos(Bulb.getPosition().x, Bulb.getPosition().y);
					refposition1 =
						mousePosition;
				}
				if (event.mouseButton.button == Mouse::Left && batteryadd) {
					
				
					for (int i = 0; i < s; i++) {

						c.push_back(CircleShape());
						c.back().setRadius(20);
						c.back().setFillColor(Color::Black);
				}
						c.back().setPosition(event.mouseButton.x, event.mouseButton.y);
						

				}
				
				
				
			}
			else if (event.type == Event::MouseButtonReleased) {
				if (event.mouseButton.button == Mouse::Left) {
					// for (auto& resistor : resistors) {
					Bulb.stopDragging();
					// }
				}
				if (event.mouseButton.button == Mouse::Left && lineOn) {
					//to get final position for the line
					Vector2f mousePosition(event.mouseButton.x, event.mouseButton.y);
					refposition2 = mousePosition;
					

					
				}
			}
			
		}
		if (Mouse::isButtonPressed(Mouse::Left)) {
			Vector2f mousePos(Mouse::getPosition(window));
			// for (auto& resistor : resistors) {
			Bulb.updatePosition(mousePos);
			// }
		}

		ImGui::SFML::Update(window, deltaClock.restart());
		ImGui::Checkbox("switch", &switchOn);
		if (ImGui::Button("Battery")) {
			cout << "added battery"<<endl;
			batteryadd = !batteryadd;
			s++;
			cout << s << endl;
		}
		if (ImGui::Button("Line"))
		{
			lineOn = !lineOn;
			cout << lineOn;
		}

		window.clear(Color::White);


		// Bulb.setRadius(20);
		// Bulb.setPosition(200, 200);
		if (switchOn) {

			Bulb.setColor(Color::Red);
		}
		else {
			Bulb.setColor(Color::Black);

		}
		for (int i = 0; i < c.size(); i++) {
			window.draw(c[i]);
		}
		window.draw(line,2,Lines);
		ImGui::SFML::Render(window);
		Bulb.draw(window);
		window.display();
	}
	ImGui::SFML::Shutdown();
	return 0;
}