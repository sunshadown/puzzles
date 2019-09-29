#version 330

in vec2 TexCord;

uniform sampler2D fixed_uniform;

out vec4 pixel;

void main()
{
  pixel = texture(fixed_uniform, TexCord);
}
