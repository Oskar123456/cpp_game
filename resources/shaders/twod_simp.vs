#version 330 core

layout (location = 0) in vec3 in_pos;

uniform mat4 modl;

void main()
{
    gl_Position = modl * vec4(in_pos, 1);
}
