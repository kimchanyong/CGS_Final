#ifndef __khrplatform_h_
#define __khrplatform_h_

/*
   최소한 GLEW가 필요로 하는 타입만 정의해 둔
   간단 버전 khrplatform.h 입니다.
   (Windows / Visual Studio 기준)
*/

typedef signed   char         khronos_int8_t;
typedef unsigned char         khronos_uint8_t;
typedef signed   short        khronos_int16_t;
typedef unsigned short        khronos_uint16_t;
typedef signed   int          khronos_int32_t;
typedef unsigned int          khronos_uint32_t;

#if defined(_MSC_VER)
/* MSVC 에서는 long long 이 64비트 */
typedef signed   long long    khronos_int64_t;
typedef unsigned long long    khronos_uint64_t;
#else
typedef signed   long long    khronos_int64_t;
typedef unsigned long long    khronos_uint64_t;
#endif

typedef signed   long long    khronos_intptr_t;
typedef unsigned long long    khronos_uintptr_t;
typedef signed   long long    khronos_ssize_t;
typedef unsigned long long    khronos_usize_t;

typedef float                 khronos_float_t;
typedef signed   int          khronos_intptr_t_gl;
typedef unsigned int          khronos_uintptr_t_gl;

/* OpenGL enum 에서 쓰는 불리언 타입 */
typedef unsigned int          khronos_boolean_enum_t;

#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#endif /* __khrplatform_h_ */
