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

#include "Shader.h"

class Tile
{
private:
  GLuint vao, vbo, ebo;
  uint32_t elements_size;
  mat4 model_matrix;
  vec3 position;
  vec3 size;
  vec3 scale;
  vec3 color;
  float rotate;
  float m_depth;
  Shader shader;

  void Init();
  void InitBuffer();
  void InitShader();
public:
  Tile();
  ~Tile();
  void Update(float dt);
  void Render(mat4 ortho_matrix);
  void SetPosition(vec3 position);
  float* GetPosition();
  void SetSize(vec3 size);
  float* GetSize();
  void SetColor(vec3 color);
  float* GetColor();
};


#endif
