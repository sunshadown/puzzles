#include "Shader.h"


Shader::Shader()
{
  Init();
}

Shader::~Shader()
{

}


void Shader::Init()
{
  program_id = glCreateProgram();
}

void Shader::Bind()
{
  glUseProgram(program_id);
}

GLuint Shader::GetLocation(std::string name)
{
  return glGetUniformLocation(program_id, name.c_str());
}

char* Shader::LoadShader(std::string path, size_t buff_size)
{
  char* buff = (char*)malloc(buff_size);
  memset(buff,0,buff_size);
  buff[buff_size - 1] = '\0';
  int fd = open(path.c_str(),O_RDONLY);
  if(fd == -1)
  {
    printf("%s \n",strerror(errno));
    return NULL;
  }
  size_t size = read(fd,buff,buff_size);
  close(fd);
  if(size < 1)return NULL;

  return buff;
}

void Shader::AttachShader(char* shader, int flag)
{
  GLuint sh;
  if(flag == 0)sh = glCreateShader(GL_VERTEX_SHADER);
  else sh = glCreateShader(GL_FRAGMENT_SHADER);

  if(shader == 0)
  {
    printf("AttachShader failed, cant create shader");
    return;
  }
  const GLchar* shader_source = shader;
  glShaderSource(sh, 1, &shader_source, 0);
  glCompileShader(sh);
  GLint succes = 0;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &succes);
  if(succes == GL_FALSE)
  {
    GLint logSize = 0;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logSize);
    char buff[logSize];
    glGetShaderInfoLog(sh,logSize, &logSize, buff);
    printf("Shader Compilation error flag: %d %s \n", flag, buff);
    return;
  }
  glAttachShader(program_id, sh);
}

void Shader::LinkShader()
{
  glLinkProgram(program_id);
  GLint isLinked = 0;
  glGetProgramiv(program_id, GL_LINK_STATUS, (int*)&isLinked);
  if(isLinked == GL_FALSE)
  {
    GLint logSize = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logSize);
    char buff[logSize];
    glGetProgramInfoLog(program_id, logSize, &logSize, buff);
    printf("Linking %s \n",buff);
  }

}
