#version 330

layout(location = 0)in vec4 vPos;
layout(location = 1)in vec3 vDir;

uniform mat4 projection;
uniform mat4 model;
uniform float m_depth;
uniform float time;

void main()
{
  vec4 npos = model * vec4(vPos.xy,m_depth,1.0);
  npos.xy = npos.xy + vDir.xy * time * time * 10.0f;
  gl_Position = projection * npos;
}
