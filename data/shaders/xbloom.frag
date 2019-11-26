#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec2 norm_pos;

uniform sampler2D screenTexture;
uniform uint scan_pos;
uniform ivec2 resolution;

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/* Blur filter copied from: https://github.com/Jam3/glsl-fast-gaussian-blur */

float normpdf(float x, float sigma)
{
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.411764705882353) * direction;
    vec2 off2 = vec2(3.2941176470588234) * direction;
    vec2 off3 = vec2(5.176470588235294) * direction;
    color += texture2D(image, uv) * 0.1964825501511404;
    color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;
    color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;
    color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;
    color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;
    color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;
    color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;
    return color;
}

void main()
{
    vec3 x_blur = blur13(screenTexture, TexCoords, vec2(resolution.x, resolution.y), vec2(1.0f, 0.0f)).rgb;

    //FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0f);
    FragColor = vec4(x_blur, 1.0f);
}
