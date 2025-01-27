#version 330 core

layout (location = 0) in vec2 in_pos;

uniform mat4 proj;

void main()
{
    gl_Position = proj * vec4(in_pos, 0, 1);
}
