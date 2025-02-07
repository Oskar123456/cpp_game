#version 330 core

in vec2 tex_coord;

uniform sampler2D tex;
uniform vec4 color;
uniform bool use_tex

out vec4 out_col;

void main()
{
    if (color == vec4(1, 1, 1, 1))
        out_col = texture(tex, tex_coord);
    else
        out_col = mix(texture(tex, tex_coord), color, 0.5);
}
