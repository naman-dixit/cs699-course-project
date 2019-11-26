#version 330 core

in vec2 position;
in vec2 tex_coord;

out vec2 Tex_coord;

uniform mat4 transform;

uniform float depth;

void main()
{
    vec4 pos_transformed = transform * vec4(position, 0, 1);
    gl_Position = vec4(pos_transformed.xy, -depth, 1);
    Tex_coord = tex_coord;
}
