#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;
layout (location = 2) in vec3 in_normal;

uniform mat4 mvp;

out vec2 tex_coord;
out vec3 normal;

void main()
{
    gl_Position = mvp * vec4(in_pos, 1);
    tex_coord = in_tex_coord;
    normal = in_normal;
}

