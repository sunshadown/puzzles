#include "Tile.h"


Tile::Tile()
{
  Init();
}

Tile::~Tile()
{

}

void Tile::Init()
{
  glm_mat4_identity(model_matrix);
  glm_vec3_zero(position);
  glm_vec3_one(size);
  glm_vec3_one(scale);
  glm_vec3_one(color);
  rotate = 0.0f;
  m_depth = 0.0f;

  InitBuffer();
  InitShader();
}

void Tile::InitBuffer()
{
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  float marray[] = {
                // Pos      // Tex
                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,

                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f
        };

  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24, marray, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  elements_size = 6;

  glBindVertexArray(0);
}

void Tile::InitShader()
{
  char* vs = shader.LoadShader("./assets/shaders/fixed.vs", 6000);
  char* fs = shader.LoadShader("./assets/shaders/fixed.fs", 6000);
  shader.AttachShader(vs, 0);
  shader.AttachShader(fs, 1);
  shader.LinkShader();
  free(vs);
  free(fs);
}

void Tile::Update(float dt)
{

}

void Tile::Render(mat4 ortho_matrix)
{
  glm_mat4_identity(model_matrix);
  glm_translate(model_matrix, position);
  vec3 npos;
  npos[0] = size[0]*0.5f;
  npos[1] = size[1]*0.5f;
  npos[2] = 0.0f;
  vec3 pivot;
  pivot[0] = 0;
  pivot[1] = 0;
  pivot[2] = 1;

  glm_translate(model_matrix, npos);
  float radians = (rotate) *(M_PI/180.0f);
  glm_rotate(model_matrix, radians, pivot);

  npos[0] = size[0]*(-0.5f);
  npos[1] = size[1]*(-0.5f);
  glm_translate(model_matrix, npos);
  glm_scale(model_matrix, size);

  glBindVertexArray(vao);
  shader.Bind();

  //glEnable(GL_TEXTURE_2D);
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D,model->tex_uniform->uniform_id);

  //glUniform1i(glGetUniformLocation(model->program_id,model->tex_uniform->uniform_name),0);
  glUniformMatrix4fv(shader.GetLocation("projection"), 1, GL_FALSE, ortho_matrix[0]);
  glUniformMatrix4fv(shader.GetLocation("model"), 1, GL_FALSE, model_matrix[0]);
  glUniform1f(shader.GetLocation("m_depth"), m_depth);
  glUniform3f(shader.GetLocation("m_color"), color[0],color[1],color[2]);

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glDrawArrays(GL_TRIANGLES, 0, elements_size);

  glDisable(GL_BLEND);
  glBindVertexArray(0);
}

void Tile::SetPosition(vec3 position)
{
  glm_vec3_copy(position, this->position);
}

void Tile::SetSize(vec3 size)
{
  glm_vec3_copy(size, this->size);
}

void Tile::SetColor(vec3 color)
{
  glm_vec3_copy(color, this->color);
}

float* Tile::GetPosition()
{
  return position;
}

float* Tile::GetSize()
{
  return size;
}

float* Tile::GetColor()
{
  return color;
}
