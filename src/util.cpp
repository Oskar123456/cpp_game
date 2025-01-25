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

#include "util.h"
#include "glad/glad.h"
#include "logging.h"

u32 util_shader_load(const char* path_vs, const char* path_fs)
{
    u32 sp;

    FILE *vs_f = fopen(path_vs, "r");
    FILE *fs_f = fopen(path_fs, "r");

    if (!vs_f || !fs_f) {
        LOG_ERROR(("util_shader_load: err opening files"));
    }

    fseek(vs_f, 0, SEEK_END);
    u32 vs_f_sz = ftell(vs_f);
    fseek(vs_f, 0, SEEK_SET);

    fseek(fs_f, 0, SEEK_END);
    u32 fs_f_sz = ftell(fs_f);
    fseek(fs_f, 0, SEEK_SET);

    u32 vs = glCreateShader(GL_VERTEX_SHADER);
    char *vs_buf = (char*)calloc(vs_f_sz + 1, 1);
    fread(vs_buf, 1, vs_f_sz, vs_f);
    glShaderSource(vs, 1, &vs_buf, NULL);
    glCompileShader(vs);
    int comp_status;
    char comp_status_buf[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &comp_status);
    if (!comp_status) {
        glGetShaderInfoLog(vs, 512, NULL, comp_status_buf);
        LOG_ERROR("err compiling vertex shader: %s", comp_status_buf);
    }

    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
    char *fs_buf = (char*)calloc(fs_f_sz + 1, 1);
    fread(fs_buf, 1, fs_f_sz, fs_f);
    glShaderSource(fs, 1, &fs_buf, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &comp_status);
    if (!comp_status) {
        glGetShaderInfoLog(fs, 512, NULL, comp_status_buf);
        LOG_ERROR("err compiling fragment shader: %s", comp_status_buf);
    }

    sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    glGetProgramiv(sp, GL_LINK_STATUS, &comp_status);
    if (!comp_status) {
        glGetShaderInfoLog(fs, 512, NULL, comp_status_buf);
        LOG_ERROR("err linking shader program: %s", comp_status_buf);
    }

    fclose(vs_f);
    fclose(fs_f);
    free(vs_buf);
    free(fs_buf);
    glDeleteShader(vs);
    glDeleteShader(fs);

    LOG_SUCCESS("succesfully loaded shaders %s, %s in %d", path_vs, path_fs, sp);

    return sp;
}
