#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdint>
#include <cstdio>
#include <cmath>

#include <string>
#include <iostream>

#if defined(__linux__)
#include <time.h>
#endif

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

u32 util_shader_load(const char* path_vs, const char* path_fs);

#endif
