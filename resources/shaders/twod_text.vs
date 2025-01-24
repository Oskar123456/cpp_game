#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;

uniform mat4 modl;

out vec2 tex_coord;

void main()
{
    gl_Position = modl * vec4(in_pos, 1);
    tex_coord = vec2(in_tex_coord.x, in_tex_coord.y);
}
