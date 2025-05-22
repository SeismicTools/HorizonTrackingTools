#ifndef COMMON_H
#define COMMON_H
#define PLATFORM_UNIX 0
#define PLATFORM_WIN32 1
#define PLATFORM_APPLE 2
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
# define PLATFORM PLATFORM_WIN32
#elif defined( __APPLE_CC__ )
# define PLATFORM PLATFORM_APPLE
#else
# define PLATFORM PLATFORM_UNIX
#endif
#define COMPILER_GNU 0
#define COMPILER_MICROSOFT 1
#ifdef _MSC_VER
# define COMPILER COMPILER_MICROSOFT
#elif defined( __GNUC__ )
# define COMPILER COMPILER_GNU
#else
# pragma error "FATAL ERROR: Unknown compiler."
#endif
#if COMPILER != COMPILER_GNU
// instead _CRT_SECURE_NO_WARNINGS
// see http://msdn.microsoft.com/en-us/library/8ef0s5kh%28VS.80%29.aspx
#pragma warning(disable: 4996)
typedef signed __int64 int64;
typedef signed __int32 int32;
typedef signed __int16 int16;
typedef signed __int8 int8;
typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <stdint.h>
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef uint32_t DWORD;
#endif

#ifdef __cplusplus
#include <limits>
#endif // __cplusplus

#endif // COMMON_H
