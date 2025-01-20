/*=======================================
 *
 * cpp_game
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *=======================================*/

#include "../include/util.hpp"
#include "../include/world.hpp"

#define SCR_W 800
#define SCR_H 600

static Font font_fira;

static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint);
static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, Vector3 text_position, float fontSize, bool backface, Color tint);

struct Unit {
    Vector3 pos;
    Vector3 dir;
    float ms_norm, ms_sprnt;
    float fs_acc, fs;
    float cam_dist;
    Camera cam;
};

float deg_to_rad(float deg)
{
    return deg * ((2.0f*M_PI) / 360.0f);
}

void unit_update(Unit* u)
{
    float ms = (IsKeyDown(KEY_LEFT_SHIFT)) ? u->ms_sprnt : u->ms_norm;

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        u->pos.x += ms;
        u->pos.z += ms;
    }

    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        u->pos.x -= ms;
        u->pos.z -= ms;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        u->pos.x += ms;
        u->pos.z -= ms;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        u->pos.x -= ms;
        u->pos.z += ms;
    }

    if (IsKeyDown(KEY_PAGE_DOWN)) {
        u->cam_dist += 2;
    }

    if (IsKeyDown(KEY_PAGE_UP)) {
        u->cam_dist -= 2;
    }


    /* u->pos = Vector3Add(u->pos, Vector3Scale(u->dir, u->ms_norm)); */
    float h = sin(30 * DEG2RAD) * u->cam_dist;
    float b = sqrt(u->cam_dist * u->cam_dist - h * h);

    float offs = b / sqrt(2);
    u->cam.position = {u->pos.x - offs,
        u->pos.y + h,
        u->pos.z - offs};
    u->cam.target = u->pos;
}

void poll_input()
{
    if (IsKeyPressed(KEY_F11))
        ToggleBorderlessWindowed();
}

Unit player = {
    {0, 0, 0},
    {0, 0, 0},
    0.08, 0.32, 0.04, 0,
    160,
    {0},
};

std::string vec3f_print(Vector3 v)
{
    char str[24];
    sprintf(str, "(%.1f, %.1f, %.1f)", v.x, v.y, v.z);
    return std::string(str);
}

void draw_axes(Vector3 pos, int len)
{
    Vector3 posn = {pos.x - 0.5f, pos.y - 0.5f + 0.05f, pos.z - 0.5f};
    Vector3 posex = {posn.x - len, posn.y + 0.05f, posn.z};
    Vector3 posey = {posn.x, posn.y + len + 0.05f, posn.z};
    Vector3 posez = {posn.x, posn.y + 0.05f, posn.z - len};
    Vector3 postxt = {posn.x - 0.4f, posn.y, posn.z + 0.4f};

    DrawCylinderEx(posn, posex, 0.05, 0.05, 10, {0, 55, 128, 64});
    DrawCylinderEx(posn, posey, 0.05, 0.05, 10, {0, 55, 128, 64});
    DrawCylinderEx(posn, posez, 0.05, 0.05, 10, {0, 55, 128, 64});

    DrawText3D(font_fira, "-x", posex, 20, 0.5, 0, false, {0, 255, 255, 128});
    DrawText3D(font_fira, "+y", posey, 20, 0.5, 0, false, {0, 255, 255, 128});
    DrawText3D(font_fira, "-z", posez, 20, 0.5, 0, false, {0, 255, 255, 128});

    DrawText3D(font_fira, vec3f_print(pos).c_str(), postxt, 20, 0.5, 0, false, {0, 255, 255, 128});
}

int main(int argc, char *argv[])
{
    /* init */
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCR_W, SCR_H, "cpp_game");
    DisableCursor();
    SetExitKey(0);
    SetTargetFPS(60);

    /* game login */
    player.cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    player.cam.fovy = 5.0f;
    player.cam.projection = CAMERA_PERSPECTIVE;

    /* draw text stuff */
    font_fira = LoadFontEx("/home/oskar/.fonts/ZedMono/ZedMonoNerdFont-Regular.ttf", 64, 0, 250);

    int fs = 20;
    int pad = 4;

    while (!WindowShouldClose())
    {
        poll_input();
        unit_update(&player);

        BeginDrawing();

            ClearBackground(BLUE);

            BeginMode3D(player.cam);

                DrawGrid(100, 1);
                DrawPlane({player.pos.x, player.pos.y - 0.51f, player.pos.z}, {100, 100}, GRAY);
                DrawCubeWires(player.pos, 1, 1, 1, GREEN);
                draw_axes(player.pos, 2);

                /* mat4s model = GLMS_MAT4_IDENTITY; */
                /* model = glms_translate(model, (vec3s){4.0f, -3.5f, 0.0}); */
                /* model = glms_scale(model, (vec3s){0.5f, 0.5f, 0.5f}); */
                /* shader.setMat4("model", model); */
                rlPushMatrix();
                rlBegin(RL_QUADS);
                    rlTranslatef(0.0f, 3.5f, 0.0);
                    rlScalef(5.5f, 5.5f, 5.5f);
                    rlColor3f(1.0f, 0.0f, 0.0f);
                    world_render_cube();
                rlEnd();
                rlPopMatrix();

            EndMode3D();

            DrawText("cpp_game", pad, pad, fs, RAYWHITE);

            DrawText(TextFormat("p_pos:  (%.1f,%.1f,%.1f)", player.pos.x, player.pos.y, player.pos.z), pad, fs + pad, fs, RAYWHITE);
            DrawText(TextFormat("c_pos:  (%.1f,%.1f,%.1f)", player.cam.position.x, player.cam.position.y, player.cam.position.z), pad, 2 * (fs + pad), fs, RAYWHITE);
            DrawText(TextFormat("c_at:  (%.1f,%.1f,%.1f)", player.cam.target.x, player.cam.target.y, player.cam.target.z), pad, 3 * (fs + pad), fs, RAYWHITE);

        EndDrawing();
    }

    return 0;
}

static void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, Vector3 text_position, float fontSize, bool backface, Color tint)
{
    int index = GetGlyphIndex(font, codepoint);
    float scale = fontSize/(float)font.baseSize;

    position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding)/(float)font.baseSize*scale;
    position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding)/(float)font.baseSize*scale;

    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f*font.glyphPadding, font.recs[index].height + 2.0f*font.glyphPadding };

    float width = (float)(font.recs[index].width + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;
    float height = (float)(font.recs[index].height + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;

    if (font.texture.id > 0)
    {
        const float x = 0.0f;
        const float y = 0.0f;
        const float z = 0.0f;

        const float tx = srcRec.x/font.texture.width;
        const float ty = srcRec.y/font.texture.height;
        const float tw = (srcRec.x+srcRec.width)/font.texture.width;
        const float th = (srcRec.y+srcRec.height)/font.texture.height;

        if (false) DrawCubeWiresV((Vector3){ position.x + width/2, position.y, position.z + height/2}, (Vector3){ width, 0.25, height }, VIOLET);

        rlCheckRenderBatchLimit(4 + 4*backface);
        rlSetTexture(font.texture.id);

        rlPushMatrix();
            rlTranslatef(text_position.x, text_position.y, text_position.z);

            /* struct timespec t; */
            /* clock_gettime(CLOCK_REALTIME, &t); */
            /* rlRotatef(((t.tv_nsec / 1000000 + (t.tv_sec % 4) * 1000) / 4000.0f) * 360.0f, 0, 1, 0); */
            rlRotatef(225.0f, 0, 1, 0);

            rlTranslatef(-text_position.x, -text_position.y, -text_position.z);

            rlTranslatef(position.x, position.y, position.z);

            rlBegin(RL_QUADS);
                rlColor4ub(tint.r, tint.g, tint.b, tint.a);

                // Front Face
                rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
                rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);              // Top Left Of The Texture and Quad
                rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height);     // Bottom Left Of The Texture and Quad
                rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);     // Bottom Right Of The Texture and Quad
                rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad

                if (backface)
                {
                    // Back Face
                    rlNormal3f(0.0f, -1.0f, 0.0f);                              // Normal Pointing Down
                    rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);          // Top Right Of The Texture and Quad
                    rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);          // Top Left Of The Texture and Quad
                    rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height); // Bottom Left Of The Texture and Quad
                    rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height); // Bottom Right Of The Texture and Quad
                }
            rlEnd();
        rlPopMatrix();

        rlSetTexture(0);
    }
}

static void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint)
{
    int length = TextLength(text);

    float textOffsetY = 0.0f;
    float textOffsetX = 0.0f;

    float scale = fontSize/(float)font.baseSize;

    for (int i = 0; i < length;)
    {
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        if (codepoint == 0x3f) codepointByteCount = 1;

        if (codepoint == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += scale + lineSpacing/(float)font.baseSize*scale;
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint3D(font, codepoint,
                        (Vector3){ position.x + textOffsetX, position.y, position.z + textOffsetY },
                        (Vector3){ position.x, position.y, position.z},
                        fontSize, backface, tint);
            }

            if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing)/(float)font.baseSize*scale;
            else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing)/(float)font.baseSize*scale;
        }

        i += codepointByteCount;
    }
}
