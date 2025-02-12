#version 330 core

in vec2 tex_coord;
in vec3 normal;

uniform sampler2D tex;
uniform vec4 color;
uniform bool use_tex;

out vec4 final_color;

void main()
{
    if (!use_tex)
        final_color = color;
    else if (color == vec4(1, 1, 1, 1))
        final_color = texture(tex, tex_coord);
    else
        final_color = mix(texture(tex, tex_coord), color, 0.5);
}
