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


class battery {
public:
	battery() {
		float volatage = 1;
	}
	int connect() {

		return 1;
	}
	int turnOn(int closecircuit) {
		if (closecircuit) {
			
			return 1;
		}
		else {
			return 0;
		}
	}
};

class wire {
public:
	int connectToWire(int pointA) {

		if (pointA == 1) {

			return 1;

		}
		else {
			return 0;
		};

	};
	
};
class bulb
{
public:
	int connect(int connected) {
		if (connected) {

			return 1;
		}
		else {
			return 0;
		}

	}




};

class switchOnOff {
public:
	int push(int a) {
		return a;
	};
};

void circuit(bool switchOn) {
	battery b1;
	bulb bulb1;
	wire wire1, wire2;
	switchOnOff s1;
	//this is the first terminal for the battery or bulb
	int a = wire1.connectToWire(b1.connect());
	//cout << "Battery is connected to wire1 :" << a << endl;
	//bulb is finally connected to the wire
	int b = bulb1.connect(a);
	//cout << "the bulb is connected to the wire :" << b << endl;
	//adding switch to the circuit
	if (switchOn) {
		 s = s1.push(1);
	}
	else
	{
		 s = s1.push(0);
	}
	//this is another wire from the bulb 
	int c = wire2.connectToWire(s);
	//finally the circuit is going to be complete
	int d = b1.turnOn(c);
	if (d) {
		cout << "the bulb has turned on" << endl;
	}
	else {
		cout << "bulb is off"<<endl;
	}
	//lot of unnecssary code uff switch lai ni bool lekdai xu 

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
					if (event.type == Event::Closed) {
						window.close();
					}
				}
				circuit(switchOn);
				ImGui::SFML::Update(window, deltaClock.restart());
				ImGui::Checkbox("switch", &switchOn);

				window.clear(Color::White);

				CircleShape bulb;
				bulb.setRadius(20);
				bulb.setPosition(200, 200);
				if (switchOn) {

				bulb.setFillColor(Color::Red);
				}
				else {
				bulb.setFillColor(Color::Black);

				}
				
				ImGui::SFML::Render(window);
				window.draw(bulb);

				window.display();
		}
		ImGui::SFML::Shutdown();
		return 0;
	}