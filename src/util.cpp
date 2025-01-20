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
#include "../include/glad/glad.h"

u32 util_shader_load(const char* path_vs, const char* path_fs)
{
    u32 sp;
    u32 vs = glCreateShader(GL_VERTEX_SHADER);
    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);

    FILE *vs_f = fopen(path_vs, "r");
    FILE *fs_f = fopen(path_fs, "r");

    if (!vs_f || !fs_f) {
        printf("util_shader_load: err opening files\n");
    }

    fseek(vs_f, 0, SEEK_END);
    u32 vs_f_sz = ftell(vs_f);
    fseek(vs_f, 0, SEEK_SET);

    fseek(fs_f, 0, SEEK_END);
    u32 fs_f_sz = ftell(fs_f);
    fseek(fs_f, 0, SEEK_SET);

    char *vs_buf = (char*)malloc(vs_f_sz);
    char *fs_buf = (char*)malloc(fs_f_sz);

    fread(vs_buf, 1, vs_f_sz, vs_f);
    fread(fs_buf, 1, fs_f_sz, fs_f);

    glShaderSource(vs, 1, &vs_buf, NULL);
    glShaderSource(fs, 1, &fs_buf, NULL);

    glCompileShader(vs);
    glCompileShader(fs);

    int err;
    char err_buf[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &err);
    if (err) {
        glGetShaderInfoLog(vs, 512, NULL, err_buf);
        printf("%s\n", err_buf);
    }
    glGetShaderiv(fs, GL_COMPILE_STATUS, &err);
    if (err) {
        glGetShaderInfoLog(fs, 512, NULL, err_buf);
        printf("%s\n", err_buf);
    }

    sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    glGetProgramiv(sp, GL_LINK_STATUS, &err);
    if (err) {
        glGetShaderInfoLog(fs, 512, NULL, err_buf);
        printf("%s\n", err_buf);
    }

    fclose(vs_f);
    fclose(fs_f);
    free(vs_buf);
    free(fs_buf);
    glDeleteShader(vs);
    glDeleteShader(fs);

    printf("succesfully loaded shaders %s, %s in %d\n", path_vs, path_fs, sp);

    return sp;
}
