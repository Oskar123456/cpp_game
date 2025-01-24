#version 330 core

in vec2 tex_coord;

uniform sampler2D tex0;
uniform vec4 uni_frag_col;

out vec4 out_col;

void main()
{
    vec4 sampled = vec4(1, 1, 1, texture(tex0, tex_coord).r);
    out_col = uni_frag_col * sampled;
}

