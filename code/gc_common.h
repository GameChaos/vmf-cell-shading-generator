/* date = October 12th 2020 6:33 pm */

#include <stdint.h>
#include <float.h>

#ifndef GC_COMMON_H
#define GC_COMMON_H

#define internal static
#define local_persist static
#define global static

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;
typedef s16 b16;
typedef s8 b8;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define S8_MIN 0xff
#define S16_MIN 0xffff
#define S32_MIN 0xffffffff
#define S64_MIN 0xffffffffffffffffLL

#define U8_MAX 0xffU
#define S8_MAX 0x7f
#define U16_MAX 0xffffU
#define S16_MAX 0x7fff
#define U32_MAX 0xffffffffU
#define S32_MAX 0x7fffffff
#define U64_MAX 0xffffffffffffffffULL
#define S64_MAX 0x7fffffffffffffffLL

#define F32_PI 3.14159265358979323846f
#define F64_PI 3.14159265358979323846

#define F32_MAX FLT_MAX
#define F64_MAX DBL_MAX

#define KILOBYTES(value) ((value) * 1024)
#define MEGABYTES(value) (KILOBYTES(value) * 1024)
#define GIGABYTES(value) (MEGABYTES(value) * 1024)
#define TERABYTES(value) (GIGABYTES(value) * 1024)

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))

#if GC_DEBUG
#define ASSERT(expression) if (!(expression)) {*(s32 *)0 = 0;}
#else
#define ASSERT(expression)
#endif

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define CLAMP(min, val, max) (MAX(MIN(val, max), min))
#define ROUND_UP_BY_POWER_OF_2(value, powerOf2) (((value) + (powerOf2) - 1) & ~((powerOf2) - 1))
#define ROUND_DOWN_BY_POWER_OF_2(value, powerOf2) ((value) & ~((powerOf2) - 1))

#define MEMBER(type, member) (((type *)0)->member)
#define OFFSETOF(type, member) ((u64)&MEMBER(type, member))

#endif //GC_COMMON_H
