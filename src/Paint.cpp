#include "Paint.h"


Paint::Paint()
{
  CreateTiles("./assets/images/p1.png");
  glm_vec3_zero(color);
  flag_clear = false;
  flag_draw = false;
  can_draw = false;
  picked_puzzles = 0;
}

Paint::~Paint()
{

}

void Paint::CreateTiles(std::string path)
{
  size_t num_x = window_x / tile_x;
  size_t num_y = window_y / tile_y;

  Tile *temp_tile = new Tile();
  tiles.clear();
  temp_tile->LoadImage(path.c_str());
  //temp_tile->InitBuffer();
  temp_tile->LoadPuzzleShader();

  float offset = 1.0f/num_x;
  float uv_x = 0.0f, uv_y = 1.0f - offset;
  int id = 0;
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
      tile.SetID(id);
      tiles.push_back(tile);
      id++;
      uv_x += offset;
    }
    uv_y -= offset;
    uv_x = 0.0f;
  }
}

void Paint::TileFocusTimer(std::vector<Tile> *tiles, float dt, GLFWwindow* window)
{
  //struct timespec timespec;
  //timespec.tv_nsec = 200;
  std::mutex m_mutex;
  while(1)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (size_t i = 0; i < tiles->size(); i++) {
      if(tiles->at(i).GetFocused() >= 1)
      {
        double time = glfwGetTime();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        ypos = 600 - ypos;
        size_t num = GetTileIndex(xpos,ypos);
        float* tile_pos = tiles->at(i).GetPosition();
        size_t num_tile = GetTileIndex(tile_pos[0], tile_pos[1]);
        if(num == num_tile)continue;
        if(time - tiles->at(i).timestamp >= dt)
        {
          tiles->at(i).SetFocused(0);
          tiles->at(i).timestamp = 0.0f;
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void Paint::TileScore(std::vector<Tile> *tiles, float *score)
{
  while(1)
  {
    *score = 0.0f;
    for (size_t i = 0; i < tiles->size(); i++) {
      float* npos = tiles->at(i).GetPosition();
      size_t num = GetTileIndex(npos[0] + 5.0f, npos[1] + 5.0f);
      if(num == tiles->at(i).GetID())*score += 1.0f;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

size_t Paint::GetTileIndex(size_t x, size_t y)
{
  size_t num_x = window_x / tile_x;
  size_t num_y = window_y / tile_y;

  size_t npos_x, npos_y;


  npos_x = x % tile_x;
  npos_y = y % tile_y;
  npos_x = (x - npos_x)/tile_x;
  npos_y = (y - npos_y)/tile_y;

  //std::cout << "x: " << npos_x << "y: " << npos_y <<std::endl;

  size_t num = npos_x + npos_y*num_x;
  return num;
}

void Paint::CheckTile(size_t x, size_t y)
{
  size_t num = GetTileIndex(x,y);
  CheckPuzzle(num);

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

void Paint::CheckTileFocus(size_t x, size_t y)
{
  size_t num = GetTileIndex(x,y);
  tiles.at(num).timestamp = glfwGetTime();
  tiles.at(num).SetFocused(1);
}

void Paint::CheckPuzzle(size_t num)
{
  tiles.at(num).SetChoosen(1);
  picked_puzzles++;
  if(picked_puzzles == 1)puzzle_one = num;
  else puzzle_two = num;

  if(picked_puzzles >= 2)
  {
    //swap
    std::cout << "swaping "<< puzzle_one << " " << puzzle_two << std::endl;
    vec3 temp_pos1;
    vec3 temp_pos2;
    glm_vec3_copy(tiles[puzzle_one].GetPosition(), temp_pos1);
    glm_vec3_copy(tiles[puzzle_two].GetPosition(), temp_pos2);
    std::cout << temp_pos1[0] << " " << temp_pos1[1] << std::endl;
    std::cout << temp_pos2[0] << " " << temp_pos2[1] << std::endl;
    tiles[puzzle_one].SetPosition(temp_pos2);
    tiles[puzzle_two].SetPosition(temp_pos1);
    std::swap(tiles[puzzle_one], tiles[puzzle_two]);
    picked_puzzles = 0;
    tiles[puzzle_one].SetChoosen(0);
    tiles[puzzle_two].SetChoosen(0);
    return;
  }

  std::cout << "num " << num << std::endl;
}

void Paint::TileShuffle()
{
  // using built-in random generator:
  std::random_shuffle ( tiles.begin(), tiles.end() );
  size_t num_x = window_x / tile_x;
  float pos_y = 0.0f;

  for (size_t i = 0, j = 0; i < tiles.size(); i++,j++) {
    float* npos = tiles.at(i).GetPosition();
    if(j >= num_x)
    {
      pos_y += tile_y;
      j = 0;
    }
    npos[0] = j * tile_x;
    npos[1] = pos_y;
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
