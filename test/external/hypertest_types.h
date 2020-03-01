#ifndef HYPERTEST_TYPES_H
#define HYPERTEST_TYPES_H
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>
typedef float           r32;
typedef double          r64;
typedef unsigned char   ubyte;
typedef unsigned int    uint;
typedef int8_t          i8;
typedef uint8_t         u8;
typedef int16_t         i16;
typedef uint16_t        u16;
typedef int32_t         i32;
typedef uint32_t        u32;
typedef int64_t         i64;
typedef uint64_t        u64;
typedef i32             b32;

typedef i8              s8;
typedef i16             s16;
typedef i32             s32;
typedef i64             s64;

typedef uintptr_t       umm;
typedef intptr_t        smm;
typedef size_t          memory_index;

#define U16MAX 65535
#define I32MIN ((i32)0x80000000)
#define I32MAX ((i32)0x7fffffff)
#define U32MIN 0
#define U32MAX ((u32)-1)
#define U64MAX ((u64)-1)
#define R32MAX FLT_MAX
#define R32MIN -FLT_MAX

#define OffsetOf(type, Member) (umm)&(((type *)0)->Member)

#define OM_PI32 3.14159265359f
#define OM_TAU32 6.28318530717958647692f

#define OM_KILOBYTES(Value) ((Value)*1024)
#define OM_MEGABYTES(Value) (OM_KILOBYTES(Value)*1024)
#define OM_GIGABYTES(Value) (OM_MEGABYTES(Value)*1024)
#define OM_TERABYTES(Value) (OM_GIGABYTES(Value)*1024)

// TODO: Add OM_DEBUG as a compiler flag.
#if 0 //OM_DEBUG
#define OM_ASSERT(Expression) if(!(Expression)) {*(volatile int *)0 = 0;}
#else
#define OM_ASSERT(Expression)
#endif

#define InvalidCodePath OM_ASSERT(!"InvalidCodePath")
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define OM_INTERNAL                     static
#define OM_LOCAL_PERSIST                static
#define OM_GLOBAL_VARIABLE              static

#define OM_ARRAYCOUNT(Array) (sizeof(Array) / sizeof((Array)[0]))

#define OM_MIN(a, b) ((a) < (b) ? (a) : (b))
#define OM_MAX(a, b) ((a) > (b) ? (a) : (b))


inline void
UNITTEST_ZeroSize(memory_index Size, void *Pointer)
{
	//TODO: Performance check
	u8 *Byte = (u8 *)Pointer;
	while (Size--)
	{
		*Byte++ = 0;
	}
}

#define UNITTEST_ZeroStruct(Instance) UNITTEST_ZeroSize(sizeof(Instance), &(Instance))

#endif // HYPERTEST_TYPES_H