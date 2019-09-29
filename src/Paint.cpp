#include "Paint.h"


Paint::Paint()
{
  CreateTiles();
  glm_vec3_zero(color);
  flag_clear = false;
  flag_draw = false;
  can_draw = false;
}

Paint::~Paint()
{

}

void Paint::CreateTiles()
{
  size_t num_x = window_x / tile_x;
  size_t num_y = window_y / tile_y;

  Tile *temp_tile = new Tile();
  temp_tile->LoadImage("./assets/images/wallpaper.png");
  //temp_tile->InitBuffer();
  temp_tile->LoadPuzzleShader();

  float offset = 1.0f/num_x;
  float uv_x = 0.0f, uv_y = 0.0f;

  for (size_t i = 0; i < num_y; i++) {
    for (size_t j = 0; j < num_x; j++) {
      Tile tile(temp_tile);
      tile.InitBuffer();
      tile.LoadPuzzle(offset, uv_x, uv_y);
      //std::cout << i << " " << j << ", tile cord: " << uv_x << " " << uv_y << std::endl;

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
      //tile.SetRotate(180.0f);
      tiles.push_back(tile);
      uv_x += offset;
    }
    uv_y += offset;
    uv_x = 0.0f;
  }
}

void Paint::CheckTile(size_t x, size_t y)
{
  size_t num_x = window_x / tile_x;
  size_t num_y = window_y / tile_y;

  size_t npos_x, npos_y;


  npos_x = x % tile_x;
  npos_y = y % tile_y;
  npos_x = (x - npos_x)/tile_x;
  npos_y = (y - npos_y)/tile_y;

  size_t num = npos_x + npos_y*num_x;

  vec3 ncolor;
  if(flag_clear)
  {
    flag_draw = !flag_clear;
    glm_vec3_one(ncolor);
    if(can_draw)tiles.at(num).SetColor(ncolor);
    return;
  }
  if(flag_draw)
  {
    flag_clear = !flag_draw;
    glm_vec3_copy(color, ncolor);
    if(can_draw)tiles.at(num).SetColor(ncolor);
    return;
  }
}

void Paint::Render(mat4 ortho_matrix)
{
  for (size_t i = 0; i < tiles.size(); i++) {
    tiles.at(i).Render(ortho_matrix);
  }
}

void Paint::ClearAll()
{
  for (size_t i = 0; i < tiles.size(); i++) {
    vec3 ncolor;
    glm_vec3_one(ncolor);
    tiles.at(i).SetColor(ncolor);
  }
}
