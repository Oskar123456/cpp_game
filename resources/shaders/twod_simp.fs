#version 330 core

uniform vec4 uni_frag_col;

out vec4 out_frag_col;

void main()
{
    out_frag_col = uni_frag_col;
}

