#version 330 core

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex_coord;

uniform mat4 mvp;

out vec2 tex_coord;

void main()
{
    gl_Position = mvp * vec4(in_pos, 0, 1);
    tex_coord = in_tex_coord;
}
