#ifndef GRID_HPP
#define GRID_HPP
#pragma once
#include <imgui.h>

#include <SFML/System/Vector2.hpp>
#include <vector>
struct Cell {
  sf::Vector2f position;
  ImU32 color;

  Cell(float x, float y, ImU32 c) : position(x, y), color(c) {}
};
void initializeGrid(int, float, int, std::vector<std::vector<Cell>>& grid);
#endif