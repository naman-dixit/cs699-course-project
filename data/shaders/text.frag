#version 330 core

in vec2 Tex_coord;

out vec4 out_color;

uniform sampler2D font_bitmap;
uniform vec3 color;

void main()
{
    vec4 temp_color = texture(font_bitmap, Tex_coord);

//    vec3 linear_color = color * temp_color.r; // Light sharp
//    vec3 exp_color = color * (1 - exp(-2.4 * temp_color.r)); // Light blurred
//    vec3 tanh_color = color * tanh(2 * temp_color.r); // Dark blurred
    vec4 better_tanh_color = vec4(color, 1) * sqrt(tanh(2 * temp_color.r * temp_color.r)); // Dark sharp
    out_color = better_tanh_color;
}
