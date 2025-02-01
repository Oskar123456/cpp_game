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
    float a = min(sqrt((border_rad * border_rad) / 2.0f), min(dims.y / 2.0f, dims.x / 2.0f));

    vec2 c1 = vec2(center.x - dims.x / 2.0f + a, center.y - dims.y / 2.0f + a);
    vec2 c2 = vec2(center.x - dims.x / 2.0f + a, center.y + dims.y / 2.0f - a);
    vec2 c3 = vec2(center.x + dims.x / 2.0f - a, center.y - dims.y / 2.0f + a);
    vec2 c4 = vec2(center.x + dims.x / 2.0f - a, center.y + dims.y / 2.0f - a);

    float border_fuzz = 1;

    if (scr_coord.x <= c1.x) {
        if (scr_coord.y <= c1.y) {
            float r = (scr_coord.x - c1.x) * (scr_coord.x - c1.x) + (scr_coord.y - c1.y) * (scr_coord.y - c1.y);
            if (r > a * a)
                discard;
            border_fuzz = smoothstep(0, 2, a - sqrt(r));
        }
        
        if (scr_coord.y >= c2.y) {
            float r = (scr_coord.x - c2.x) * (scr_coord.x - c2.x) + (scr_coord.y - c2.y) * (scr_coord.y - c2.y);
            if (r > a * a)
                discard;
            border_fuzz = smoothstep(0, 2, a - sqrt(r));
        }
    }
    
    else if (scr_coord.x >= c3.x) {
        if (scr_coord.y <= c3.y) {
            float r = (scr_coord.x - c3.x) * (scr_coord.x - c3.x) + (scr_coord.y - c3.y) * (scr_coord.y - c3.y);
            if (r > a * a)
                discard;
            border_fuzz = smoothstep(0, 2, a - sqrt(r));
        }
        
        if (scr_coord.y >= c4.y) {
            float r = (scr_coord.x - c4.x) * (scr_coord.x - c4.x) + (scr_coord.y - c4.y) * (scr_coord.y - c4.y);
            if (r > a * a)
                discard;
            border_fuzz = smoothstep(0, 2, a - sqrt(r));
        }
    }

    if (border_fuzz == 1) {
        border_fuzz = min(smoothstep(0, 2, dims.y / 2.0f - abs(scr_coord.y - center.y)),
                smoothstep(0, 2, dims.x / 2.0f - abs(scr_coord.x - center.x)));
    }

    /* coloring */
    if (!use_tex)
        out_col = color;
    else if (color == vec4(1, 1, 1, 1))
        out_col = texture(tex, tex_coord);
    else
        out_col = mix(texture(tex, tex_coord), color, 0.5);

    out_col.w = border_fuzz;
}

