#version 330 core

in vec2 scr_coord;
in vec2 tex_coord;

uniform sampler2D tex;
uniform bool use_tex;
uniform vec4 color;
uniform vec2 c;
uniform float r;

out vec4 col_fin;

void main()
{    
    float dc = (scr_coord.x - c.x) * (scr_coord.x - c.x) + (scr_coord.y - c.y) * (scr_coord.y - c.y);
    
    if (dc > r * r)
        discard;
    
    float border_thickness = 2;
    float dp = r - sqrt(dc);
    float t = 1.0 - smoothstep(0.0, border_thickness, dp);
    
    if (use_tex) {
        col_fin = mix(texture(tex, tex_coord), color, 0.5);
    } else {
        col_fin = color;
    }
    
    col_fin.w = 1.0 - t;
}
