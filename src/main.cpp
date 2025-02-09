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
 *=======================================*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_time.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <glad/glad.h>
#include <util.h>
#include <world.h>
#include <twod.h>
#include <threed.h>
#include <camera.h>
#include <menu.h>
#include <logging.h>

#include <iostream>
#include <cstdio>
#include <map>
#include <set>

#define WINDOW_WIDTH   1000
#define WINDOW_HEIGHT  800

using namespace std;

/* TEMP */
/* TEMP */
/* TEMP */
static int scr_w, scr_h;
static SDL_Time t_start, t_last_update, t_now, t_delta;

static SDL_Time t_hist[60];
static int t_hist_idx;

static vec3s circ = {400, 400, 100};
static vec4s rect = {100, 100, 100, 100};
vec3s cube_pos = {0, 0, -10};
vec3s cube_rot_axis = {0, 1, 0};
float cube_rot = 0;
static float rect_angle = M_PI / 4;
static int border_radius = 0;

static bool paused;

static bool exit_first = true;

extern char **environ;

static bool mouse_hidden = true;
static Button btn;

Camera cam;
static World world;

u32 main_mat_tex_id;
u32 dirt_tex_id;
/* TEMP */
/* TEMP */
/* TEMP */

struct MetaData {
    const char *key;
    const char *value;
};

struct AppState {
    SDL_Window *window;
    SDL_GLContext gl_context;
};

struct AppState app_state;

void poll_mouse()
{
    float x, y;
    SDL_MouseButtonFlags mf = SDL_GetMouseState(&x, &y);
    if (mf & SDL_BUTTON_LEFT) {
    }
}

void poll_key()
{
    int n;
    const bool *kcs = SDL_GetKeyboardState(&n);

    int ms = 4;
    if (kcs[SDL_SCANCODE_UP]) {
        rect.y = fmax(INT32_MIN, rect.y - ms);
        /* cube_pos.y += 0.1f; */
    }
    if (kcs[SDL_SCANCODE_DOWN]) {
        rect.y = fmax(INT32_MIN, rect.y + ms);
        /* cube_pos.y -= 0.1f; */
    }
    if (kcs[SDL_SCANCODE_LEFT]) {
        rect.x = fmax(INT32_MIN, rect.x - ms);
        /* cube_pos.x -= 0.1f; */
    }
    if (kcs[SDL_SCANCODE_RIGHT]) {
        rect.x = fmax(INT32_MIN, rect.x + ms);
        /* cube_pos.x += 0.1f; */
    }
    if (kcs[SDL_SCANCODE_R]) {
        rect_angle = rect_angle + 0.10;
        cube_rot += 0.1f;
    }
    if (kcs[SDL_SCANCODE_W]) {
        circ.y = fmax(0, circ.y + ms);
        rect.z++;
        /* cube_pos.z -= 0.1f; */
        /* printf("%f %f\n", circ.x, circ.y); */
    }
    if (kcs[SDL_SCANCODE_S]) {
        circ.y = fmax(0, circ.y - ms);
        rect.z--;
        /* cube_pos.z += 0.1f; */
        /* printf("%f %f\n", circ.x, circ.y); */
    }
    if (kcs[SDL_SCANCODE_A]) {
        circ.x = fmax(0, circ.x - ms);
        rect.w--;
        /* printf("%f %f\n", circ.x, circ.y); */
    }
    if (kcs[SDL_SCANCODE_D]) {
        circ.x = fmax(0, circ.x + ms);
        rect.w++;
        /* printf("%f %f\n", circ.x, circ.y); */
    }
    if (kcs[SDL_SCANCODE_R]) {
        rect_angle = rect_angle + 0.10;
    }
    if (kcs[SDL_SCANCODE_N]) {
        border_radius += 1;
    }
    if (kcs[SDL_SCANCODE_M]) {
        border_radius -= 1;
    }
}

SDL_AppResult mouse_move_callback(SDL_MouseMotionEvent e)
{
    int p = 10;
    if (mouse_hidden && (e.x < p || e.x >= scr_w - p || e.y < p || e.y >= scr_h - p))
        SDL_WarpMouseInWindow(app_state.window, scr_w / 2, scr_h / 2);
    camera_free_update_mouse(cam, e.xrel, e.yrel);
    return SDL_APP_CONTINUE;
}

SDL_AppResult mouse_click_callback(SDL_MouseButtonEvent e)
{
    if (e.button & SDL_BUTTON_LEFT && e.down)
        menu_button_check_if_clicked(btn, e.x, e.y);

    return SDL_APP_CONTINUE;
}

SDL_AppResult key_callback(SDL_KeyboardEvent kb_event)
{
    switch (kb_event.scancode) {
        case SDL_SCANCODE_F11:
            SDL_SetWindowFullscreen(app_state.window, !(SDL_GetWindowFlags(app_state.window) & SDL_WINDOW_FULLSCREEN));
            break;
        case SDL_SCANCODE_ESCAPE:
            printf("<ESC> pressed, exiting...\n");
            return SDL_APP_SUCCESS;
            break;
        case SDL_SCANCODE_L:
            if (mouse_hidden) {
                SDL_ShowCursor();
                /* bool succ = SDL_CaptureMouse(false); */
                /* printf("%s\n", SDL_GetError()); */
                /* SDL_SetWindowMouseGrab(app_state.window, false); */
                mouse_hidden = !mouse_hidden;
            }
            else {
                SDL_HideCursor();
                /* bool succ = SDL_CaptureMouse(true); */
                /* printf("%s\n", SDL_GetError()); */
                /* SDL_SetWindowMouseGrab(app_state.window, true); */
                mouse_hidden = !mouse_hidden;
            }
            break;
        case SDL_SCANCODE_P:
            glms_vec4_print(rect, stdout);
            glms_vec3_print(circ, stdout);
            break;
        case SDL_SCANCODE_PAGEUP:
            rect.z += 5;
            rect.w += 5;
            circ.z += 5;
            break;
        case SDL_SCANCODE_PAGEDOWN:
            rect.z -= 5;
            rect.w -= 5;
            circ.z -= 5;
            break;
        case SDL_SCANCODE_SPACE:
            paused = !paused;
            printf("%s\n", (paused) ? "paused" : "unpaused");
            break;
        case SDL_SCANCODE_F12:
            screenshot();
            break;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

struct MetaData metadata[] =
{
    { SDL_PROP_APP_METADATA_NAME_STRING, "LOL" },
    { SDL_PROP_APP_METADATA_URL_STRING, "LOL" },
    { SDL_PROP_APP_METADATA_CREATOR_STRING, "LOL" },
    { SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "LOL" },
    { SDL_PROP_APP_METADATA_TYPE_STRING, "LOL" }
};

void pp()
{
    printf("click\n");
}

SDL_AppResult SDL_AppInit(void **state, int argc, char *argv[])
{
    /* begin */
    *state = (void*)&app_state;
    AppState *as = (AppState*)*state;

    if (!SDL_Init(SDL_INIT_VIDEO)){
        return SDL_APP_FAILURE;
    }

    bool gl_loaded = SDL_GL_LoadLibrary(NULL);
    if (!gl_loaded)
    {
        std::cout << "Failed to initialize GL (" << SDL_GetError() << ")" << std::endl;
        return SDL_APP_FAILURE;
    }

    const char* glsl_version = "#version 330 core";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    /* create window with graphics context */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    /* anti-aliasing */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;// | SDL_WINDOW_HIDDEN;
    as->window = SDL_CreateWindow("Cube Render", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
    if (as->window == NULL)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetWindowPosition(as->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    as->gl_context = SDL_GL_CreateContext(as->window);
    if (as->gl_context == NULL)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_MakeCurrent(as->window, as->gl_context);
    SDL_GL_SetSwapInterval(1); /* vsync */
    /* SDL_ShowWindow(as->window); */

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return SDL_APP_FAILURE;
    }

    glEnable(GL_MULTISAMPLE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_GetCurrentTime(&t_last_update);
    SDL_GetCurrentTime(&t_start);

    SDL_GetWindowSizeInPixels(as->window, &scr_w, &scr_h);
    twod_update_scr_dims(scr_w, scr_h);
    camera_update_scr_dims(scr_w, scr_h);
    twod_init();
    threed_init();
    world_init();

    int radius = 20;
    for (int x = 0; x < radius; ++x) {
        for (int y = 0; y < sqrt(radius); ++y) {
            for (int z = 0; z < radius; ++z) {
                world_gen_chunk(world, {x, y, z});
            }
        }
    }

    /* cells.emplace((vec2s){11, 11}); */
    /* cells.emplace((vec2s){11, 12}); */
    /* cells.emplace((vec2s){11, 13}); */
    /* cells.emplace((vec2s){12, 11}); */
    /* cells.emplace((vec2s){10, 12}); */

    /* text */
    /* testing */
    /* twod_create_tex("/home/oskar/Documents/cpp_game/resources/textures/minecraft_grass_side.jpg", "dirt"); */
    /* dirt_tex_id = threed_create_tex("/home/oskar/Documents/cpp_game/resources/textures/minecraft_grass_side.jpg", "dirt"); */
    main_mat_tex_id = threed_create_tex("/home/oskar/Documents/cpp_game/resources/textures/spritesheet_tiles_10x10.png", "main_mat", GL_RGBA);
    // twod_create_tex("/home/oskar/Documents/cpp_game/resources/textures/minecraft_snow_top.jpg", "snow");

    btn.text = "testing button";
    btn.pos = {600, 100};
    btn.color_fg = COL_PINK;
    btn.color_bg = {0.1f, 0.3f, 0.7f, 0.5f};
    btn.width = 300;
    btn.height = btn.width / 4;
    btn.border_radius = btn.width / 5;
    btn.rotation = 0;
    btn.on_click = pp;

    cam.up = {0, 1, 0};
    cam.at = {0, 0, -10};
    cam.pos = {0, 10, 10};
    cam.fov = 45;
    camera_update(cam);

    SDL_WarpMouseGlobal(scr_w / 2, scr_h / 2);
    SDL_HideCursor();
    mouse_hidden = true;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *state)
{
    AppState *as = (AppState*)state;

    SDL_GetCurrentTime(&t_now);
    t_delta = t_now - t_last_update;
    float t_delta_ms = t_delta / 1000000.0f;
    t_last_update = t_now;
    t_hist[t_hist_idx++ % 60] = t_delta;
    SDL_Time t_avg = 0;
    for (int i = 0; i < 60; ++i)
        t_avg += t_hist[i];
    t_avg /= 60;

    int kb_n;
    /* float m_x, m_y; */
    /* SDL_MouseButtonFlags mb_flags = SDL_GetGlobalMouseState(&m_x, &m_y); */
    const bool *kb_state = SDL_GetKeyboardState(&kb_n);

    poll_key();
    poll_mouse();
    camera_free_poll_keys(cam, kb_state, t_delta_ms);
    /* camera_free_poll_mouse(cam, mb_flags, m_x, m_y, t_delta_ms); */
    camera_update(cam);

    Color bg = COL_TOKYO;
    glClearColor(VEC4EXP(bg));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    i64 rot_interval_ms = 1000;
    i64 dt_ms = (t_now - t_start) / 1000000;
    float angle = dt_ms * (2 * M_PI / rot_interval_ms);

    btn.width = rect.z * 4;
    btn.height = rect.w;

    /* twod_draw_rectf_rounded(rect.x, rect.y, rect.z, rect.w, border_radius, COL_WHITE, "dirt", rect_angle); */
    /* menu_button_update(btn, t_start, t_now, t_delta); */
    /* menu_button_render(btn, t_start, t_now, t_delta); */

    /* threed_render_cube(cam.view_proj, cube_pos, cube_rot, cube_rot_axis, COL_WHITE, main_mat_tex_id); */

    world_render(world, cam);

    if (!paused) {
    }

    vec3i cam_chunk_pos = world_map_to_chunkf(cam.pos);
    vec3i cam_vox_pos = world_map_to_voxf(cam.pos);

    char fps_str[300];
    sprintf(fps_str, "ft: %5.1fms (dt: %5.2fs)", t_avg / 1000000.0f, dt_ms / 1000.0f);
    twod_draw_text(fps_str, strlen(fps_str), 10, 20, 0.3, COL_WHITE, 0);
    sprintf(fps_str, "cam: (%5.1f, %5.1f, %5.1f) at: (%5.1f, %5.1f, %5.1f) up: (%5.1f, %5.1f, %5.1f)",
            cam.pos.x, cam.pos.y, cam.pos.z,
            cam.at.x, cam.at.y, cam.at.z,
            cam.up.x, cam.up.y, cam.up.z);
    twod_draw_text(fps_str, strlen(fps_str), 10, 45, 0.3, COL_WHITE, 0);
    sprintf(fps_str, "pos: chunk(%d, %d, %d) vox(%d, %d, %d)",
            cam_chunk_pos.x, cam_chunk_pos.y, cam_chunk_pos.z,
            cam_vox_pos.x, cam_vox_pos.y, cam_vox_pos.z);
    twod_draw_text(fps_str, strlen(fps_str), 10, 70, 0.3, COL_WHITE, 0);
    sprintf(fps_str, "world.settings: seed: %d, h:%d-%d(wl:%d), s/l/g/o: %5.2f/%5.2f/%5.2f/%5.2f",
            world.settings.seed, world.settings.max_height, world.settings.min_height,
            world.settings.water_level, world.settings.scale, world.settings.lacunarity, world.settings.gain, world.settings.octaves);
    twod_draw_text(fps_str, strlen(fps_str), 10, 95, 0.3, COL_WHITE, 0);
    sprintf(fps_str, "cube: (%5.1f, %5.1f, %5.1f)", cube_pos.x, cube_pos.y, cube_pos.z);
    twod_draw_text(fps_str, strlen(fps_str), 10, 120, 0.3, COL_WHITE, 0);

    SDL_GL_SwapWindow(as->window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *state, SDL_Event *event)
{
    SDL_AppResult res;
    AppState *as = (AppState*)state;

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        SDL_GetWindowSizeInPixels(as->window, &scr_w, &scr_h);
        twod_update_scr_dims(scr_w, scr_h);
        camera_update_scr_dims(scr_w, scr_h);
        camera_update(cam);
        glViewport(0, 0, scr_w, scr_h);
        /* printf("%d %d\n", w, h); */
    }

    if (event->type == SDL_EVENT_KEY_DOWN)
        if ((res = key_callback(event->key)))
            return res;

    if (event->type == SDL_EVENT_MOUSE_MOTION)
        mouse_move_callback(event->motion);

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        mouse_click_callback(event->button);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *state, SDL_AppResult result)
{
    printf("SDL_AppQuit... (%s) (%d)\n", SDL_GetError(), result);
    AppState *as = (AppState*)state;

    SDL_GL_DestroyContext(as->gl_context);
}
