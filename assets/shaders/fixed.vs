#version 330

layout(location = 0)in vec4 vPos;

out vec2 TexCord;

uniform mat4 projection;
uniform mat4 model;
uniform float m_depth;


void main()
{
  gl_Position = projection * model * vec4(vPos.xy,m_depth,1.0);
  TexCord = vPos.zw;
}
