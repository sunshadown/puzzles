#version 330

uniform sampler2D background_uniform;

out vec4 Color;
in vec2 TexCord;

void main()
{
  Color = texture2D(background_uniform, TexCord);
}
