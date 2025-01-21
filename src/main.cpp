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

#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600

using namespace std;

/* TEMP */
/* TEMP */
/* TEMP */
static SDL_Time t_last_update, t_now, t_delta;

static SDL_Time t_hist[60];
static int t_hist_idx;

static vec4s rect = {100, 100, 600, 400};
/* TEMP */
/* TEMP */
/* TEMP */

struct MetaData
{
    const char *key;
    const char *value;
};

struct AppState {
    SDL_Window *window;
    SDL_GLContext gl_context;
};

struct AppState app_state;

SDL_AppResult key_callback(SDL_KeyboardEvent kb_event)
{
    int ms = 15;
    switch (kb_event.scancode) {
        case SDL_SCANCODE_F11:
            SDL_SetWindowFullscreen(app_state.window, !(SDL_GetWindowFlags(app_state.window) & SDL_WINDOW_FULLSCREEN));
            break;
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_Q:
            return SDL_APP_CONTINUE;
            break;
        case SDL_SCANCODE_UP:
            rect.y = fmax(0, rect.y - ms);
            break;
        case SDL_SCANCODE_DOWN:
            rect.y = fmax(0, rect.y + ms);
            break;
        case SDL_SCANCODE_LEFT:
            rect.x = fmax(0, rect.x - ms);
            break;
        case SDL_SCANCODE_RIGHT:
            rect.x = fmax(0, rect.x + ms);
            break;
        case SDL_SCANCODE_P:
            glms_vec4_print(rect, stdout);
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
    *state = (void*)&app_state;
    AppState *as = (AppState*)*state;

    SDL_InitSubSystem(SDL_INIT_VIDEO);

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

    SDL_GetCurrentTime(&t_last_update);

    int w, h;
    SDL_GetWindowSizeInPixels(as->window, &w, &h);
    twod_update_scr_dims(w, h);
    twod_init();

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

    /* printf("ft: %lums (avg. ft: %lums)\n", t_delta / 1000000, t_avg / 1000000); */
    Color bg = COL_TOKYO;
    glClearColor(VEC4EXP(bg));
    glClear(GL_COLOR_BUFFER_BIT);

    twod_draw_rectf(rect.x, rect.y, rect.z, rect.w, COL_PINK);

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
        int w, h;
        SDL_GetWindowSizeInPixels(as->window, &w, &h);
        twod_update_scr_dims(w, h);
        glViewport(0, 0, w, h);
        /* printf("%d %d\n", w, h); */
    }

    if (event->type == SDL_EVENT_KEY_DOWN)
        if ((res = key_callback(event->key)))
            return res;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *state, SDL_AppResult result)
{
    printf("SDL_AppQuit... (%d)\n", result);
    AppState *as = (AppState*)state;

    SDL_GL_DestroyContext(as->gl_context);
}
