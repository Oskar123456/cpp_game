#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdint>
#include <cstdio>
#include <cmath>
#include <string>

#if defined(__linux__)
#include <time.h>
#endif

#define CGLM_USE_ANONYMOUS_STRUCT 1
#define CGLM_OMIT_NS_FROM_STRUCT_API 1
#include "../include/cglm/include/cglm/struct.h"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

#endif
