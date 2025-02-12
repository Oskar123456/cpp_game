#ifndef WORLD_H
#define WORLD_H

#include <threed.h>
#include <logging.h>
#include <camera.h>

#include <tuple>
#include <vector>
#include <unordered_map>

#define CHUNK_SIZE 16

struct vec2i { union { struct { int x, y; }; struct { int i, j; }; }; };
struct vec3i { int x, y, z; };

template <>
struct std::hash<vec3i> {
    size_t operator()(const vec3i& v) const {
        return ((std::hash<int>{}(v.x)) << 1) ^ ((std::hash<int>{}(v.y)) << 1) ^ ((std::hash<int>{}(v.z)) << 1);
    }
};

template <>
struct std::hash<vec2i> {
    size_t operator()(const vec2i& v) const {
        return ((std::hash<int>{}(v.x)) << 1) ^ ((std::hash<int>{}(v.y)) << 1);
    }
};

enum Voxel { VOX_NONE, VOX_AIR, VOX_GRASS_DIRT, VOX_DIRT, VOX_GRASS_STONE, VOX_STONE, VOX_WATER, VOX_NUM };
enum Direction { DIR_UP,  DIR_DOWN,  DIR_LEFT,  DIR_RIGHT,  DIR_FORWARD,  DIR_BACKWARD, DIR_NUM };

struct Voxel_Data {
    union { Voxel type; Voxel id; };
    vec2i tex_offs[DIR_NUM];
    bool is_solid, is_opaque;
    u32 vao;
};

struct Chunk_Mesh {
    std::vector<float> vertices, vertices_water, vertices_collision;
    std::vector<u32> triangles, triangles_water, triangles_collision;
};

struct Chunk {
    union {
        struct { int x, y, z; };
        vec3i pos;
    };
    union {
        struct { int world_x, world_y, world_z; };
        vec3i world_pos;
    };
    Voxel vox[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    Chunk_Mesh mesh;
    u32 vao;
};

struct World_Settings {
    int seed, seed_x, seed_y, seed_z;
    int max_height = 20, min_height = -2, water_level = 0;
    float scale = 0.01f, lacunarity = 2, gain = 0.5f, octaves = 4;
};

struct World {
    World_Settings settings;
    std::unordered_map<vec3i, Chunk> chunks;
    std::unordered_map<vec3i, Chunk_Mesh> chunk_meshes;
};


bool operator<(const vec2i& a, const vec2i& b);
bool operator==(const vec2i& a, const vec2i& b);
vec2i operator+(const vec2i& a, const vec2i& b);

bool operator<(const vec3i& a, const vec3i& b);
bool operator==(const vec3i& a, const vec3i& b);
vec3i operator+(const vec3i& a, const vec3i& b);

void world_print_info();
void world_render_cube();
void world_render(World& world, Camera& cam);
void world_init();
void world_gen_chunk(World& world, vec3i pos);
void world_load_voxel_data(Voxel vox);
void world_gen_chunk_mesh(World& world, Chunk& chunk);
void world_regen_chunk(World& world, Chunk& chunk);

vec3i world_map_chunk_to_vox(vec3i pos);
vec3i world_map_to_chunki(vec3i pos);
vec3i world_map_to_chunkf(vec3s pos);
vec3i world_map_to_voxi(vec3i pos);
vec3i world_map_to_voxf(vec3s pos);
Voxel world_get_voxeli(World& world, vec3i pos);

vec3s world_dir(u8 dir);
vec3i world_dir_i(u8 dir);



#endif

