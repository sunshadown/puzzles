#version 330

in vec2 TexCord;

uniform sampler2D fixed_uniform;
uniform int choosen;

out vec4 pixel;

void main()
{
  pixel = texture(fixed_uniform, TexCord);
  if(choosen > 0)
  {
    pixel.rgb = pixel.rgb * vec3(0.2,0.2,0.7);
  }
}
