#if !defined(NLIB_H_INCLUDE_GUARD)

/* ****************************************************************************
 * COMMON *********************************************************************
 */

/* ===============
 * Platform Identification
 */

# if defined(_MSC_VER)
#  if defined(__clang__)
#   define COMPILER_CLANG
#   define COMPILER_CLANG_WITH_MSVC
#  else
#   define COMPILER_MSVC
#  endif
# elif defined(__clang__)
#  define COMPILER_CLANG
# else
#  error Compiler not supported
# endif

# if defined(_WIN32)
#  define OS_WINDOWS
# elif defined(__linux__)
#  define OS_LINUX
# else
#  error Operating system not supported
# endif

# if defined(COMPILER_MSVC) || defined(COMPILER_CLANG_WITH_MSVC)
#  if defined(_M_IX86)
#   define ARCH_X86
#  elif defined(_M_X64)
#   define ARCH_X64
#  endif
# elif defined(COMPILER_CLANG)
#  if defined(__i386__)
#   define ARCH_X86
#  elif defined(__x86_64__)
#   define ARCH_X64
#  endif
# endif

# if !defined(ARCH_X86) && !defined(ARCH_X64)
#  error Architecture not supported
# endif


/* ===========================
 * Standard C Headers Includes
 */

# if defined(COMPILER_MSVC)
#  pragma warning(push)
#   pragma warning(disable:4668)
# endif

# include <stddef.h>

# if defined(COMPILER_MSVC)
#  pragma warning(pop)
# endif

# include <stdint.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>

/* ===========================
 * Misc C Headers Includes
 */

// stb_sprintf
# define STB_SPRINTF_IMPLEMENTATION
# define STB_SPRINTF_STATIC

# if defined(COMPILER_MSVC)
#  pragma warning(push)
#   pragma warning(disable:4100)
#   pragma warning(disable:4820)
#    include "stb/stb_sprintf.h"
#  pragma warning(pop)
# endif

# if defined(COMPILER_CLANG)
#  pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wlanguage-extension-token"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wconditional-uninitialized"
#   pragma clang diagnostic ignored "-Wextra-semi-stmt"
#   pragma clang diagnostic ignored "-Wpadded"
#    include "stb/stb_sprintf.h"
#  pragma clang diagnostic pop
# endif

/* ===========================
 * Platform Headers Includes
 */

# if defined(OS_WINDOWS)

#  if defined(COMPILER_MSVC)
#   pragma warning(push)
#    pragma warning(disable:4255)
#    pragma warning(disable:4668)
#  endif

#  include <Windows.h>

#  if defined(COMPILER_MSVC)
#   pragma warning(pop)
#  endif

#  if defined(COMPILER_MSVC)
#   pragma warning(push)
#    pragma warning(disable:4820)
#    pragma warning(disable:4668)
#    pragma warning(disable:4255)
#  endif

#  include <intrin.h>

#  if defined(COMPILER_MSVC)
#   pragma warning(pop)
#  endif

# elif defined(OS_LINUX)
// ...

# endif

/* ===============
 * Primitive Types
 */

typedef int8_t               S8;
typedef int16_t              S16;
typedef int32_t              S32;
typedef int64_t              S64;
typedef int                  Sint;

typedef uint8_t              U8;
typedef uint16_t             U16;
typedef uint32_t             U32;
typedef uint64_t             U64;
typedef unsigned             Uint;

typedef size_t               Size;

typedef uintptr_t            Uptr;
typedef intptr_t             Sptr;
typedef ptrdiff_t            Dptr;

typedef float                F32;
typedef double               F64;

typedef U8                   B8;
typedef U16                  B16;
typedef U32                  B32;
typedef U64                  B64;
# define true                1U
# define false               0U

typedef unsigned char        Byte;
typedef char                 Char;

/* ========================
 * Preprocessor Definitions
 */

# define elemin(array) (sizeof(array)/sizeof((array)[0]))

# if !defined(max)
#  define max(x, y) ((x) >= (y) ? (x) : (y))
# endif

# if !defined(min)
# define min(x, y) ((x) <= (y) ? (x) : (y))
# endif

# define KiB(x) (   (x) * 1024ULL)
# define MiB(x) (KiB(x) * 1024ULL)
# define GiB(x) (MiB(x) * 1024ULL)
# define TiB(x) (GiB(x) * 1024ULL)

# define THOUSAND 1000L
# define MILLION  1000000L
# define BILLION  1000000000L

# define unused_variable(var) (void)var

# define global_variable   static
# define persistent_value  static

# define internal_function static
# define header_function   static inline

/* =======================
 * Compiler Specific Hacks
 */

/* Microsoft Visual C */
# if defined(COMPILER_MSVC)

// #  if _MSC_VER != 1916 // Visual Studio 2017 Version 15.7
// #   error MSVC version not supported
// #  endif // MSVC Versions

#  define _Alignof __alignof
#  define alignof _Alignof
#  define __alignof_is_defined 1

/* In alignas(a), 'a' should be a power of two that is at least the type's
   alignment and at most the implementation's alignment limit.  This limit is
   2**13 on MSVC. To be portable to MSVC through at least version 10.0,
   'a' should be an integer constant, as MSVC does not support expressions
   such as 1 << 3.

   The following C11 requirements are not supported here:

   - If 'a' is zero, alignas has no effect.
   - alignas can be used multiple times; the strictest one wins.
   - alignas (TYPE) is equivalent to alignas (alignof (TYPE)).
*/

#  define _Alignas(a) __declspec(align(a))
#  define alignas _Alignas
#  define __alignas_is_defined 1

/* Malloc Alignment: https://msdn.microsoft.com/en-us/library/ycsb6wwf.aspx
 */
#  if defined(ARCH_x86)
// Alignment is 8 bytes
typedef union {
    alignas(8) Byte alignment[8];
    F64 a;
} max_align_t;
#  elif defined(ARCH_X64)
// Alignment is 16 bytes
typedef union {
    alignas(16) Byte alignment[16];
    alignas(16) struct { F64 a, b; } f;
} max_align_t;
#  endif

# elif defined(COMPILER_CLANG)

#  include <stdalign.h>

# endif


/* =======================
 * Other nlib libraries
 */

# include "unicode.h"


/* =======================
 * OS Specific Hacks
 */

# if defined(OS_WINDOWS)

header_function
void reportDebugV(Char *format, va_list ap)
{
    Char buffer[2048] = {0};

    stbsp_vsnprintf(buffer, 2048, format, ap);
    buffer[2047] = '\0';

    LPWSTR wcstr = unicodeWin32UTF16FromUTF8(buffer);
    OutputDebugStringW(wcstr);
    unicodeWin32UTF16Dealloc(wcstr);

    return;
}

header_function
void reportDebug(Char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    reportDebugV(format, ap);
    va_end(ap);

    return;
}


#  if defined(BUILD_INTERNAL)
#   define report(...)  reportDebug(__VA_ARGS__)
#   define reportv(...) reportDebugV(__VA_ARGS__)
#   define breakpoint() __debugbreak()
#   define quit() breakpoint()
#  else
#   define report(...)
#   define reportv(...)
#   define quit() abort()
#  endif

/* _BitScanReverse64(&r, x) scans for the first 1-bit from left in x. Once it finds it,
 * it returns the number of bits after the found 1-bit.
 *
 * If b is the bit-width of the number,
 *    p is the closest lower power of two and
 *    r is the number of bits to the right of the first 1-bit when seen from left; then
 * then a number between 2^p and 2^(p+1) has the form: (b-p-1 0-bits) 1 (p bits)
 *
 * => r = p
 *
 * Thus, the rounded-down log2 of the number is r.
 */
header_function
U64 u64Log2(U64 x)
{
    unsigned long result = 0;
    _BitScanReverse64(&result, x);
    return result;
}

/* Linear Congruential Generator
 *
 * If x is the last random number,
 *    m is a number greater than zero that is a power of two,
 *    a is a number between 0 and m,
 * then the next random number is ((x * a) % m).
 *
 * Unfortunately, the lower bits don't have enought randomness in them. The LSB doesn't
 * change at all, the second LSB alternates, the one after that toggles every 2 turns and so
 * on. Therefore, we try to get rid of the LSBs by pulling in some MSBs.
 *
 * NOTE(naman): Seed should be an odd number or the randomness might drop drastically.
 * NOTE(naman): "a" should be equal to 5(mod 8) or 3(mod 8).
 */

header_function
U64 u64Rand (U64 seed)
{
    U64 previous = seed;
    if (previous == 0) {
        // This seed has been tested and should be preferred in normal circumstances.
        previous = 2531011ULL;
    }

    U64 a = 214013ULL;

    U64 upper = 0, lower = 0;
    lower = _umul128(previous, a, &upper);
    U64 log_upper = u64Log2(upper);
    U64 shift_amount = 64 - (log_upper + 1);
    upper = (upper << shift_amount) | (lower >> log_upper);

    U64 result = upper * a;

    return result;
}

# elif defined(OS_LINUX)

#  if defined(BUILD_INTERNAL)
#   define report(format, ...)      fprintf(stderr, format, ##__VA_ARGS__)
#   define reportv(format, va_list) vfprintf(stderr, format, va_list)
#   define breakpoint() __asm__ volatile("int $0x03")
#   define quit() breakpoint()
#  else
#   define report(format, ...)
#   define reportv(format, va_list)
#   define quit() abort()
#  endif

/* __builtin_clzll(x) returns the leading number of 0-bits in x, starting from
 * most significant position.
 *
 * If b is the bit-width of the number,
 *    p is the closest lower power of two and
 *    lz is the number of leading 0-bits; then
 * then a number between 2^p and 2^(p+1) has the form: (b-p-1 0-bits) 1 (p bits)
 *
 * => lz = b-p-1
 * => p = b-(lz+1)
 *
 * Thus, the rounded-down log2 of the number is b-(lz+1).
 */
header_function
U64 u64Log2(U64 x)
{
    U64 result = 64ULL - ((U64)__builtin_clzll(x) + 1ULL);
    return result;
}

/* Linear Congruential Generator
 *
 * If x is the last random number,
 *    m is a number greater than zero that is a power of two,
 *    a is a number between 0 and m,
 * then the next random number is ((x * a) % m).
 *
 * Unfortunately, the lower bits don't have enought randomness in them. The LSB doesn't
 * change at all, the second LSB alternates, the one after that toggles every 2 turns and so
 * on. Therefore, we try to get rid of the LSBs by pulling in some MSBs.
 *
 * We do the multiplcation twice because Chi-Square Test indicated that this method
 * gives better randomness. Don't ask.
 *
 * NOTE(naman): Seed should be an odd number or the randomness might drop drastically.
 */

header_function
U64 u64Rand (U64 seed)
{
    U64 previous = seed;
    if (previous == 0) {
        // This seed has been tested and should be preferred in normal circumstances.
        previous = 2531011ULL;
    }

    U64 a = 214013ULL;

    __uint128_t product = (__uint128_t)previous * (__uint128_t)a;
    U64 upper = product >> 64, lower = (U64)product;
    U64 log_upper = u64Log2(upper);
    U64 shift_amount = 64 - (log_upper + 1);
    upper = (upper << shift_amount) | (lower >> log_upper);

    U64 result = upper * a;

    return result;
}

# endif

/* ===============================
 * Custom Memory Allocator Helpers
 */

#define MEM_MAX_ALIGN_MINUS_ONE (alignof(max_align_t) - 1u)
#define memAlignUp(p) (((p) + MEM_MAX_ALIGN_MINUS_ONE) & (~ MEM_MAX_ALIGN_MINUS_ONE))
#define memAlignDown(p) (memAlignUp((p) - MEM_MAX_ALIGN_MINUS_ONE))


/* ****************************************************************************
 * LIBRARIES ******************************************************************
 */

/* ==============
 * Claim (assert)
 */

# define claim(cond) claim_(cond, #cond, __FILE__, __LINE__)

header_function
void claim_ (B32 cond,
             Char *cond_str,
             Char *filename, U32 line_num)
{
    if (!cond) {
        report("Claim \"%s\" Failed in %s:%u\n\n",
               cond_str, filename, line_num);

        quit();
    }
}

/* ===================
 * Unit Test Framework
 */

# define utTest(cond) ut_Test(cond, #cond, __FILE__, __LINE__)

header_function
void ut_Test (B32 cond,
              Char *cond_str,
              Char *filename, U32 line_num) {
    if (!(cond)) {
        report("Test Failed: (%s:%u) %s\n", filename, line_num, cond_str);
        quit();
    }
}

/* ****************************************************************************
 * MEMORY ALLOCATORS **********************************************************
 */

# if defined(COMPILER_CLANG)
#  pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
# endif

typedef enum MemAllocMode {
    MemAllocMode_NONE,
    MemAllocMode_CREATE,
    MemAllocMode_ALLOC,
    MemAllocMode_REALLOC,
    MemAllocMode_DEALLOC,
    MemAllocMode_DEALLOC_ALL,
} MemAllocMode;

# define MEM_ALLOCATOR(allocator)               \
    void* allocator(MemAllocMode mode,          \
                    Size size, void* old_ptr,   \
                    void *data)
typedef MEM_ALLOCATOR(MemAllocator);

/* =============================
 * Default Memory Allocator
 */

struct MemCRT_Header {
    Size size;
};

# define memCRTAlloc(size)        memCRT(MemAllocMode_ALLOC,   size, NULL, NULL)
# define memCRTRealloc(ptr, size) memCRT(MemAllocMode_REALLOC, size, ptr,  NULL)
# define memCRTDealloc(ptr)       memCRT(MemAllocMode_DEALLOC, 0,    ptr,  NULL)

header_function
MEM_ALLOCATOR(memCRT)
{
    unused_variable(data);
    switch (mode) {
        case MemAllocMode_CREATE: {
            // NOTE(naman): Not needed for now
        } break;

        case MemAllocMode_ALLOC: {
            Size memory_size = memAlignUp(size);
            Size header_size = memAlignUp(sizeof(struct MemCRT_Header));
            Size total_size = memory_size + header_size;

            Byte *mem = malloc(total_size);

            struct MemCRT_Header *header = (struct MemCRT_Header *)mem;
            header->size = memory_size;

            Byte *result = mem + header_size;

            memset(result, 0, memory_size);

            return result;
        } break;

        case MemAllocMode_REALLOC: {
            Size memory_size = memAlignUp(size);
            Size header_size = memAlignUp(sizeof(struct MemCRT_Header));
            Size total_size = memory_size + header_size;

            Byte *mem = malloc(total_size);

            struct MemCRT_Header *header = (struct MemCRT_Header *)mem;
            header->size = memory_size;

            Byte *result = mem + header_size;

            if (old_ptr != NULL) {
                Byte *previous_mem = (Byte*)old_ptr - header_size;
                struct MemCRT_Header *previous_header = (struct MemCRT_Header *)previous_mem;
                Size previous_size = previous_header->size;

                memcpy(result, old_ptr, previous_size);
                memset(result + previous_size, 0, memory_size - previous_size);

                memCRTDealloc(old_ptr);
            } else {
                memset(result, 0, memory_size);
            }

            return result;
        } break;

        case MemAllocMode_DEALLOC: {
            if (old_ptr == NULL) {
                return NULL;
            }

            Size header_size = memAlignUp(sizeof(struct MemCRT_Header));
            Byte *mem = (Byte*)old_ptr - header_size;
            free(mem);
        } break;

        case MemAllocMode_DEALLOC_ALL: {
            // TODO(naman): Maybe we should use a off-the-shelf malloc that allows this?
        } break;

        case MemAllocMode_NONE: {
            breakpoint();
        } break;
    }

    return NULL;
}

/* =============================
 * Arena Memory Allocator
 */

typedef struct {
    Size len;
    Size cap;
    void *previous;
    void *next;
    Byte memory[];
} MemArena;

# define memArenaCreate(cap)       memArena(MemAllocMode_CREATE,     cap,  NULL, NULL)
# define memArenaAlloc(data, size) memArena(MemAllocMode_ALLOC,      size, NULL, data)
# define memArenaDeallocAll(data)  memArena(MemAllocMode_DEALLOC_ALL, 0,   NULL, data)

header_function
MEM_ALLOCATOR(memArena)
{
    unused_variable(old_ptr);

    switch (mode) {
        case MemAllocMode_CREATE: {
            Size memory_size = memAlignUp(size);
            MemArena *arena = memCRTAlloc(sizeof(*arena) + memory_size);

            arena->len = 0;
            arena->cap = memory_size;
            arena->previous = NULL;
            arena->next = NULL;

            return arena;
        } break;

        case MemAllocMode_ALLOC: {
            Size memory_size = memAlignUp(size);

            MemArena *arena = data;

            while (true) {
                if ((arena->len + memory_size) <= (arena->cap)) {
                    break;
                } else if (arena->next == NULL) {
                    Size arena_size = (arena->cap > memory_size) ? arena->cap : memory_size;
                    MemArena *next = memCRTAlloc(sizeof(*next) + arena_size);

                    arena->next = next;

                    next->len = 0;
                    next->cap = arena_size;
                    next->previous = arena;
                    next->next = NULL;

                    arena = next;

                    break;
                } else {
                    arena = arena->next;
                }
            }

            // NOTE(naman): At this point, `arena` holds the arena which has enough space for
            // the memory allocation
            void *result = &(arena->memory[arena->len]);
            arena->len = arena->len + memory_size;

            return result;
        } break;

        case MemAllocMode_REALLOC: {
            // NOTE(naman): Not supported
        } break;

        case MemAllocMode_DEALLOC: {
            // NOTE(naman): Not supported
        } break;

        case MemAllocMode_DEALLOC_ALL: {
            MemArena *arena = data;

            while (arena != NULL) {
                MemArena *child = arena->next;
                memCRTDealloc(arena);
                arena = child;
            }
        } break;

        case MemAllocMode_NONE: {
            breakpoint();
        } break;
    }

    return NULL;
}

/* =============================
 * Tree Memory Allocator
 */

struct MemTree_Header {
    void *child;
    void *parent;
    void *next;
    void *previous;
};

# define memTree_GetHeader(ptr) ((struct MemTree_Header*)               \
                                 ((Byte*)(ptr) - memAlignUp(sizeof(struct MemTree_Header))))

# define memTreeAlloc(size, parent) memTree(MemAllocMode_ALLOC,   size, NULL, parent)
# define memTreeDealloc(ptr)        memTree(MemAllocMode_DEALLOC, 0,    ptr,  NULL)

header_function
void memTree_DeallocRecursive (void *node)
{
    struct MemTree_Header *node_header = memTree_GetHeader(node);
    void *child = node_header->child;

    if (child != NULL) {
        void *now = child;

        while (now != NULL) {
            struct MemTree_Header *now_header = memTree_GetHeader(now);
            void *sibling = now_header->next;
            memTree_DeallocRecursive(now);
            node_header->child = sibling; // To prevent dangling pointers
            now = sibling;
        }
    }

    claim(node_header->child == NULL);

    if (node_header->previous != NULL) {
        struct MemTree_Header *previous_header = memTree_GetHeader(node_header->previous);
        previous_header->next = node_header->next;
    }

    if (node_header->next != NULL) {
        struct MemTree_Header *next_header = memTree_GetHeader(node_header->next);
        next_header->previous = node_header->previous;
    }

    if (node_header->parent != NULL) {
        struct MemTree_Header *parent_header = memTree_GetHeader(node_header->parent);
        parent_header->child = node_header->next;
    }

    memCRTDealloc(node_header);

    return;
}

header_function
MEM_ALLOCATOR(memTree)
{
    switch (mode) {
        case MemAllocMode_CREATE: {
            // NOTE(naman): Not supported
        } break;
        case MemAllocMode_ALLOC: {
            void *parent = data;

            Size requested_size = size;
            Size allocated_size = memAlignUp(requested_size);

            Size header_size = sizeof(struct MemTree_Header);
            Size aligned_header_size = memAlignUp(header_size);

            Size total_size = allocated_size + aligned_header_size;

            void *mem = memCRTAlloc(total_size);

            void *result = (Byte*)mem + aligned_header_size;

            struct MemTree_Header *header = mem;
            header->parent = parent;
            header->child = NULL;
            header->next = NULL;
            header->previous = NULL;

            if (parent == NULL) {
                return result;
            }

            struct MemTree_Header *parent_header = memTree_GetHeader(parent);

            if (parent_header->child == NULL) {
                parent_header->child = result;
                return result;
            }

            void *first_child = parent_header->child;
            struct MemTree_Header *first_child_header = memTree_GetHeader(first_child);

            void *last_sibling = first_child;
            void *sibling = first_child_header->next;

            while (sibling != NULL) {
                struct MemTree_Header *sibling_header = memTree_GetHeader(sibling);
                last_sibling = sibling;
                sibling = sibling_header->next;
            }

            struct MemTree_Header *last_sibling_header = memTree_GetHeader(last_sibling);
            last_sibling_header->next = result;
            header->previous = last_sibling;

            return result;
        } break;

        case MemAllocMode_REALLOC: {
            // TODO(naman): Just allocate a new node and patch-up the the pointers?
        } break;

        case MemAllocMode_DEALLOC: {
            if (old_ptr == NULL) {
                return NULL;
            }

            memTree_DeallocRecursive(old_ptr);

            return NULL;
        } break;

        case MemAllocMode_DEALLOC_ALL: {
            // TODO(naman): Should we somehow save the top level parent in the header?
        } break;

        case MemAllocMode_NONE: break;
    }

    return NULL;
}

# if defined(COMPILER_CLANG)
#  pragma clang diagnostic pop
# endif


/* ****************************************************************************
 * DATA STRUCTURES ******************************************************************
 */

/* ==============
 * Strechy Buffer
 */

/* API ----------------------------------------
 * Size  sbufAdd       (T *ptr, T elem)
 * void  sbufDelete    (T *ptr)
 * T*    sbufEnd       (T *ptr)
 *
 * Size  sbufSizeof    (T *ptr)
 * Size  sbufElemin    (T *ptr)
 * Size  sbufMaxSizeof (T *ptr)
 * Size  sbufMaxElemin (T *ptr)
 */

typedef struct Sbuf_Header {
    Size cap; // NOTE(naman): Maximum number of elements that can be stored
    Size len; // NOTE(naman): Count of elements actually stored
    Byte buf[];
} Sbuf_Header;

# define sbuf_GetHeader(sb) ((Sbuf_Header*)(void*)((Byte*)(sb) - offsetof(Sbuf_Header, buf)))

# define sbuf_Len(sb)         ((sb) ? sbuf_GetHeader(sb)->len : 0U)
# define sbuf_Cap(sb)         ((sb) ? sbuf_GetHeader(sb)->cap : 0U)

# define sbufAdd(sb, ...)     ((sb) = sbuf_Grow((sb), sizeof(*(sb))),   \
                               (sb)[sbuf_Len(sb)] = (__VA_ARGS__),      \
                               ((sbuf_GetHeader(sb))->len)++)
# define sbufDelete(sb)       ((sb) ?                                   \
                               (memCRTDealloc(sbuf_GetHeader(sb)), (sb) = NULL) : \
                               0)
# define sbufClear(sb)        ((sb) ?                                   \
                               (memset((sb), 0, sbufSizeof(sb)),        \
                                sbuf_GetHeader(sb)->len = 0) :          \
                               0)
# define sbufResize(sb, n)    (((n) > sbufMaxElemin(sb)) ?              \
                               ((sb) = sbuf_Resize(sb, n, sizeof(*(sb)))) : \
                               (sb) = (sb))

# define sbufSizeof(sb)       (sbuf_Len(sb) * sizeof(*(sb)))
# define sbufElemin(sb)       (sbuf_Len(sb))
# define sbufMaxSizeof(sb)    (sbuf_Cap(sb) * sizeof(*(sb)))
# define sbufMaxElemin(sb)    (sbuf_Cap(sb))
# define sbufEnd(sb)          ((sb) + sbuf_Len(sb))

#define sbufUnsortedDelete(sb, i, z) (((sb)[(i)] = (sb)[sbuf_Len(sb) - 1]), \
                                      ((sb)[sbuf_Len(sb) - 1] = (z)),   \
                                      ((sbuf_GetHeader(sb)->len)--))

# if defined(COMPILER_CLANG)
#  pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
# endif

header_function
    void* sbuf_Grow (void *buf, Size elem_size)
{
    if ((sbuf_Len(buf) + 1) <= sbuf_Cap(buf)) {
        return buf;
    } else {
        Size new_cap = max(2 * sbuf_Cap(buf), 4);

        Size new_size = (new_cap * elem_size) + sizeof(Sbuf_Header);

        Sbuf_Header *new_header = NULL;

        if (buf != NULL) {
            new_header = memCRTRealloc(sbuf_GetHeader(buf), new_size);
        } else {
            new_header = memCRTAlloc(new_size);
        }

        new_header->cap = new_cap;

        return new_header->buf;
    }
}

header_function
void* sbuf_Resize (void *buf, Size elem_size, Size elem_count)
{
    Size new_cap = elem_count;

    Size new_size = (new_cap * elem_size) + sizeof(Sbuf_Header);

    Sbuf_Header *new_header = NULL;

    if (buf != NULL) {
        new_header = memCRTRealloc(sbuf_GetHeader(buf), new_size);
    } else {
        new_header = memCRTAlloc(new_size);
    }

    new_header->cap = new_cap;

    return new_header->buf;
}

header_function
void sbufUnitTest (void)
{
    S32 *buf = NULL;

    sbufAdd(buf, 42);
    utTest(buf != NULL);

    sbufAdd(buf, 1234);

    utTest(sbuf_Len(buf) == 2);
    utTest(sbuf_Cap(buf) >= sbuf_Len(buf));

    utTest(buf[0] == 42);
    utTest(buf[1] == 1234);

    sbufDelete(buf);

    utTest(buf == NULL);
}

# if defined(COMPILER_CLANG)
#  pragma clang diagnostic pop
# endif

/* ==========================
 * Pointer-Pointer Hash Map
 */

typedef struct Hashmap {
    struct HashmapKeys {
        U64  hash;
        Uptr key;
    } *keys;

    Uptr *values;

    MemAllocator *allocator;

    Size total_slots, filled_slots;

    U64 a, b, m; /* Hashing constants */

    U64 r; /* Last random number */
} Hashmap;

typedef enum HM_Flag {
    HM_Flag_EMPTY,
    HM_Flag_FILLED,
    HM_Flag_VACATED, // Previously filled
} HM_Flag;


/* API ---------------------------------------------------
 * Hashmap hmCreate  (MemAllocator allocator,
 *                    Size min_slots);
 * void    hmDelete  (Hashmap hm);
 * void*   hmInsert  (Hashmap *hm, void *key, void *value);
 * Uptr    hmInsertI (Hashmap *hm, Uptr key, Uptr value);
 * void*   hmLookup  (Hashmap *hm, void *key);
 * Uptr    hmLookupI (Hashmap *hm, Uptr key);
 * void*   hmRemove  (Hashmap *hm, void *key);
 * Uptr    hmRemoveI (Hashmap *hm, Uptr key);
 */


#define hm_GetFlag(hash) ((hash) >> 62)
#define hm_GetHash(hash) ((hash) & 0x3FFFFFFFFFFFFFFFULL)
#define hm_SetFlag(hash, flag) (hm_GetHash(hash) | (((U64)(flag)) << 62))

/*
 * https://en.wikipedia.org/wiki/Universal_hashing#Avoiding_modular_arithmetic

 * w is number of bits in machine word (64 in our case)
 * s is the number of buckets/bins (slots in the hash table) to which the
 *   universe of hashable objects is to be mapped
 * m is log2(s) (=> m = 2^s) and is equal to the number of bits in the final hash
 * a is a random odd positive integer < 2^w (fitting in w bits)
 * b is a random non-negative integer < 2^(w-m) (fitting in (w-m) bits)
 *
 * r is the last random number generated and is just an implementation detail.
 */

header_function
void hm_UpdateConstants (Hashmap *hm)
{
    hm->m = hm->m + 1;
    hm->total_slots = 1ULL << (hm->m);

    do {
        hm->r = u64Rand(hm->r);
        hm->a = hm->r;
    } while ((hm->a & 0x01) != 0x01); // Make sure that hm.a is odd

    hm->r = u64Rand(hm->r);
    // b should be (64 - m) bits long
    hm->b = hm->r >> hm->m;
}

header_function
U64 hm_Hash (Hashmap *hm, Uptr key)
{
    U64 result =  ((hm->a * key) + hm->b) & (0xFFFFFFFFFFFFFFFFULL >> (64 - hm->m));
    return result;
}

header_function
Hashmap hmCreate (MemAllocator allocator,
                  Size min_slots)
{
    Hashmap hm = {0};
    hm.allocator = allocator;
    hm.m = u64Log2(min_slots); // Log of closest lower power of two

    // This will make m log of closest upper power of two
    hm_UpdateConstants(&hm);

    hm.keys   = allocator(MemAllocMode_ALLOC,
                          (hm.total_slots) * sizeof(*(hm.keys)),
                          NULL, NULL);
    hm.values = allocator(MemAllocMode_ALLOC,
                          (hm.total_slots) * sizeof(*(hm.values)),
                          NULL, NULL);

    // For NULL keys
    hm.keys[0].hash = hm_SetFlag(hm.keys[0].hash, HM_Flag_FILLED);
    hm.filled_slots = 1;

    return hm;
}

header_function
void hmDelete (Hashmap hm)
{
    hm.allocator(MemAllocMode_DEALLOC, 0, hm.keys,   NULL);
    hm.allocator(MemAllocMode_DEALLOC, 0, hm.values, NULL);
}

header_function
Size hm_LinearProbeSearch (Hashmap *hm, Uptr key)
{
    if (key == 0) return 0;

    U64 hash = hm_Hash(hm, key);
    U64 hash_real = hm_GetHash(hash);

    Size index = 0, i = 0;
    B32 found = false;

    for (i = 0; !found && (i < hm->total_slots); ++i) {
        index = (hash_real + i) % (hm->total_slots);

        HM_Flag flag = hm_GetFlag(hm->keys[index].hash);
        if ((flag == HM_Flag_FILLED) &&
            (hm->keys[index].key == key)) {
            found = true;
        } else if (flag == HM_Flag_VACATED) {
            continue;
        } else if (flag == HM_Flag_EMPTY) {
            break;
        }
    }

    if (i == hm->total_slots) {
        index = 0;
    }

    Size result = (found ? index : 0);

    return result;
}

header_function
Uptr hm_LinearProbeInsertion (Hashmap *hm,
                              U64 hash, Uptr key, Uptr value)
{
    Uptr result_value = value;

    for (Size i = 0; i < hm->total_slots; ++i) {
        Size index = (hash + i) % (hm->total_slots);
        HM_Flag flag  = hm_GetFlag(hm->keys[index].hash);

        B32 fill_now = false;
        switch (flag) {
            case HM_Flag_FILLED: {
                if (hm->keys[index].key == key) {
                    result_value = hm->values[index];
                    fill_now = true;
                }
            } break;
            case HM_Flag_VACATED:
            case HM_Flag_EMPTY: {
                fill_now = true;
            } break;
        }

        if (fill_now) {
            hm->keys[index].key = key;
            hm->values[index] = value;
            hm->keys[index].hash = hm_SetFlag(hash, HM_Flag_FILLED);
            break;
        }
    }

    return result_value;
}

header_function
Uptr hmInsertI (Hashmap *hm, Uptr key, Uptr value)
{
    if ((key == 0) || (value == 0)) return 0;

    if ((2U * (hm->filled_slots)) > (hm->total_slots)) {
        Size total_slots = hm->total_slots;
        struct HashmapKeys *keys   = hm->keys;
        Uptr *values = hm->values;

        hm_UpdateConstants(hm);

        hm->keys   = (hm->allocator)(MemAllocMode_ALLOC,
                                     sizeof(*(hm->keys)) * hm->total_slots,
                                     NULL, NULL);
        hm->values = (hm->allocator)(MemAllocMode_ALLOC,
                                     sizeof(*(hm->values)) * hm->total_slots,
                                     NULL, NULL);

        // For NULL keys
        hm->keys[0].hash = hm_SetFlag(hm->keys[0].hash, HM_Flag_FILLED);

        for (Size i = 1; i < total_slots; ++i) {
            U64 hash_i_old = keys[i].hash;
            Uptr key_i     = keys[i].key;
            Uptr value_i   = values[i];
            HM_Flag flag_i   = hm_GetFlag(hash_i_old);
            U64 hash_i_new = hm_Hash(hm, key_i);
            if (flag_i == HM_Flag_FILLED) {
                hm_LinearProbeInsertion(hm, hash_i_new, key_i, value_i);
            }
        }

        (hm->allocator)(MemAllocMode_DEALLOC, 0, keys,   NULL);
        (hm->allocator)(MemAllocMode_DEALLOC, 0, values, NULL);
    }

    U64 hash = hm_Hash(hm, key);

    Uptr result_value = hm_LinearProbeInsertion(hm, hash, key, value);
    hm->filled_slots += 1;

    return result_value;
}

# if defined(COMPILER_CLANG)
#  pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wbad-function-cast"
# endif

header_function
void* hmInsert (Hashmap *hm, void *key, void *value)
{
    void *result_value = (void*)hmInsertI(hm, (Uptr)key, (Uptr)value);
    return result_value;
}

header_function
Uptr hmLookupI (Hashmap *hm, Uptr key)
{
    Size location = hm_LinearProbeSearch(hm, key);
    Uptr result_value = hm->values[location];

    return result_value;
}

header_function
void* hmLookup (Hashmap *hm, void *key)
{
    void *result_value = (void*)hmLookupI(hm, (Uptr)key);
    return result_value;
}

header_function
Uptr hmRemoveI (Hashmap *hm, Uptr key)
{
    Size location = hm_LinearProbeSearch(hm, key);

    Uptr result_value = 0;
    if (location != 0) {
        U64 hash = hm->keys[location].hash;
        hm->keys[location].hash = hm_SetFlag(hash, HM_Flag_VACATED);
        hm->filled_slots -= 1;
        result_value = hm->values[location];
    }

    return result_value;
}

header_function
void* hmRemove (Hashmap *hm, void *key)
{
    void *result_value = (void*)hmRemoveI(hm, (Uptr)key);
    return result_value;
}

# if defined(COMPILER_CLANG)
#  pragma clang diagnostic pop
# endif

header_function
void hmUnitTest (void)
{
    Hashmap hm = hmCreate(memCRT, 1);

    /* No Entries */
    utTest(hmLookup(&hm, (void*)0) == NULL);
    utTest(hmLookup(&hm, (void*)1) == NULL);
    utTest(hmLookup(&hm, (void*)2) == NULL);

    /* Insertion Test */
    Size f0 = hm.filled_slots;

    hmInsert(&hm, (void*)1, (void*)1);
    utTest(hm.filled_slots == (f0 + 1));
    utTest(hmLookup(&hm, (void*)0) == NULL);
    utTest(hmLookup(&hm, (void*)1) == (void*)1);
    utTest(hmLookup(&hm, (void*)2) == NULL);

    hmInsert(&hm, (void*)2, (void*)42);
    utTest(hm.filled_slots == (f0 + 2));
    utTest(hmLookup(&hm, (void*)0) == NULL);
    utTest(hmLookup(&hm, (void*)1) == (void*)1);
    utTest(hmLookup(&hm, (void*)2) == (void*)42);

    /* Duplicate Key */
    void *v1 = hmInsert(&hm, (void*)2, (void*)24);
    utTest(v1 == (void*)42);
    utTest(hmLookup(&hm, (void*)0) == NULL);
    utTest(hmLookup(&hm, (void*)1) == (void*)1);
    utTest(hmLookup(&hm, (void*)2) == (void*)24);

    /* Removal Test */
    void *v2 = hmRemove(&hm, (void*)2);
    utTest(v2 == (void*)24);
    utTest(hmLookup(&hm, (void*)2) == NULL);

    void *v3 = hmRemove(&hm, (void*)1);
    utTest(v3 == (void*)1);
    utTest(hmLookup(&hm, (void*)1) == NULL);

    /* NULL Check */
    Size f1 = hm.filled_slots;
    hmInsert(&hm, (void*)0, (void*)1);
    utTest(hm.filled_slots == f1);
    utTest(hmLookup(&hm, (void*)0) == (void*)0);

    Size f2 = hm.filled_slots;
    hmRemove(&hm, (void*)0);
    utTest(hm.filled_slots == f2);
    utTest(hmLookup(&hm, (void*)0) == (void*)0);

    /* Expansion Test */
    hmInsert(&hm, (void*)3, (void*)33);
    utTest(hmLookup(&hm, (void*)3) == (void*)33);

    hmInsert(&hm, (void*)4, (void*)44);
    utTest(hmLookup(&hm, (void*)4) == (void*)44);

    hmInsert(&hm, (void*)5, (void*)55);
    utTest(hmLookup(&hm, (void*)5) == (void*)55);

    hmInsert(&hm, (void*)6, (void*)66);
    utTest(hmLookup(&hm, (void*)6) == (void*)66);

    hmInsert(&hm, (void*)7, (void*)77);
    utTest(hmLookup(&hm, (void*)7) == (void*)77);

    hmInsert(&hm, (void*)8, (void*)88);
    utTest(hmLookup(&hm, (void*)8) == (void*)88);

    hmInsert(&hm, (void*)9, (void*)99);
    utTest(hmLookup(&hm, (void*)9) == (void*)99);

    /* Removal after Expansion */

    hmRemove(&hm, (void*)3);
    utTest(hmLookup(&hm, (void*)3) == NULL);

    hmRemove(&hm, (void*)4);
    utTest(hmLookup(&hm, (void*)4) == NULL);

    hmRemove(&hm, (void*)5);
    utTest(hmLookup(&hm, (void*)5) == NULL);

    hmRemove(&hm, (void*)6);
    utTest(hmLookup(&hm, (void*)6) == NULL);

    hmRemove(&hm, (void*)7);
    utTest(hmLookup(&hm, (void*)7) == NULL);

    hmRemove(&hm, (void*)8);
    utTest(hmLookup(&hm, (void*)8) == NULL);

    hmRemove(&hm, (void*)9);
    utTest(hmLookup(&hm, (void*)9) == NULL);

    hmDelete(hm);

    return;
}

/* ==========================
 * Text Interning
 */

/* API ----------------------------------------------------
 * Char* internBuffer (Hashmap *hm, Char *start, Char *end)
 * Char* internString (Hashmap *hm, Char *str)
 */

// TODO(naman): Replace all the Char* with Txt after implementing a proper string type

typedef struct {
    Size string_index;
    Size string_length;
    Size child_index;
    B32 used;
    B32 has_child;
} InternElem;

typedef struct {
    InternElem base_elems[256];

    InternElem *more_elems;
    Size more_elem_count;
    Size more_elem_next_entry;

    Char *strings;
    Size strings_size;
    Size strings_next_index;
} InternTable;

header_function
InternTable internCreate (void)
{
    InternTable i = {0};

    i.strings_size = 1024;
    i.strings = memCRTAlloc(i.strings_size);

    return i;
}

header_function
Char *intern_StringInsert (InternTable *it, InternElem *elems, Size index,
                           Char *start, Size string_len)
{
    elems[index].used = true;
    elems[index].string_index = it->strings_next_index;
    elems[index].string_length = string_len;

    if (it->strings_next_index + string_len > it->strings_size) {
        it->strings_size = string_len + (2 * it->strings_size);
        it->strings = memCRTRealloc(it->strings, it->strings_size);
    }

    memcpy(&(it->strings[it->strings_next_index]), start, string_len);
    it->strings[it->strings_next_index + string_len] = '\0';
    it->strings_next_index = it->strings_next_index + string_len + 1;

    Char *result = &(it->strings[elems[index].string_index]);
    return result;
}

header_function
U8 intern_PearsonHash (Char *string, Size len)
{
    // NOTE(naman): Pearson's hash for 8-bit hashing
    // https://en.wikipedia.org/wiki/Pearson_hashing
    persistent_value U8 hash_lookup_table[256] = {
        // 0-255 shuffled in any (random) order suffices
        98,    6,   85, 150,  36,  23, 112, 164, 135, 207, 169,   5,  26,  64, 165, 219, // 01
        61,   20,   68,  89, 130,  63,  52, 102,  24, 229, 132, 245,  80, 216, 195, 115, // 02
        90,  168,  156, 203, 177, 120,   2, 190, 188,   7, 100, 185, 174, 243, 162,  10, // 03
        237,  18,  253, 225,   8, 208, 172, 244, 255, 126, 101,  79, 145, 235, 228, 121, // 04
        123, 251,   67, 250, 161,   0, 107,  97, 241, 111, 181,  82, 249,  33,  69,  55, // 05
        59,  153,   29,   9, 213, 167,  84,  93,  30,  46, 94,   75, 151, 114,  73, 222, // 06
        197,  96,  210,  45,  16, 227, 248, 202,  51, 152, 252, 125,  81, 206, 215, 186, // 07
        39,  158,  178, 187, 131, 136,   1,  49,  50,  17, 141,  91,  47, 129,  60,  99, // 08
        154,  35,   86, 171, 105,  34,  38, 200, 147,  58,  77, 118, 173, 246,  76, 254, // 09
        133, 232,  196, 144, 198, 124,  53,   4, 108,  74, 223, 234, 134, 230, 157, 139, // 10
        189, 205,  199, 128, 176,  19, 211, 236, 127, 192, 231,  70, 233,  88, 146,  44, // 11
        183, 201,   22,  83,  13, 214, 116, 109, 159,  32,  95, 226, 140, 220,  57,  12, // 12
        221,  31,  209, 182, 143,  92, 149, 184, 148,  62, 113,  65,  37,  27, 106, 166, // 13
        3,    14,  204,  72,  21,  41,  56,  66,  28, 193,  40, 217,  25,  54, 179, 117, // 14
        238,  87,  240, 155, 180, 170, 242, 212, 191, 163,  78, 218, 137, 194, 175, 110, // 15
        43,  119,  224,  71, 122, 142,  42, 160, 104,  48, 247, 103,  15,  11, 138, 239, // 16
    };

    U8 hash = (U8)len;
    for (Size i = 0; i < len; i++) {
        hash = hash_lookup_table[hash ^ string[i]];
    }

    return hash;
}

// Intern a non-zero terminated string
//     start points to the first character.
//     end points to the character after the last character.
header_function
Char * internStringBuffer (InternTable *it, Char *start, Char *end)
{
    Size len = (Size)(Dptr)(end - start); // Length of string

    U8 base_index = intern_PearsonHash(start, len);

    if (it->base_elems[base_index].used) {
        // Our string has probably been inserted already.
        // (or atleast some string with same hash has been inserted :)
        Char *value = &(it->strings[it->base_elems[base_index].string_index]);

        if ((it->base_elems[base_index].string_length == len) &&
            (strncmp(value, start, len) == 0)) {
            // This is our string, return it
            return value;
        }

        // That wasn't our string, keep searching for it...
        // Perhaps our string lies deeper in a rabbit hole?
        if (it->base_elems[base_index].has_child) {
            // More strings with same hash have been added, our string might be one of them!
            Size i = it->base_elems[base_index].child_index;

            do {
                value = &(it->strings[it->more_elems[i].string_index]);
                if ((it->more_elems[i].string_length == len) &&
                    (strncmp(value, start, len) == 0)) {
                    // Found it, this is our string. Return it
                    return value;
                }

                i = it->more_elems[i].child_index;
            } while (it->more_elems[i].has_child);


            // No, it wasn't.
            // That means our string hasn't been added yet.
            // So, let's add it!!! (and return it)
            if (it->more_elem_next_entry == it->more_elem_count) {
                it->more_elem_count = 2 * it->more_elem_count;
                it->more_elems = memCRTRealloc(it->more_elems,
                                               sizeof(*(it->more_elems)) * it->more_elem_count);
            }

            Size entry_index = it->more_elem_next_entry;
            it->more_elem_next_entry += 1;

            it->more_elems[i].has_child = true;
            it->more_elems[i].child_index = entry_index;


            Char *result = intern_StringInsert(it, it->more_elems, entry_index, start, len);
            return result;
        } else {
            // Base didn't have a child, so we'll make it adopt our string by necessary force.
            if (it->more_elems != NULL) {
                // Though a rabbit hole exists, our string is not yet in it.
                // So by gods, we are going to shove it right in.
                if (it->more_elem_next_entry == it->more_elem_count) {
                    it->more_elem_count = 2 * (it->more_elem_count);
                    it->more_elems = memCRTRealloc(it->more_elems,
                                                   sizeof(*(it->more_elems)) *
                                                   it->more_elem_count);
                }

                Size entry_index = it->more_elem_next_entry;
                it->more_elem_next_entry += 1;

                it->base_elems[base_index].has_child = true;
                it->base_elems[base_index].child_index = entry_index;

                Char *result = intern_StringInsert(it, it->more_elems, entry_index, start, len);
                return result;
            } else {
                // There is no rabbit hole to begin with, there never was! :(
                // But we, the brave earthlings, are going to make one. Or die trying.
                it->more_elem_count = 256;
                it->more_elems = memCRTRealloc(it->more_elems,
                                               sizeof(*(it->more_elems)) * it->more_elem_count);

                // And now let's put in the element and return it.
                Size entry_index = 0;
                it->more_elem_next_entry = 1;

                it->base_elems[base_index].has_child = true;
                it->base_elems[base_index].child_index = entry_index;

                Char *result = intern_StringInsert(it, it->more_elems, entry_index, start, len);
                return result;
            }
        }
    } else {
        // The string is a lie.
        // Tyler Durden never existed.
        // God is dead.
        // E.g., string has not been inserted before, insert it and return a pointer to it.
        Char *result = intern_StringInsert(it, it->base_elems, base_index, start, len);
        return result;
    }
}

// Intern a non-zero terminated string
//     start points to the first character.
//     end points to the character after the last character.
header_function
Char * internCheckStringBuffer (InternTable *it, Char *start, Char *end)
{
    Size len = (Size)(Dptr)(end - start); // Length of string

    U8 base_index = intern_PearsonHash(start, len);

    if (it->base_elems[base_index].used) {
        // Our string has probably been inserted already.
        // (or atleast some string with same hash has been inserted :)
        Char *value = &(it->strings[it->base_elems[base_index].string_index]);

        if ((it->base_elems[base_index].string_length == len) &&
            (strncmp(value, start, len) == 0)) {
            // This is our string, return it
            return value;
        }

        // That wasn't our string, keep searching for it...
        // Perhaps our string lies deeper in a rabbit hole?
        if (it->base_elems[base_index].has_child) {
            // More strings with same hash have been added, our string might be one of them!
            Size i = it->base_elems[base_index].child_index;

            do {
                value = &(it->strings[it->more_elems[i].string_index]);
                if ((it->more_elems[i].string_length == len) &&
                    (strncmp(value, start, len) == 0)) {
                    // Found it, this is our string. Return it
                    return value;
                }

                i = it->more_elems[i].child_index;
            } while (it->more_elems[i].has_child);


            // No, it wasn't.
            // That means our string hasn't been added yet.
            return NULL;
        } else {
            // Base didn't have a child, so that's that.
            return NULL;
        }
    } else {
        // The string is a lie.
        // Tyler Durden never existed.
        // God is dead.
        return NULL;
    }
}

// Intern a zero-terminated C-string
header_function
Char* internString (InternTable *it, Char *str)
{
    Size len = strlen(str);
    Char *result = internStringBuffer(it, str, str + len);

    return result;
}

// Check if a zero-terminated C-string has already been interned
header_function
Char* internCheckString (InternTable *it, Char *str)
{
    Size len = strlen(str);
    Char *result = internCheckStringBuffer(it, str, str + len);

    return result;
}

header_function
void internUnitTest (void)
{
    Char x[] = "Hello";
    Char y[] = "Hello";

    InternTable intern_table = internCreate();

    utTest(x != y);

    Char *y_interned = internString(&intern_table, y);
    Char *x_interned = internString(&intern_table, x);
    utTest(x_interned == y_interned);

    Char z[] = "World";
    Char *z_interned = internString(&intern_table, z);
    utTest(x_interned != z_interned);

    Char p[] = "Hello!!";
    Char *p_interned = internString(&intern_table, p);
    utTest(x_interned != p_interned);

    // TODO(naman): Write tests to see what happens if two strings with same hash are interned.

    return;
}

/* ****************************************************************************
 * UNIT TESTS FOR EVERYTHING IN THIS FILE *************************************
 */

header_function
void nlibUnitTest (void)
{
    sbufUnitTest();
    hmUnitTest();
    internUnitTest();

    return;
}


#define NLIB_H_INCLUDE_GUARD
#endif // NLIB_H_INCLUDE_GUARD
