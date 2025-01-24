#version 330 core

in vec2 frag_pos;

uniform vec4 uni_frag_col;
uniform float r;
uniform vec2 c;
uniform vec2 cr;
uniform vec2 asp;

out vec4 out_frag_col;

void main()
{
    float asp_rat = asp.x / asp.y;
    
    vec2 xy = vec2(asp_rat * frag_pos.x, frag_pos.y);
    
    float border_thickness = 0.01;
    float blueness = 0.02;
    float d = sqrt(dot(xy, xy));
    float t = 1.0 - smoothstep(0.0, border_thickness, abs(sqrt(r) - d));
    float tb = 1.0 - smoothstep(0.0, blueness, abs(sqrt(r) - d));
    
    out_frag_col = uni_frag_col;
    out_frag_col.y += tb;
    out_frag_col.w = 1.0 - t;
    
    if (dot(xy, xy) > r)
        discard;
}

