#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec2 norm_pos;

uniform sampler2D screenTexture;
uniform sampler2D blurTexture;
uniform uint scan_pos;
uniform ivec2 resolution;

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/* Blur filter copied from: https://github.com/Jam3/glsl-fast-gaussian-blur */

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
    vec4 background = vec4(0.0f);
    float background_brightness = 0.9f;
    float noise_magnitude = 0.2f + fract(float(scan_pos)/1.1)/30.0f;
    int scanline_spacing = 6;
    vec3 ambient = vec3(0.2f);

    { // Background
        { // Add scanlines
            float noise = rand(gl_FragCoord.xy)* noise_magnitude;
            float spacing = float(scanline_spacing);
            int gap = scanline_spacing / 3;

            vec3 background_green = vec3(0.0f);
            background_green.g = 1.00f * fract((gl_FragCoord.y + (0 * gap)) / spacing) + noise;
            background_green.r = 0.85f * fract((gl_FragCoord.y + (1 * gap)) / spacing) + noise;
            background_green.b = 0.65f * fract((gl_FragCoord.y + (2 * gap)) / spacing) + noise;

            vec3 background_blue = vec3(0.0f);
            background_blue.b = 1.00f  * fract((gl_FragCoord.y + (2 * gap)) / spacing) + noise;
            background_blue.g = 0.85f  * fract((gl_FragCoord.y + (0 * gap)) / spacing) + noise;
            background_blue.r = 0.65f * fract((gl_FragCoord.y + (1 * gap)) / spacing) + noise;

            float threshold = step(0.0f, norm_pos.x);
            background.rgb = threshold * background_green + (1.0f - threshold) * background_blue;

            // CRT dots
            //background.rgb = background.rgb * (1 - fract((gl_FragCoord.x + (0 * gap)) / spacing)) + ambient;
        }

        // Darkness towards edges
        background = background * (1 - 0.5f * (abs(norm_pos.x) * abs(norm_pos.x) + 0.8f * abs(norm_pos.y) * abs(norm_pos.y)));

        // Scan gun trace
        background = background + 0.05f * step(scan_pos, gl_FragCoord.y) * exp(-0.002f * (gl_FragCoord.y - scan_pos));

        background = background_brightness * background;
    }

    vec3 y_blur = blur13(blurTexture, TexCoords, vec2(resolution.x, resolution.y), vec2(0.0f, 1.0f)).rgb;
    vec3 blur = y_blur + texture(blurTexture, TexCoords).rgb;

    vec3 text_color = texture(screenTexture, TexCoords).rgb;
    float text_alpha = step(vec3(0.01, 0.01, 0.01), text_color).x;

    //FragColor = vec4(background.rgb, 1.0f);
    FragColor = vec4(blur, 0.5f) + vec4(background.rgb, 0.05f) + vec4(text_color, text_alpha);
}
