#version 330


in vec2 TexCord;
//out vec4 Color;
out vec4 pixel;

uniform vec3 m_color;
uniform vec2 tex_cords;
uniform sampler2D fixed_uniform;

float glow_size = .5;
vec3 glow_colour = vec3(0, 0, 0);
float glow_intensity = 5;
float glow_threshold = .2;


void main()
{
  //Color = texture2D(fixed_uniform, TexCord);
  pixel = texture(fixed_uniform, TexCord);

  if(pixel.a <= glow_threshold)
  {
    ivec2 size = textureSize(fixed_uniform, 0);
    float uv_x = TexCord.x * size.x;
    float uv_y = TexCord.y * size.y;

    float sum = 0.0;
    for (int n = 0; n < 9; ++n) {
      uv_y = (TexCord.y * size.y) + (glow_size * float(n - 4.5));
      float h_sum = 0.0;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x - (4.0 * glow_size), uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x - (3.0 * glow_size), uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x - (2.0 * glow_size), uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x - glow_size, uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x, uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x + glow_size, uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x + (2.0 * glow_size), uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x + (3.0 * glow_size), uv_y), 0).a;
      h_sum += texelFetch(fixed_uniform, ivec2(uv_x + (4.0 * glow_size), uv_y), 0).a;
      sum += h_sum / 9.0;
    }
    pixel = vec4(glow_colour, (sum / 9.0) * glow_intensity);
  }
  pixel = vec4(m_color, 1.0);
  //pixel = texture(fixed_uniform, TexCord);
}
