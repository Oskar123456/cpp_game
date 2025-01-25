#version 330 core

in vec2 tex_coord;

uniform sampler2D text;
uniform vec4 text_color;

out vec4 col_fin;

void main()
{    
    col_fin = vec4(text_color.xyz, texture(text, tex_coord).r);
    /* col_fin = vec4(1, 1, 1, 1); */
}
