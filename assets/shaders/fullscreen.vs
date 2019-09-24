#version 330

layout(location = 0)in vec3 vPos;
out vec2 TexCord;

void main()
{
  gl_Position = vec4(vPos,1.0);
  TexCord = (vPos.xy + 1.0)/2.0;
}
