#version 140

uniform vec2 dim;
uniform vec2 offset;
uniform float zoom;

layout(std140) uniform BloodPosition
{
  uniform vec2 positions[256];
};

layout(std140) uniform BloodSpeed
{
  uniform vec2 speeds[128];
};

in vec2 pos;
in vec2 coord;

out vec2 vert_coord;

void main()
{
  vert_coord = coord;
  int id = gl_InstanceID;
  vec2 speed = speeds[id];

  speed = normalize(speed);
  vec2 position = positions[id];

  // vec2 perp = vec2(-speed.y, speed.x);
  vec2 out_pos = pos * 0.01f;
  out_pos.x *= 4.0;
  out_pos = vec2(out_pos.x * speed.x - out_pos.y * speed.y, out_pos.x * speed.y + out_pos.y * speed.x);
  out_pos += position + offset;
  out_pos *= zoom;
  gl_Position = vec4(out_pos / dim, 0.0f, 1.0f);
}
