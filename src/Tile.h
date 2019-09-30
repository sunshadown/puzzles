#ifndef TILE_H
#define TILE_H

//OpenGl wrangler
#include "../lib/includes/GL/glew.h"
//MATH LIBRARY
#include "../lib/includes/cglm/cglm.h"
#include "../lib/includes/cglm/vec3.h"
#include "../lib/includes/cglm/vec4.h"
#include "../lib/includes/cglm/mat4.h"
#include "../lib/includes/cglm/affine.h"
#include "../lib/includes/cglm/common.h"
#include "../lib/includes/cglm/types.h"
//IMAGELOADER
#ifndef LODE
#define LODE
#include "../lib/lodepng.h"
#endif
///////////////////////////
#include <iostream>
#include <vector>
///////////////////////////
#include <string>
#include "Shader.h"


class Tile
{
private:
  GLuint vao, vbo, ebo, tex_id;
  uint32_t elements_size;
  mat4 model_matrix;
  vec3 position;
  vec3 size;
  vec3 scale;
  vec3 color;
  float rotate;
  float m_depth;
  Shader shader;
  bool haveImage;
  int choosen;
  int focused;
  int id;

  void Init();
  void InitPuzzleBuffer(float tile_size, float uv_x, float uv_y);
  void InitShader();
  void InitPuzzleShader();
public:
  Tile();
  Tile(Tile *tile);
  ~Tile();

  double timestamp;

  void Update(float dt);
  void Render(mat4 ortho_matrix);

  void LoadImage(std::string path);

  void Copy(Tile *tile);
  void LoadPaintShader();
  void LoadPuzzleShader();
  void InitBuffer();
  void LoadPuzzle(float tile_size, float uv_x, float uv_y);

  void SetVao(GLuint vao);
  GLuint GetTexture();
  void SetTexture(GLuint tex_id);
  void SetPosition(vec3 position);
  float* GetPosition();
  void SetSize(vec3 size);
  float* GetSize();
  void SetColor(vec3 color);
  float* GetColor();
  void SetRotate(float rotate);
  int GetChoosen();
  void SetChoosen(int choosen);
  int GetFocused();
  void SetFocused(int focused);
  void SetID(int id);
  int GetID();
};


#endif
