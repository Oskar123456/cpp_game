#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

uniform mat4 modl;

void main()
{
    gl_Position = modl * vec4(in_pos, 1);
}
