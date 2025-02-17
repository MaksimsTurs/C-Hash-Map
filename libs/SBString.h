#pragma once
/*=======================================================================*/
#include <stdlib.h>
#include <string.h>
/*=======================================================================*/
typedef unsigned char      SB_Return_Code;
typedef unsigned char      SB_Bool;
typedef unsigned char      SB_UChar;
typedef char*              SB_String_Ptr;
typedef char               SB_Char;

typedef unsigned long long SB_ULLong;
typedef long long          SB_LLong;

typedef unsigned short     SB_UShort;
/*=======================================================================*/
// Types constants.
#define SB_TRUE                   (SB_UChar)1
#define SB_FALSE                  (SB_UChar)0
// Error/success constants.
#define SB_ERROR_MEMALLOC         (SB_UChar)0
#define SB_ERROR_INVALID_PTR      (SB_UChar)2
#define SB_ERROR_INVALID_POSITION (SB_UChar)3
#define SB_ERROR_INVALID_COUNT    (SB_UChar)4
#define SB_SUCCESS                (SB_UChar)1
// Character constants.
#define SB_NULL_TERMINATOR        (SB_UChar)'\0'
/*=======================================================================*/
typedef struct {
	SB_String_Ptr value;
	SB_ULLong length;
} SB_String;

typedef struct {
	SB_String_Ptr* items;
	SB_UShort size;
} SB_Split;
/*=======================================================================*/
SB_Return_Code sb_string_create(SB_String* self, const SB_String_Ptr string);
SB_Return_Code sb_string_free(SB_String* self);
SB_Return_Code sb_string_split(SB_Split* split, const SB_String_Ptr to_split, const SB_Char spliter);
SB_Return_Code sb_string_split_free(SB_Split* self);
SB_Return_Code sb_string_append(SB_String* self, const SB_String_Ptr append_string, const SB_UChar count);
SB_Return_Code sb_string_slice(SB_String* self, const SB_ULLong start, const SB_ULLong end);

SB_ULLong sb_string_length(const SB_String_Ptr self);
SB_LLong sb_string_index_of(SB_String* self, const SB_Char ch, const SB_ULLong start_position);

SB_Bool sb_string_compare(const SB_String_Ptr str_one, const SB_String_Ptr str_two);
/*=======================================================================*/