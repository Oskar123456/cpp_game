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

#include <vector>
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
static float tex_padding = 0.007f;
static Voxel_Data vox_data[VOX_NUM];
static float vox_padding = 0.01f;

static u32 shader;
static u32 shader_mvp;
static u32 shader_col;
static u32 shader_use_tex;

void world_gen_chunk(World& world, vec3i pos)
{
    Chunk chunk = { .pos = pos, .world_pos = {pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE} };
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            float noise = stb_perlin_fbm_noise3((pos.x * CHUNK_SIZE + x) * world.settings.scale + world.settings.scale, 0,
                    (pos.z * CHUNK_SIZE + z) * world.settings.scale + world.settings.scale,
                    world.settings.lacunarity, world.settings.gain, world.settings.octaves);
            /* printf("%f\n", noise); */
            noise = std::clamp(fabs(noise), 0.0f, 1.0f);
            /* printf("%f at (%d %d)\n", noise, chunk.pos.x * CHUNK_SIZE + x, chunk.pos.z * CHUNK_SIZE + z); */
            int h = (noise * (world.settings.max_height - world.settings.min_height)) + world.settings.min_height;
            for (int y = 0; y < CHUNK_SIZE; ++y) {
                int y_vox = y + chunk.y * CHUNK_SIZE;
                if (y_vox < h)
                    chunk.vox[x][y][z] = VOX_DIRT;
                else if (y_vox == h)
                    chunk.vox[x][y][z] = VOX_GRASS_DIRT;
                else if (y_vox > h) {
                    if (y_vox <= world.settings.water_level)
                        chunk.vox[x][y][z] = VOX_WATER;
                    else
                        chunk.vox[x][y][z] = VOX_AIR;
                }
                /* printf("create voxel (h:%d) (%s) at (%d %d %d)\n", h, voxel_names[chunk.vox[x][y][z]], chunk.pos.x * CHUNK_SIZE + x, chunk.pos.y * CHUNK_SIZE + y, chunk.pos.z * CHUNK_SIZE + z); */
            }
        }
    }
    world.chunks[chunk.pos] = chunk;
}

void world_render(World& world, Camera& cam)
{
    Color c = COL_WHITE;

    glUseProgram(shader);
    glBindTexture(GL_TEXTURE_2D, main_tex_id);
    glUniform4fv(shader_col, 1, (float*)&c);
    glUniform1i(shader_use_tex, true);

    for (auto [pos, chunk] : world.chunks) {
        float rotation = 0;
        vec3s rot_axis = {0, 1, 0};
        vec3s scale = {1, 1, 1};
        vec4s tint = {1, 1, 1, 1};
        vec3s chunk_pos_in_vox = {(float)pos.x * CHUNK_SIZE, (float)pos.y * CHUNK_SIZE, (float)pos.z * CHUNK_SIZE};

        render(cam, chunk.model, chunk_pos_in_vox, scale, rotation, rot_axis, tint, false);

        // mat4s mvp = GLMS_MAT4_IDENTITY;
        // mvp = mat4_mul(mvp, cam.view_proj);
        // mvp = glms_translate(mvp, chunk_pos_in_vox);

        // glUseProgram(shader);
        // glBindTexture(GL_TEXTURE_2D, main_tex_id);
        // glBindVertexArray(chunk.vao);
        // glUniformMatrix4fv(shader_mvp, 1, GL_FALSE, (float*)&mvp);
        // glUniform4fv(shader_col, 1, (float*)&c);
        // glUniform1i(shader_use_tex, true);
        // glDrawElements(GL_TRIANGLES, chunk.mesh.triangles.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);
    }
}

void world_gen_chunk_mesh(World& world, Chunk& chunk)
{
    Chunk_Mesh mesh;
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                Voxel type = chunk.vox[x][y][z];
                if (type == VOX_AIR || type == VOX_NONE)
                    continue;
                vec3i vox_pos = world_map_to_voxi({x, y, z}) + world_map_chunk_to_vox(chunk.pos);
                for (int dir = 0; dir < DIR_NUM; ++dir) {
                    vec3i vox_pos_n = vox_pos + world_dir_i(dir);
                    Voxel vox_n = world_get_voxeli(world, vox_pos_n);
                    Voxel_Data vox_data_n = vox_data[vox_n];
                    if (vox_data_n.is_opaque) {
                        continue;
                    }
                    std::vector<float> v(threed_get_quad_vertices(dir), threed_get_quad_vertices(dir) + 32);
                    std::vector<u32> t(threed_get_quad_indices(0), threed_get_quad_indices(0) + 6);

                    for (int i = 0; i < 6; ++i) {
                        t[i] += mesh.vertices.size() / 8;
                    }

                    for (int i = 0; i < 4; ++i) {
                        v[i * 8 + 0] += x;
                        v[i * 8 + 1] += y;
                        v[i * 8 + 2] += z;
                    }

                    v[3]  =  vox_data[type].tex_offs[dir].x       * tex_size + tex_padding;
                    v[4]  =  vox_data[type].tex_offs[dir].y       * tex_size + tex_padding;
                    v[11] =  vox_data[type].tex_offs[dir].x       * tex_size + tex_padding;
                    v[12] = (vox_data[type].tex_offs[dir].y + 1)  * tex_size - tex_padding;
                    v[19] = (vox_data[type].tex_offs[dir].x + 1)  * tex_size - tex_padding;
                    v[20] = (vox_data[type].tex_offs[dir].y + 1)  * tex_size - tex_padding;
                    v[27] = (vox_data[type].tex_offs[dir].x + 1)  * tex_size - tex_padding;
                    v[28] =  vox_data[type].tex_offs[dir].y       * tex_size + tex_padding;

                    mesh.vertices.insert(mesh.vertices.end(), v.begin(), v.end());
                    mesh.triangles.insert(mesh.triangles.end(), t.begin(), t.end());

                }
            }
        }
    }

    chunk.mesh = mesh;
    world.chunk_meshes[chunk.pos] = mesh;
}

void world_regen_chunk(World& world, Chunk& chunk)
{
    if (!world.chunk_meshes.count(chunk.pos)) {
        LOG_WARN("chunk mesh not found for (%d %d %d)", chunk.pos.x, chunk.pos.y, chunk.pos.z);
        return;
    }

    Chunk_Mesh mesh = chunk.mesh;

    /* printf("uploading %.2fkb of vertices (%ld triangles) for chunk at %d %d %d\n", */
    /*         (mesh.vertices.size() * sizeof(float) + mesh.triangles.size() * sizeof(float)) / 1000.0f, mesh.triangles.size() / 3, chunk.world_pos.x, chunk.world_pos.y, chunk.world_pos.z); */

    u32 vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.triangles.size() * sizeof(float), mesh.triangles.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    chunk.vao = vao;
    chunk.model.mesh = vao;
    chunk.model.vertices = mesh.vertices.size();
    chunk.model.triangles = mesh.triangles.size();
    chunk.model.texture = main_tex_id;
    chunk.model.material.ambient = {1, 1, 1};
    chunk.model.material.diffuse = {1, 1, 1};
    chunk.model.material.specular = {1, 1, 1};
    chunk.model.material.smoothness = 0.1f;
}

void world_init()
{
    main_tex_id = threed_create_tex("textures/spritesheet_tiles_10x10.png", "main_tex_id", GL_RGBA);

    shader = util_shader_load("shaders/threed_cube.vs", "shaders/threed_cube.fs");
    shader_mvp = glGetUniformLocation(shader, "mvp");
    shader_col = glGetUniformLocation(shader, "color");
    shader_use_tex = glGetUniformLocation(shader, "use_tex");

    vox_data[VOX_NONE].id = VOX_NONE;
    vox_data[VOX_NONE].is_opaque = false;

    vox_data[VOX_AIR].id = VOX_AIR;
    vox_data[VOX_AIR].is_opaque = false;

    vox_data[VOX_GRASS_DIRT].id = VOX_GRASS_DIRT;
    vox_data[VOX_GRASS_DIRT].is_opaque = true;
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_UP] = { 5, 8 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_DOWN] = { 7, 4 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_LEFT] = { 7, 5 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_RIGHT] = { 7, 5 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_FORWARD] = { 7, 5 };
    vox_data[VOX_GRASS_DIRT].tex_offs[DIR_BACKWARD] = { 7, 5 };

    vox_data[VOX_DIRT].id = VOX_DIRT;
    vox_data[VOX_DIRT].is_opaque = true;
    vox_data[VOX_DIRT].tex_offs[DIR_UP] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_DOWN] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_LEFT] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_RIGHT] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_FORWARD] = { 7, 4 };
    vox_data[VOX_DIRT].tex_offs[DIR_BACKWARD] = { 7, 4 };

    vox_data[VOX_WATER].id = VOX_WATER;
    vox_data[VOX_WATER].is_opaque = false;
    vox_data[VOX_WATER].tex_offs[DIR_UP] = { 0, 3 };
    vox_data[VOX_WATER].tex_offs[DIR_DOWN] = { 0, 3 };
    vox_data[VOX_WATER].tex_offs[DIR_LEFT] = { 0, 3 };
    vox_data[VOX_WATER].tex_offs[DIR_RIGHT] = { 0, 3 };
    vox_data[VOX_WATER].tex_offs[DIR_FORWARD] = { 0, 3 };
    vox_data[VOX_WATER].tex_offs[DIR_BACKWARD] = { 0, 3 };

    vox_data[VOX_GRASS_STONE].id = VOX_GRASS_STONE;
    vox_data[VOX_STONE].id = VOX_STONE;

    for (int i = 0; i < VOX_NUM; ++i) {
        world_load_voxel_data((Voxel)i);
    }

    Light sun = {
        .ambient = {1, 1, 1}, .diffuse = {1, 1, 1}, .specular = {1, 1, 1},
        .position = {0, 1000, 1000}, .direction {0, -1, -1}, .attenuation = 0
    };

    render_init();
    render_add_lights(&sun, 1);
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

Voxel world_get_voxeli(World& world, vec3i pos)
{
    vec3i chunk_pos = world_map_to_chunki(pos);
    vec3i vox_pos = world_map_to_voxi(pos);
    /* if (pos.x < 0 || pos.y < 0 || pos.z < 0) { */
    /*     printf("should be neg chunk\n"); */
    /*     if (chunk_pos.x < 0 || chunk_pos.y < 0 || chunk_pos.z < 0) { */
    /*         printf("looking for voxel at (%d %d %d) maps to (%d %d %d)\n", pos.x, pos.y, pos.z, chunk_pos.x, chunk_pos.y, chunk_pos.z); */
    /*     } */
    /* } */
    /* printf("looking for voxel at (%d %d %d) maps to (%d %d %d)\n", pos.x, pos.y, pos.z, chunk_pos.x, chunk_pos.y, chunk_pos.z); */
    if (!world.chunks.count(chunk_pos))
        return VOX_NONE;
    /* printf("found voxel %s (%d %d %d)\n", voxel_names[world.chunks[chunk_pos].vox[vox_pos.x][vox_pos.y][vox_pos.z]], pos.x, pos.y, pos.z); */
    return world.chunks[chunk_pos].vox[vox_pos.x][vox_pos.y][vox_pos.z];
}

vec3i world_map_chunk_to_vox(vec3i pos)
{
    return { pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, pos.z * CHUNK_SIZE };
}

vec3i world_map_to_chunki(vec3i pos)
{
    return {(int)floor((float)pos.x / CHUNK_SIZE), (int)floor((float)pos.y / CHUNK_SIZE), (int)floor((float)pos.z / CHUNK_SIZE)};
}

vec3i world_map_to_chunkf(vec3s pos)
{
    return {(int)floor(pos.x / CHUNK_SIZE), (int)floor(pos.y / CHUNK_SIZE), (int)floor(pos.z / CHUNK_SIZE)};
}

vec3i world_map_to_voxi(vec3i pos)
{
    return {(((pos.x) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE,
        (((pos.y) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE,
        (((pos.z) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE};
}

vec3i world_map_to_voxf(vec3s pos)
{
    return {(((int)floor(pos.x) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE,
        (((int)floor(pos.y) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE,
        (((int)floor(pos.z) % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE};
}

vec3s world_dir(u8 dir)
{
    static vec3s dirs[] = {
        {0,  1, 0},
        {0, -1, 0},
        {-1, 0, 0},
        {1,  0, 0},
        {0,  0, 1},
        {0,  0, -1}
    };
    return dirs[dir];
}

vec3i world_dir_i(u8 dir)
{
    static vec3i dirs[] = {
        {0,  1, 0},
        {0, -1, 0},
        {-1, 0, 0},
        {1,  0, 0},
        {0,  0, 1},
        {0,  0, -1}
    };
    return dirs[dir];
}

