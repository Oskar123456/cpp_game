#version 330 core

in vec2 tex_coord;
in vec2 scr_coord;

uniform sampler2D tex;
uniform bool use_tex;
uniform vec4 color;
uniform vec2 dims;
uniform vec2 center;
uniform float border_rad;

out vec4 out_col;

void main()
{
    /* corner calc */

    float sc_h = dims.y / 2.0f;
    float sc_w = min(dims.x / 2.0f, sc_h);
    float sc_r = max(1, sqrt(sc_h * sc_h + sc_w * sc_w) - border_rad);
    vec2 sc_c_l = vec2(center.x - (dims.x / 2.0f - sc_w), center.y);
    vec2 sc_c_r = vec2(center.x + (dims.x / 2.0f - sc_w), center.y);
    
    float border_fuzz = 1;

    if (scr_coord.x < sc_c_l.x) {
        float r = sqrt((scr_coord.x - sc_c_l.x) * (scr_coord.x - sc_c_l.x) + (scr_coord.y - sc_c_l.y) * (scr_coord.y - sc_c_l.y));
        
        if (r > sc_r) {
            discard;
        }
        
        border_fuzz = smoothstep(0, 2, sc_r - r);
    }
    
    else if (scr_coord.x > sc_c_r.x) {
        float r = sqrt((scr_coord.x - sc_c_r.x) * (scr_coord.x - sc_c_r.x) + (scr_coord.y - sc_c_r.y) * (scr_coord.y - sc_c_r.y));
        
        if (r > sc_r) {
            discard;
        }
        
        border_fuzz = smoothstep(0, 2, sc_r - r);
    }

    else {
        border_fuzz = smoothstep(0, 2, dims.y / 2.0f - abs((scr_coord.y - center.y)));
    }

    /* coloring */
    if (!use_tex) {
        out_col = color;
    }
    else if (color == vec4(1, 1, 1, 1))
        out_col = texture(tex, tex_coord);
    else
        out_col = mix(texture(tex, tex_coord), color, 0.5);

    out_col.w = border_fuzz;
}

