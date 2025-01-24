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

#include "logging.hpp"
#include <cstdio>
#include <ctime>
#include <cstdarg>

enum LOG_COLOR {
    LOG_COLOR_RED    = 31,
    LOG_COLOR_GREEN  = 32,
    LOG_COLOR_YELLOW = 33,
    LOG_COLOR_BLUE   = 34
};

static LOG_LEVEL _log_level = LOG_LEVEL_INFO;
static FILE* _log_outf = stderr;

void log_set_level(LOG_LEVEL lim)
{
    _log_level = lim;
}

void log_set_outf(FILE* out)
{
    _log_outf = out;
}

void log_print_w_color(const char* s, int col)
{
    fprintf(_log_outf, "\033[%dm", col);
    fprintf(_log_outf, "%s", s);
    fprintf(_log_outf, "\033[0m");
}

void log_print_timestamp()
{
    time_t t = time(NULL);
    struct tm *t_tm = localtime(&t);
    char t_str[100];
    strftime(t_str, 100, "%y-%m-%d %H:%M:%S", t_tm);
    fprintf(_log_outf, "%s", t_str);
}

void log_error(const char* fmt, ...)
{
    fprintf(_log_outf, "(");
    log_print_timestamp();
    fprintf(_log_outf, ")");
    log_print_w_color(" [WARNING] ", LOG_COLOR_RED);
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\t");
}

void log_warn(const char* fmt, ...)
{
    if (_log_level > LOG_LEVEL_WARN) return;
    fprintf(_log_outf, "(");
    log_print_timestamp();
    fprintf(_log_outf, ")");
    log_print_w_color(" [WARNING] ", LOG_COLOR_YELLOW);
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\t");
}

void log_info(const char* fmt, ...)
{
    if (_log_level > LOG_LEVEL_INFO) return;
    fprintf(_log_outf, "(");
    log_print_timestamp();
    fprintf(_log_outf, ")");
    log_print_w_color(" [WARNING] ", LOG_COLOR_BLUE);
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\t");
}

void log_success(const char* fmt, ...)
{
    if (_log_level > LOG_LEVEL_SUCCESS) return;
    fprintf(_log_outf, "(");
    log_print_timestamp();
    fprintf(_log_outf, ")");
    log_print_w_color(" [WARNING] ", LOG_COLOR_GREEN);
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\t");
}

void log_out(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\n");
}

void log_error_out(const char* fmt, ...)
{
    if (_log_level > LOG_LEVEL_ERROR) return;
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\n");
}

void log_warn_out(const char* fmt, ...)
{
    if (_log_level > LOG_LEVEL_WARN) return;
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\n");
}

void log_success_out(const char* fmt, ...)
{
    if (_log_level > LOG_LEVEL_SUCCESS) return;
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\n");
}

void log_info_out(const char* fmt, ...)
{
    if (_log_level > LOG_LEVEL_INFO) return;
    va_list va;
    va_start(va, fmt);
    vfprintf(_log_outf, fmt, va);
    va_end(va);
    fprintf(_log_outf, "\n");
}
