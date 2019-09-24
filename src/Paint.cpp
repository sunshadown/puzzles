#include "Paint.h"


Paint::Paint()
{
  CreateTiles();
  glm_vec3_zero(color);
  flag_clear = false;
}

Paint::~Paint()
{

}

void Paint::CreateTiles()
{
  size_t num_x = window_x / tile_x;
  size_t num_y = window_y / tile_y;

  for (size_t i = 0; i < num_y; i++) {
    for (size_t j = 0; j < num_x; j++) {
      Tile tile;
      vec3 npos;
      vec3 nsize;
      glm_vec3_one(nsize);
      glm_vec3_zero(npos);
      npos[0] += tile_x * j;
      npos[1] += tile_y * i;
      nsize[0] *= tile_x;
      nsize[1] *= tile_y;
      tile.SetPosition(npos);
      tile.SetSize(nsize);
      tiles.push_back(tile);
    }
  }
}

void Paint::CheckTile(size_t x, size_t y)
{
  std::cout <<"mouse x,y: "<< x << " " << y <<std::endl;

  size_t num_x = window_x / tile_x;
  size_t num_y = window_y / tile_y;

  size_t npos_x, npos_y;


  npos_x = x % tile_x;
  npos_y = y % tile_y;
  npos_x = (x - npos_x)/tile_x;
  npos_y = (y - npos_y)/tile_y;
  std::cout << "npos: " <<npos_x << " " << npos_y <<std::endl;
  size_t num = npos_x + npos_y*num_x;

  vec3 ncolor;
  if(!flag_clear)glm_vec3_zero(ncolor);
  else glm_vec3_one(ncolor);
  tiles.at(num).SetColor(ncolor);

}

void Paint::Render(mat4 ortho_matrix)
{
  for (size_t i = 0; i < tiles.size(); i++) {
    tiles.at(i).Render(ortho_matrix);
  }
}
