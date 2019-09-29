#include "Tile.h"


Tile::Tile()
{
  Init();
}

Tile::Tile(Tile *tile)
{
  Copy(tile);
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
  haveImage = false;
  choosen = 0;

  //InitBuffer();
  //InitShader();
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

void Tile::InitPuzzleBuffer(float tile_size, float uv_x, float uv_y)
{
  float offset = tile_size;
  std::vector<float> data;
  //Left Triangle
  data.push_back(uv_x);data.push_back(uv_y - offset);
  data.push_back(uv_x + offset);data.push_back(uv_y);
  data.push_back(uv_x);data.push_back(uv_y);
  //Right Triangle
  data.push_back(uv_x);data.push_back(uv_y - offset);
  data.push_back(uv_x + offset);data.push_back(uv_y - offset);
  data.push_back(uv_x + offset);data.push_back(uv_y);

  GLuint puzzle_vbo;
  glGenBuffers(1, &puzzle_vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, puzzle_vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data.size(), &data[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  elements_size = data.size();

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

void Tile::InitPuzzleShader()
{
  char* vs = shader.LoadShader("./assets/shaders/puzzle.vs", 6000);
  char* fs = shader.LoadShader("./assets/shaders/puzzle.fs", 6000);
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

  if(haveImage)
  {
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex_id);
    glUniform1i(shader.GetLocation("fixed_uniform"), 0);
    glUniform1i(shader.GetLocation("choosen"), choosen);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }

  glUniformMatrix4fv(shader.GetLocation("projection"), 1, GL_FALSE, ortho_matrix[0]);
  glUniformMatrix4fv(shader.GetLocation("model"), 1, GL_FALSE, model_matrix[0]);
  glUniform1f(shader.GetLocation("m_depth"), m_depth);
  glUniform3f(shader.GetLocation("m_color"), color[0],color[1],color[2]);



  glDrawArrays(GL_TRIANGLES, 0, elements_size);

  glDisable(GL_BLEND);
  glBindVertexArray(0);
}

void Tile::LoadImage(std::string path)
{
  std::vector<unsigned char> image; //the raw pixels
  unsigned width, height;

  //decode
  unsigned error = lodepng::decode(image, width, height, path.c_str());

  //if there's an error, display it
  if(error)
  {
    std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    return;
  }
  //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
  haveImage = true;

  glGenTextures(1,&tex_id);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,tex_id);

  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,width,height,0, GL_RGBA, GL_UNSIGNED_BYTE,&image[0]);
}

void Tile::Copy(Tile *tile)
{
  //this->position = tile->position;

  glm_vec3_copy(tile->position,position);
  glm_vec3_copy(tile->size,size);
  glm_vec3_copy(tile->scale,scale);
  glm_vec3_copy(tile->color,color);
  this->rotate = tile->rotate;
  this->m_depth = tile->m_depth;
  this->shader = tile->shader;
  this->haveImage = tile->haveImage;
  this->vao = tile->vao;
  this->tex_id = tile->tex_id;
  this->elements_size = tile->elements_size;
  this->choosen = tile->choosen;
}

void Tile::LoadPaintShader()
{
  InitShader();
}

void Tile::LoadPuzzleShader()
{
  InitPuzzleShader();
}

void Tile::LoadPuzzle(float tile_size, float uv_x, float uv_y)
{
  InitPuzzleBuffer(tile_size, uv_x, uv_y);
}

void Tile::SetVao(GLuint vao)
{
  this->vao = vao;
}

void Tile::SetTexture(GLuint tex_id)
{
  this->tex_id = tex_id;
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

void Tile::SetRotate(float rotate)
{
  this->rotate = rotate;
}

int Tile::GetChoosen()
{
  return choosen;
}

void Tile::SetChoosen(int choosen)
{
  this->choosen = choosen;
}


void Tile::SetID(int id)
{
  this->id = id;
}

int Tile::GetID()
{
  return id;
}
