#include "grid.hpp"

// #include "Vector2.hpp"

void initializeGrid(int numRows, float cellSize, int numCols,
                    std::vector<std::vector<Cell>>& grid) {
  grid.clear();
  for (int row = 0; row < numRows; row++) {
    std::vector<Cell> rowCells;
    for (int col = 0; col < numCols; col++) {
      rowCells.emplace_back(col * cellSize, row * cellSize,
                            IM_COL32(255, 255, 255, 255));
    }
    grid.push_back(rowCells);
  }
}