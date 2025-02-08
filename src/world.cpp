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

#include <world.h>

#include <algorithm>
#include <cmath>

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>
#include <glad/glad.h>

bool operator<(const vec2i& a, const vec2i& b) { return std::tie(a.x, a.y) < std::tie(b.x, b.y); }
bool operator==(const vec2i& a, const vec2i& b) { return a.x == b.x && a.y == b.y; }
vec2i operator+(const vec2i& a, const vec2i& b) { return {a.x + b.x, a.y + b.y}; }
bool operator<(const vec3i& a, const vec3i& b) { return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z); }
bool operator==(const vec3i& a, const vec3i& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
vec3i operator+(const vec3i& a, const vec3i& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }

const char* voxel_names[] = { "VOX_NONE", "VOX_AIR", "VOX_GRASS_DIRT", "VOX_DIRT", "VOX_GRASS_STONE", "VOX_STONE", "VOX_WATER", "VOX_NUM" };

static u32 main_tex_id;

static float tex_size = 0.1f;
static float tex_padding = 0.001f;
static Voxel_Data vox_data[VOX_NUM];
static float vox_padding = 0.001f;

void world_gen_chunk(World& world, vec3i pos)
{
    Chunk chunk = { .pos = pos };
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            float noise = stb_perlin_fbm_noise3((pos.x * CHUNK_SIZE + x) * world.settings.scale + world.settings.scale, 0,
                    (pos.z * CHUNK_SIZE + z) * world.settings.scale + world.settings.scale,
                    world.settings.lacunarity, world.settings.gain, world.settings.octaves);
            /* printf("%f\n", noise); */
            noise = std::clamp(fabs(noise), 0.0f, 1.0f);
            printf("%f at (%d %d)\n", noise, chunk.pos.x * CHUNK_SIZE + x, chunk.pos.z * CHUNK_SIZE + z);
            int h = (noise * (world.settings.max_height - world.settings.min_height)) + world.settings.min_height;
            for (int y = 0; y < CHUNK_SIZE; ++y) {
                int y_vox = y + chunk.y * CHUNK_SIZE;
                if (y_vox <= world.settings.water_level)
                    chunk.vox[x][y][z] = VOX_WATER;
                else if (y_vox > h)
                    chunk.vox[x][y][z] = VOX_AIR;
                else if (y_vox == h)
                    chunk.vox[x][y][z] = VOX_GRASS_DIRT;
                else if (y_vox < h)
                    chunk.vox[x][y][z] = VOX_DIRT;
                /* printf("create voxel (h:%d) (%s) at (%d %d %d)\n", h, voxel_names[chunk.vox[x][y][z]], chunk.pos.x * CHUNK_SIZE + x, chunk.pos.y * CHUNK_SIZE + y, chunk.pos.z * CHUNK_SIZE + z); */
            }
        }
    }
    world.chunks[chunk.pos] = chunk;
}

void world_render(World& world, Camera& cam)
{
    for (auto [pos, chunk] : world.chunks) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            for (int y = 0; y < CHUNK_SIZE; ++y) {
                for (int z = 0; z < CHUNK_SIZE; ++z) {
                    Voxel type = chunk.vox[x][y][z];
                    if (type == VOX_AIR || type == VOX_NONE)
                        continue;
                    vec3s rot = {0, 1, 0};
                    vec3i chunk_pos = world_map_chunk_to_vox(chunk.pos);
                    vec3s vox_pos = {(float)x + chunk_pos.x, (float)y + chunk_pos.y, (float)z + chunk_pos.z};
                    /* printf("rendering voxel at %f %f %f\n", pos.x, pos.y, pos.z); */
                    /* if (type == VOX_AIR) */
                    /*     threed_render_cube(cam.view_proj, pos, 0, rot, COL_WHITE, 0, 0); */
                    /* if (type == VOX_GRASS_DIRT) */
                    /*     threed_render_cube(cam.view_proj, pos, 0, rot, COL_GREEN, 0, 0); */
                    /* if (type == VOX_DIRT) */
                    /*     threed_render_cube(cam.view_proj, pos, 0, rot, COL_BROWN, 0, 0); */
                    /* if (type == VOX_WATER) */
                    /*     threed_render_cube(cam.view_proj, pos, 0, rot, COL_BLUE, 0, 0); */
                    threed_render_cube(cam.view_proj, vox_pos, 0, rot, COL_WHITE, vox_data[type].vao, main_tex_id);
                }
            }
        }
    }
}

void world_init()
{
    main_tex_id = threed_create_tex("textures/spritesheet_tiles_10x10.png", "main_tex_id", GL_RGBA);

    vox_data[VOX_NONE].id = VOX_NONE;

    vox_data[VOX_AIR].id = VOX_AIR;

    vox_data[VOX_GRASS_DIRT].id = VOX_GRASS_DIRT;
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_UP] = { 5, 8 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_DOWN] = { 7, 4 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_LEFT] = { 7, 5 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_RIGHT] = { 7, 5 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_FORWARD] = { 7, 5 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_BACKWARD] = { 7, 5 };

    vox_data[VOX_DIRT].id = VOX_DIRT;
    vox_data[VOX_DIRT].tex_offs[DIR_UP] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_DOWN] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_LEFT] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_RIGHT] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_FORWARD] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_BACKWARD] = { 7, 4 };

    vox_data[VOX_GRASS_STONE].id = VOX_GRASS_STONE;

    vox_data[VOX_STONE].id = VOX_STONE;

    vox_data[VOX_WATER].id = VOX_WATER;

    for (int i = 0; i < VOX_NUM; ++i) {
        world_load_voxel_data((Voxel)i);
    }
}

void world_load_voxel_data(Voxel vox)
{
    vec2s tex_offs[24];
    for (int i = 0; i < DIR_NUM; ++i) {
        tex_offs[i * 4 + 0].x = vox_data[vox].tex_offs[i].x * tex_size + tex_padding;
        tex_offs[i * 4 + 0].y = vox_data[vox].tex_offs[i].y * tex_size + tex_padding;
        tex_offs[i * 4 + 1].x = vox_data[vox].tex_offs[i].x * tex_size + tex_padding;
        tex_offs[i * 4 + 1].y = (vox_data[vox].tex_offs[i].y + 1) * tex_size - tex_padding;
        tex_offs[i * 4 + 2].x = (vox_data[vox].tex_offs[i].x + 1) * tex_size - tex_padding;
        tex_offs[i * 4 + 2].y = (vox_data[vox].tex_offs[i].y + 1) * tex_size - tex_padding;
        tex_offs[i * 4 + 3].x = (vox_data[vox].tex_offs[i].x + 1) * tex_size - tex_padding;
        tex_offs[i * 4 + 3].y = vox_data[vox].tex_offs[i].y * tex_size + tex_padding;
    }
    /* for (int i = 0; i < 24; ++i) { */
    /*     glms_vec2_print(tex_offs[i], stdout); */
    /* } */
    vox_data[vox].vao = threed_create_cube_vao(tex_offs);
}

void world_print_info() { }

vec3i world_map_chunk_to_vox(vec3i pos)
{
    return { pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE };
}

vec3i world_map_to_chunk(vec3s pos)
{
    return {(int)floor(pos.x / CHUNK_SIZE), (int)floor(pos.y / CHUNK_SIZE), (int)floor(pos.z / CHUNK_SIZE)};
}

vec3i world_map_to_vox(vec3s pos)
{
    return {(((int)floor(pos.x) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE,
        (((int)floor(pos.y) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE,
        (((int)floor(pos.z) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE};
}





























