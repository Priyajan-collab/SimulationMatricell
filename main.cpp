#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
using namespace sf;
using namespace std;
int s;

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
{	public:
	int operator - (CircuitElement ce) {
		return 1;
	}
};
class Load :public CircuitElement
{	protected:
	
	public:
	
	void connected(int x) {
		if (x)
		{
			is_connected= true;
		}
		
	}
	
	int operator - (CircuitElement ce) {
		return 1;
		}
	
};
class Wire :public CircuitElement
{	public:
	int operator - (CircuitElement ce) {
		return 1;
	}
	int operator + (CircuitElement ce ) {
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
	int & toggle(int &a, bool click) {
		if (click==false) {
			is_switch_on = false;
			a =0;
			return a;
		}
		else {
			return a;
		}
		
	
}


};

static void circuit_connection(bool switchToggle) {
	int a, b, c;
	Battery b1;
	Load l1;
	Wire w1,w2;
	Switch s1;
	a = w1 - b1; // wire is connected to the battery
	s1.toggle(a, switchToggle);
	l1.connected(a); // One end of load is connected to the wire
	b = w2 + l1; //wire is connected to other end of the load
	c=b1.connected(b);//the wire is connected to other end of battery
	if (c == 1) {
		cout << "The circuit is on" << endl;
	}
	else {
		cout << "circuit is  off" << endl;
	}
	
}




	int main()

	{

		bool switchOn = true;
		
		
		
		RenderWindow window( VideoMode(512, 512), "Simulation", Style::Close | Style::Resize);
		ImGui::SFML::Init(window);
		Clock deltaClock;
		while (window.isOpen()) {
			Event event;
				while (window.pollEvent(event)) {
					ImGui::SFML::ProcessEvent(event);
					circuit_connection(switchOn);
					if (event.type == Event::Closed) {
						window.close();
					}
				}
				
				ImGui::SFML::Update(window, deltaClock.restart());
				ImGui::Checkbox("switch", &switchOn);

				window.clear(Color::White);

				CircleShape Bulb;
				Bulb.setRadius(20);
				Bulb.setPosition(200, 200);
				if (switchOn) {

				Bulb.setFillColor(Color::Red);
				}
				else {
				Bulb.setFillColor(Color::Black);

				}
				
				ImGui::SFML::Render(window);
				window.draw(Bulb);

				window.display();
		}
		ImGui::SFML::Shutdown();
		return 0;
	}