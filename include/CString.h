#pragma once

/*=======================================================================*/

#include <stdlib.h>

/*=======================================================================*/

typedef unsigned char      CSTR_Ret_Code;
typedef unsigned char      CSTR_Bool;

typedef unsigned char      CSTR_UChar;
typedef signed char        CSTR_SChar;
typedef char               CSTR_Char;

typedef unsigned long long CSTR_ULLong;
typedef signed long long   CSTR_SLLong;
typedef long long          CSTR_LLong;
typedef long               CSTR_Long;

typedef unsigned short     CSTR_UShort;
typedef signed short       CSTR_SShort;
typedef short              CSTR_Short;

typedef float              CSTR_Float;

typedef double             CSTR_Double;

/*=======================================================================*/

// Boolean constants.
#define CSTR_TRUE                          (CSTR_UChar)1
#define CSTR_FALSE                         (CSTR_UChar)0
// Error/success constants.
#define CSTR_ERROR_MEMALLOC                (CSTR_UChar)0
#define CSTR_ERROR_INVALID_PTR             (CSTR_UChar)1
#define CSTR_ERROR_INVALID_LENGTH          (CSTR_UChar)2
#define CSTR_ERROR_INVALID_POSITION        (CSTR_UChar)3
#define CSTR_ERROR_INVALID_COUNT           (CSTR_UChar)4
#define CSTR_ERROR_INVALID_RESIZE_FACTOR   (CSTR_UChar)5
#define CSTR_ERROR_INVALID_MEMORY_LOCATION (CSTR_UChar)6
#define CSTR_ERROR_INVALID_BUFFER_SIZE     (CSTR_UChar)7
#define CSTR_ERROR_BUFFER_OVERFLOW         (CSTR_UChar)8
#define CSTR_ERROR_NAN                     (CSTR_UChar)9
#define CSTR_ERROR_MEMORY_NOT_FREED        (CSTR_UChar)10
#define CSTR_ERROR_NOT_FOUND               (CSTR_UChar)11
#define CSTR_SUCCESS                       (CSTR_UChar)12
// Character constants.
#define CSTR_NULL_TERMINATOR               (CSTR_UChar)'\0'
// Magic constants
#define CSTR_LONG_MAGIC                    (CSTR_ULLong)0x01010101L
#define CSTR_HIG_MAGIC                     (CSTR_ULLong)0x80808080L
// Restrictions constants.
#define CSTR_MAX_RESIZE_FACTOR             (CSTR_Float)5.0f
// String memory locations.
#define CSTR_STACK_ALLOCATED               (CSTR_UChar)0
#define CSTR_HEAP_ALLOCATED                (CSTR_UChar)1
// Utils macros.
#define CSTR_FAIL_IF(condition, error_code) do { \
	if(condition)return error_code;                \
} while(0);
#define CSTR_PEAK_TOKEN(str, offset) *(str + offset)
#define CSTR_UNLIKEY(cond)           __builtin_expect(cond, CSTR_FALSE)
#define CSTR_LIKELY(cond)            __builtin_expect(cond, CSTR_TRUE)
#define CSTR_IS_BUFFER_USED(size)    size != 0

/*=======================================================================*/

typedef struct {
	CSTR_Char* val;
	CSTR_ULLong   len;
	
	CSTR_ULLong   buff_size;
	CSTR_Float    buff_resize_fac;
	CSTR_UChar    buff_location : 1;
} CSTR_String;

typedef struct {
	CSTR_Char* start;
	CSTR_ULLong   length;
} CSTR_Slice;

typedef struct {
	CSTR_Slice* items;
	CSTR_UShort size;
} CSTR_Slices;

/*=======================================================================*/

CSTR_Ret_Code cstr_build(CSTR_String* this, const CSTR_Char* val);
CSTR_Ret_Code cstr_free(CSTR_String* this);
CSTR_Ret_Code cstr_split(CSTR_Slices* this, const CSTR_Char* to_split, CSTR_Char spliter);
CSTR_Ret_Code cstr_split_free(CSTR_Slices* this);
CSTR_Ret_Code cstr_append(CSTR_String* this, const CSTR_Char* append_string, CSTR_UChar count);
CSTR_Ret_Code cstr_slice(CSTR_String* this, CSTR_ULLong start, CSTR_ULLong end);
CSTR_LLong    cstr_index_of(CSTR_String* this, CSTR_Char ch, CSTR_ULLong start_position);

CSTR_Ret_Code cstr_cpy(CSTR_Char* dest, const CSTR_Char* source, CSTR_LLong len);
CSTR_ULLong   cstr_len(const CSTR_Char* value);
CSTR_Bool     cstr_comp(const CSTR_Char* str_one, const CSTR_Char* str_two);

CSTR_Ret_Code cstr_to_llong(CSTR_LLong* num, const CSTR_Char* str_num, CSTR_UChar str_len);
CSTR_Ret_Code cstr_llong_to_str(CSTR_Char* buffer, CSTR_UChar buffer_size, CSTR_LLong num);

/*=======================================================================*/