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

#include <raylib.h>
#include <raymath.h>

#include <cstdio>
#include <cmath>

#define SCR_W 800
#define SCR_H 600

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

    if (IsKeyDown(KEY_UP)) {
        u->pos.x += ms;
        u->pos.z += ms;
    }

    if (IsKeyDown(KEY_DOWN)) {
        u->pos.x -= ms;
        u->pos.z -= ms;
    }

    if (IsKeyDown(KEY_LEFT)) {
        u->pos.x += ms;
        u->pos.z -= ms;
    }

    if (IsKeyDown(KEY_RIGHT)) {
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

    /* draw text params */
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
                DrawCubeWires(player.pos, 1, 1, 1, GREEN);

            EndMode3D();

            DrawText("cpp_game", pad, pad, fs, RAYWHITE);

            DrawText(TextFormat("p_pos:  (%.1f,%.1f,%.1f)", player.pos.x, player.pos.y, player.pos.z), pad, fs + pad, fs, RAYWHITE);
            DrawText(TextFormat("c_pos:  (%.1f,%.1f,%.1f)", player.cam.position.x, player.cam.position.y, player.cam.position.z), pad, 2 * (fs + pad), fs, RAYWHITE);
            DrawText(TextFormat("c_at:  (%.1f,%.1f,%.1f)", player.cam.target.x, player.cam.target.y, player.cam.target.z), pad, 3 * (fs + pad), fs, RAYWHITE);

        EndDrawing();
    }

    return 0;
}
