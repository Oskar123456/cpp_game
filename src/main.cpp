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
#include <util.hpp>
#include <world.hpp>
#include <twod.hpp>
#include <logging.hpp>

#include <map>
#include <set>

#define WINDOW_WIDTH   1000
#define WINDOW_HEIGHT  800

using namespace std;

/* TEMP */
/* TEMP */
/* TEMP */
static int scr_w, scr_h;
static SDL_Time t_last_update, t_now, t_delta;

static SDL_Time t_hist[60];
static int t_hist_idx;

static vec3s circ = {400, 400, 100};
static vec4s rect = {100, 100, 100, 100};
static float rect_angle = M_PI / 4;

static bool paused;

static bool exit_first = true;

extern char **environ;

static u64 gol_updates;
set<vec2s> cells;
bool operator<(const vec2s& a, const vec2s& b) {return tie(a.x, a.y) < tie(b.x, b.y);}

set<vec2s> gol_next_gen(set<vec2s> prev_gen)
{
    set<vec2s> next_gen;
    for (auto c : prev_gen) {
        int neighbors = 0;
        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) {
                if (!i && !j)
                    continue;
                vec2s cn = {c.x + j, c.y + i};
                if (prev_gen.count({cn.x, cn.y})) {
                    neighbors++;
                }
                else {
                    int cn_neighbors = 0;
                    for (int k = -1; k < 2; ++k) {
                        for (int l = -1; l < 2; ++l) {
                            if (!k && !l)
                                continue;
                            if (prev_gen.count({cn.x + l, cn.y + k})) {
                                cn_neighbors++;
                            }
                        }
                    }
                    if (cn_neighbors == 3) {
                        next_gen.emplace(cn);
                    }
                }
            }
        }
        if (neighbors >= 2 && neighbors <= 3) {
            next_gen.emplace(c);
        }
    }
    return next_gen;
}

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

void mouse_poll()
{
    float x, y;
    SDL_MouseButtonFlags mf = SDL_GetMouseState(&x, &y);
    if (mf & SDL_BUTTON_LEFT) {
        int cx = x / 10;
        int cy = y / 10;
        vec2s cc = {(float)cx, (float)cy};
        if (!cells.count(cc)) {
            cells.emplace(cc);
        }
    }
}

void key_poll()
{
    int n;
    const bool *kcs = SDL_GetKeyboardState(&n);

    int ms = 4;
    if (kcs[SDL_SCANCODE_UP]) {
        rect.y = fmax(INT32_MIN, rect.y - ms);
    }
    if (kcs[SDL_SCANCODE_DOWN]) {
        rect.y = fmax(INT32_MIN, rect.y + ms);
    }
    if (kcs[SDL_SCANCODE_LEFT]) {
        rect.x = fmax(INT32_MIN, rect.x - ms);
    }
    if (kcs[SDL_SCANCODE_RIGHT]) {
        rect.x = fmax(INT32_MIN, rect.x + ms);
    }
    if (kcs[SDL_SCANCODE_R]) {
        rect_angle = rect_angle + 0.10;
    }
    if (kcs[SDL_SCANCODE_W]) {
        circ.y = fmax(0, circ.y - ms);
    }
    if (kcs[SDL_SCANCODE_S]) {
        circ.y = fmax(0, circ.y + ms);
    }
    if (kcs[SDL_SCANCODE_A]) {
        circ.x = fmax(0, circ.x - ms);
    }
    if (kcs[SDL_SCANCODE_D]) {
        circ.x = fmax(0, circ.x + ms);
    }
    if (kcs[SDL_SCANCODE_R]) {
        rect_angle = rect_angle + 0.10;
    }
}

SDL_AppResult key_callback(SDL_KeyboardEvent kb_event)
{
    if (kb_event.scancode != SDL_SCANCODE_ESCAPE)
        exit_first = false;
    switch (kb_event.scancode) {
        case SDL_SCANCODE_F11:
            SDL_SetWindowFullscreen(app_state.window, !(SDL_GetWindowFlags(app_state.window) & SDL_WINDOW_FULLSCREEN));
            break;
        case SDL_SCANCODE_ESCAPE:
            printf("esc down\n");
            if (!exit_first)
                return SDL_APP_SUCCESS;
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

SDL_AppResult SDL_AppInit(void **state, int argc, char *argv[])
{
    /* begin */
    *state = (void*)&app_state;
    AppState *as = (AppState*)*state;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_GetCurrentTime(&t_last_update);

    SDL_GetWindowSizeInPixels(as->window, &scr_w, &scr_h);
    twod_update_scr_dims(scr_w, scr_h);
    twod_init();

    cells.emplace((vec2s){11, 11});
    cells.emplace((vec2s){11, 12});
    cells.emplace((vec2s){11, 13});
    cells.emplace((vec2s){12, 11});
    cells.emplace((vec2s){10, 12});

    /* text */
    /* testing */
    /* twod_create_tex_a("font_text", "gol"); */
    twod_create_tex("textures/minecraft_dirt_pure.jpg", "dirt");
    twod_create_tex("textures/minecraft_snow_top.jpg", "snow");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *state)
{
    AppState *as = (AppState*)state;

    SDL_GetCurrentTime(&t_now);
    t_delta = t_now - t_last_update;
    t_last_update = t_now;
    t_hist[t_hist_idx++ % 60] = t_delta;
    SDL_Time t_avg = 0;
    for (int i = 0; i < 60; ++i)
        t_avg += t_hist[i];
    t_avg /= 60;

    key_poll();
    mouse_poll();

    /* printf("ft: %lums (avg. ft: %lums)\n", t_delta / 1000000, t_avg / 1000000); */

    Color bg = COL_TOKYO;
    glClearColor(VEC4EXP(bg));
    glClear(GL_COLOR_BUFFER_BIT);

    /* twod_draw_circlef(circ.x, circ.y, circ.z, COL_PINK); */
    twod_draw_rectf_tex_rot(rect.x, rect.y, rect.z, rect.w, COL_WHITE, "dirt", rect_angle);
    /* twod_draw_rectf_tex(rect.x, rect.y, rect.z, rect.w, COL_WHITE, "gol", rect_angle); */
    for (auto c : cells) {
        twod_draw_rectf(c.x * 10, c.y * 10, 10, 10, COL_BLACK);
        /* twod_draw_rectf(c.x * 10 + 1, c.y * 10 + 1, 8, 8, COL_WHITE); */
        twod_draw_rectf_tex_rot(c.x * 10 + 1, c.y * 10 + 1, 8, 8, COL_WHITE, "snow", 0);
    }

    /* twod_draw_text("game of life", rect.x, rect.y, 10, COL_BLACK, 0); */
    const char* str = "testing twod_draw_text... :)";
    float str_text_pad = 20;
    float str_text_len = twod_get_text_length(str, strlen(str), 0.5);
    twod_draw_text(str, strlen(str), scr_w - str_text_len - str_text_pad, str_text_pad, 0.5, COL_WHITE, rect_angle - M_PI / 2);

    if (!paused)
    {
        if (++gol_updates % 5 == 0) {
            cells = gol_next_gen(cells);
        }
    }

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
        glViewport(0, 0, scr_w, scr_h);
        /* printf("%d %d\n", w, h); */
    }

    if (event->type == SDL_EVENT_KEY_DOWN)
        if ((res = key_callback(event->key)))
            return res;

    /* if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) */
    /*     mouseclick_callback(event->key) */

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *state, SDL_AppResult result)
{
    printf("SDL_AppQuit... (%d)\n", result);
    AppState *as = (AppState*)state;

    SDL_GL_DestroyContext(as->gl_context);
}
