#ifndef SHADER_H
#define SHADER_H

//OpenGl wrangler
#include "../lib/includes/GL/glew.h"

#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class Shader
{
private:
  GLuint program_id;
  void Init();
public:
  Shader();
  ~Shader();

  void Bind();
  GLuint GetLocation(std::string name);
  char* LoadShader(std::string path, size_t buff_size);
  void AttachShader(char* shader, int flag);
  void LinkShader();
};


#endif
