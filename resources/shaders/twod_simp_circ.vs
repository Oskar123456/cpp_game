#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

uniform mat4 modl;

out vec2 frag_pos;

void main()
{
    gl_Position = modl * vec4(in_pos, 1);
    frag_pos = vec2(in_pos.x, in_pos.y);
}
