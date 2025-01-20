#version 330

in vec3 in_pos;

uniform mat4 modl
uniform mat4 view
uniform mat4 proj

void main()
{
    gl_Position = proj * view * modl * vec4(in_pos, 1.0);
}
