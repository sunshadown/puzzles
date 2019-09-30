#ifndef PAINT_H
#define PAINT_H

#include "Tile.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>
#include <mutex>
#include <time.h>
//MATH LIBRARY
#include "../lib/includes/cglm/cglm.h"
#include "../lib/includes/cglm/vec3.h"
#include "../lib/includes/cglm/vec4.h"
#include "../lib/includes/cglm/mat4.h"
#include "../lib/includes/cglm/affine.h"
#include "../lib/includes/cglm/common.h"
#include "../lib/includes/cglm/types.h"

class Paint
{
private:
  static const size_t window_x = 800, window_y = 600;
  static const size_t tile_x = 100, tile_y = 100;
  size_t picked_puzzles,puzzle_one, puzzle_two;
  void CreateTiles(std::string path);

public:
  Paint();
  ~Paint();

  vec3 color;
  bool flag_clear;
  bool flag_draw;
  bool can_draw;

  static void TileFocusTimer(std::vector<Tile> *tiles, float dt);
  static void TileScore(std::vector<Tile> *tiles, float *score);

  std::vector<Tile>tiles;
  static size_t GetTileIndex(size_t x, size_t y);
  void CheckTile(size_t x, size_t y);
  void CheckTileFocus(size_t x, size_t y);
  void CheckPuzzle(size_t num);
  void TileShuffle();
  void Render(mat4 ortho_matrix);
  void ClearAll();
};


#endif
