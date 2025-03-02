#pragma once

/*=======================================================================*/

#include <stdlib.h>

#include "./include/CString.h"

/*=======================================================================*/

typedef unsigned char      CMAP_Ret_Code;
typedef unsigned char      CMAP_Bool;
typedef unsigned char      CMAP_UChar;
typedef char               CMAP_Char;

typedef void*              CMAP_Any;
typedef void               CMAP_None;

typedef unsigned int       CMAP_UInt;

typedef unsigned short     CMAP_UShort;

typedef unsigned long long CMAP_Hash;
typedef unsigned long long CMAP_ULLong;

typedef float              CMAP_Float;

/*=======================================================================*/
// Restriction constants
#define CMAP_MAX_SIZE                       (CMAP_ULLong)4000000000
#define CMAP_MAX_KEY_LENGTH                 (CMAP_UChar)64
// Error/success constants.
#define CMAP_ERROR_MEMALLOC                 (CMAP_UChar)0
#define CMAP_ERROR_OVERFLOW                 (CMAP_UChar)1
#define CMAP_ERROR_ITEM_NOT_FOUND           (CMAP_UChar)2
#define CMAP_ERROR_INVALID_RESIZE_DIRECTION (CMAP_UChar)3
#define CMAP_ERROR_INVALID_KEY              (CMAP_UChar)4
#define CMAP_ERROR_INVALID_MAP_SIZE         (CMAP_UChar)5
#define CMAP_ERROR_INVALID_KEY_LENGTH       (CMAP_UChar)6
#define CMAP_ERROR_INVALID_PTR              (CMAP_UChar)7
#define CMAP_ITEM_FOUND                     (CMAP_UChar)8
#define CMAP_SUCCESS                        (CMAP_UChar)9
// Map resize constants
#define CMAP_KEY_GROWTH_SIZE                (CMAP_UChar)1
#define CMAP_KEY_SHRINK_SIZE                (CMAP_UChar)2
// Map factors and size constants
#define CMAP_SMALL_SIZE                     (CMAP_UShort)2000
#define CMAP_SMALL_GROWTH_AT                (CMAP_Float)0.7f
#define CMAP_SMALL_SHRINK_AT                (CMAP_Float)0.4f
#define CMAP_MEDIUM_SIZE                    (CMAP_UShort)20000
#define CMAP_MEDIUM_GROWTH_AT               (CMAP_Float)0.8f
#define CMAP_MEDIUM_SHRINK_AT               (CMAP_Float)0.5f
#define CMAP_BIG_SIZE                       (CMAP_UShort)200000
#define CMAP_BIG_GROWTH_AT                  (CMAP_Float)0.9f
#define CMAP_BIG_SHRINK_AT                  (CMAP_Float)0.5f
// Type constants
#define CMAP_TRUE                           (CMAP_UChar)1
#define CMAP_FALSE                          (CMAP_UChar)0
// Map characters
#define CMAP_NULL_TERMINATOR                '\0'

// Utils macros.
#define CMAP_FAIL_IF(condition, error_message)          do { if(condition) return error_message; } while(0);
#define CMAP_RESET_HASH_IF_BIGGER_THAN_SIZE(hash, size) do { if((hash + 1) >= size) hash = 0; } while(0);
#define CMAP_USE_HASH_ALGORITHM(hash, index, size) do { \
	if(size <= CMAP_SMALL_SIZE) {                         \
		hash += 1;                                          \
	}	else {                                              \
		hash = (hash + (index ^ 2)) % size;                 \
		index += 1;                                         \
	}	                                                    \
} while(0);
#define CMAP_SAFE_CALL(x) do {                     \
	CMAP_Ret_Code exec_code = (x);                   \
	if(exec_code != CMAP_SUCCESS) return exec_code; \
} while(0);
#define CMAP_INITIALIZE(ptr, size) do {             \
	for(CMAP_ULLong index = 0; index < size; index++) \
		*(ptr + index) = NULL;                          \
} while(0);
#define CMAP_GET_SHRINK_FACTOR(size) size >= CMAP_BIG_SIZE    ? CMAP_BIG_SHRINK_AT    :                      \
																		 size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_SHRINK_AT :                      \
																		 size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_SHRINK_AT  : CMAP_SMALL_SHRINK_AT
#define CMAP_GET_GROWTH_FACTOR(size) size >= CMAP_BIG_SIZE    ? CMAP_BIG_GROWTH_AT    :                      \
																		 size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_GROWTH_AT :                      \
																		 size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_GROWTH_AT  : CMAP_SMALL_GROWTH_AT
#define CMAP_GET_PRIME_FROM(x)               (x * 2) >= CMAP_MAX_SIZE ? CMAP_MAX_SIZE : x * 2
#define CMAP_IS_MAP_TO_SMALL(occupied, size) (((CMAP_Float)occupied) / size) >= (CMAP_GET_GROWTH_FACTOR(size))
#define CMAP_IS_MAP_TO_BIG(occupied, size)   (((CMAP_Float)occupied) / size) <= (CMAP_GET_SHRINK_FACTOR(size))
// Optimization macros
#define CMAP_IS_EQUAL(x, y)                  (((x) ^ (y)) == 0)
#define CMAP_IS_NEQUAL(x, y)                 (((x) ^ (y)) != 0)
#define CMAP_UNLIKEY(x)                      __builtin_expect(x, CMAP_FALSE)
#define CMAP_LIKELY(x)                       __builtin_expect(x, CMAP_TRUE)

/*=======================================================================*/

typedef struct CMAP_Item {
	CMAP_Char* key;
	CMAP_Any value;
} CMAP_Item;

typedef struct CMAP_Map {
	CMAP_Item** items;
	CMAP_ULLong size;
	CMAP_ULLong occupied;
} CMAP_Map;

/*=======================================================================*/

/* Hashing and Collision handle functions */
CMAP_Ret_Code cmap_gen_hash(CMAP_Hash* const hash, const CMAP_Char* key, CMAP_Hash size);
CMAP_Ret_Code cmap_find_hash(CMAP_Hash* const hash, const CMAP_Item** items, CMAP_ULLong occupied, CMAP_ULLong size);
CMAP_Ret_Code cmap_find_hash_key(CMAP_Hash* const hash, const CMAP_Char* key, const CMAP_Item** items, CMAP_ULLong occupied, CMAP_ULLong size);
/* Helper functions */
CMAP_Ret_Code cmap_resize(CMAP_Map* this, CMAP_UChar direction);
/* API functions */
CMAP_Ret_Code cmap_init(CMAP_Map* this, CMAP_ULLong size);
CMAP_Ret_Code cmap_set(CMAP_Map* this, const CMAP_Char* key, const CMAP_Any value, CMAP_UShort value_size);
CMAP_Ret_Code cmap_delete(CMAP_Map* this);
CMAP_Ret_Code cmap_get(const CMAP_Item** item, const CMAP_Char* key, CMAP_Map map);
CMAP_Ret_Code cmap_delete_item(const CMAP_Hash* hash, const CMAP_Char* key, CMAP_Map* const this);

/*=======================================================================*/