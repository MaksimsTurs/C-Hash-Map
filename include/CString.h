#pragma once

/*=======================================================================*/

#include <stdlib.h>

/*=======================================================================*/

typedef char*              CSTR_Char_Ptr;
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
#define CSTR_TRUE                    (CSTR_UChar)1
#define CSTR_FALSE                   (CSTR_UChar)0
// Error/success constants.
#define CSTR_ERROR_MEMALLOC          (CSTR_UChar)0
#define CSTR_ERROR_INVALID_PTR       (CSTR_UChar)1
#define CSTR_ERROR_INVALID_LENGTH    (CSTR_UChar)2
#define CSTR_ERROR_INVALID_POSITION  (CSTR_UChar)3
#define CSTR_ERROR_INVALID_COUNT     (CSTR_UChar)4
#define CSTR_ERROR_VALUE_IS_NOT_NULL (CSTR_UChar)5
#define CSTR_ERROR_NOT_FOUND         (CSTR_UChar)6
#define CSTR_SUCCESS                 (CSTR_UChar)7
// Character constants.
#define CSTR_NULL_TERMINATOR         (CSTR_UChar)'\0'

// Utils macros.
#define CSTR_FAIL_IF(condition, error_code) do { \
	if(condition) return error_code;              \
} while(0);

/*=======================================================================*/

typedef struct {
	CSTR_Char_Ptr value;
	CSTR_ULLong length;
} CSTR_String;

typedef struct {
	CSTR_Char_Ptr start;
	CSTR_ULLong length;
} CSTR_Slice;

typedef struct {
	CSTR_Slice* items;
	CSTR_UShort size;
} CSTR_Slices;

/*=======================================================================*/

CSTR_Ret_Code cstr_build(CSTR_String* const this, CSTR_Char_Ptr const value);
CSTR_Ret_Code cstr_free(CSTR_String* const this);
CSTR_Ret_Code cstr_split(CSTR_Slices* const this, CSTR_Char_Ptr const to_split, CSTR_Char const spliter);
CSTR_Ret_Code cstr_split_free(CSTR_Slices* const this);
CSTR_Ret_Code cstr_append(CSTR_String* const this, CSTR_Char_Ptr append_string, CSTR_UChar const count);
CSTR_Ret_Code cstr_slice(CSTR_String* const this, CSTR_ULLong const start, CSTR_ULLong const end);
CSTR_LLong    cstr_index_of(CSTR_String* const this, CSTR_Char const ch, CSTR_ULLong const start_position);

CSTR_Ret_Code cstr_cpy(CSTR_Char_Ptr const dest, CSTR_Char_Ptr const source, CSTR_LLong const length);
CSTR_ULLong   cstr_len(CSTR_Char_Ptr const value);
CSTR_Bool     cstr_comp(CSTR_Char_Ptr const str_one, CSTR_Char_Ptr const str_two);

/*=======================================================================*/