#ifndef LOGGING_H
#define LOGGING_H

#include <cstdarg>

enum LOG_LEVEL {
    LOG_LEVEL_INFO,
    LOG_LEVEL_SUCCESS,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};

void log_set_level(LOG_LEVEL l);
void log_warn(const char* fmt, ...);
void log_success(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_out(const char* fmt, ...);
void log_error_out(const char* fmt, ...);
void log_warn_out(const char* fmt, ...);
void log_info_out(const char* fmt, ...);
void log_success_out(const char* fmt, ...);

/*
 allow empty variadic args list:
     C++20 MACRO:   __VA_OPT__(,) __VA_ARGS__
     GNU CPP:     ##__VA_ARGS__
 */

#define LOG_WARN(fmt, ...) log_warn("%s|%d|:", __FILE__, __LINE__); log_warn_out(fmt, ##__VA_ARGS__)
#define LOG_SUCCESS(fmt, ...) log_success("%s|%d|:", __FILE__, __LINE__); log_success_out(fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_error("%s|%d|:", __FILE__, __LINE__); log_error_out(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) log_info("%s|%d|:", __FILE__, __LINE__); log_info_out(fmt, ##__VA_ARGS__)

#endif
