#pragma once

#include <SFML/Graphics/CircleShape.hpp>


class CircuitElement {
public:
	bool is_connected = false;
	int connected(int x);
};


