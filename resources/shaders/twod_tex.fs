#version 330 core

in vec2 tex_coord;

uniform sampler2D tex0;

out vec4 out_col;

void main()
{
    out_col = texture(tex0, tex_coord);
}

