#ifndef PAINT_H
#define PAINT_H

#include "Tile.h"
#include <vector>
#include <iostream>
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
  const size_t window_x = 800, window_y = 600;
  const size_t tile_x = 50, tile_y = 50;
  void CreateTiles();
public:
  Paint();
  ~Paint();

  vec3 color;
  bool flag_clear;

  std::vector<Tile>tiles;
  void CheckTile(size_t x, size_t y);
  void Render(mat4 ortho_matrix);
};


#endif
