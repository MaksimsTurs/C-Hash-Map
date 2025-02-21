#pragma once
/*=======================================================================*/

#include <stdlib.h>

#include "./include/CString.h"

/*=======================================================================*/

typedef unsigned char      CMAP_Ret_Code;
typedef unsigned char      CMAP_Bool;
typedef unsigned char      CMAP_UChar;
typedef char               CMAP_Char;
typedef char*              CMAP_Char_Ptr;

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
#define CMAP_ERROR_INVALID_SIZE             (CMAP_UChar)5
#define CMAP_ERROR_INVALID_KEY_LENGTH       (CMAP_UChar)6
#define CMAP_ERROR_INVALID_PTR              (CMAP_UChar)7
#define CMAP_ITEM_FOUND                     (CMAP_UChar)8
#define CMAP_SUCCESS                        (CMAP_UChar)9
// Map resize constants
#define CMAP_KEY_GROWTH_SIZE                (CMAP_UChar)1
#define CMAP_KEY_SHRINK_SIZE                (CMAP_UChar)2
// Map factors and size constants
#define CMAP_SMALL_SIZE                     (CMAP_UShort)2000
#define CMAP_SMALL_GROWTH_AT                (CMAP_Float)0.7
#define CMAP_SMALL_SHRINK_AT                (CMAP_Float)0.4
#define CMAP_MEDIUM_SIZE                    (CMAP_UShort)20000
#define CMAP_MEDIUM_GROWTH_AT               (CMAP_Float)0.8
#define CMAP_MEDIUM_SHRINK_AT               (CMAP_Float)0.5
#define CMAP_BIG_SIZE                       (CMAP_UShort)200000
#define CMAP_BIG_GROWTH_AT                  (CMAP_Float)0.9
#define CMAP_BIG_SHRINK_AT                  (CMAP_Float)0.5
#define CMAP_TRUE                           (CMAP_UChar)1
#define CMAP_FALSE                          (CMAP_UChar)0
// Map characters
#define CMAP_NULL_TERMINATOR                '\0'

// Map utils macros.
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
	if (exec_code != CMAP_SUCCESS) return exec_code; \
} while (0);
#define CMAP_GET_SHRINK_FACTOR(size) size >= CMAP_BIG_SIZE    ? CMAP_BIG_SHRINK_AT    :                      \
																		 size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_SHRINK_AT :                      \
																		 size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_SHRINK_AT  : CMAP_SMALL_SHRINK_AT
#define CMAP_GET_GROWTH_FACTOR(size) size >= CMAP_BIG_SIZE    ? CMAP_BIG_GROWTH_AT    :                      \
																		 size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_GROWTH_AT :                      \
																		 size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_GROWTH_AT  : CMAP_SMALL_GROWTH_AT
#define CMAP_GET_PRIME_FROM(x)               (x * 2) >= CMAP_MAX_SIZE ? CMAP_MAX_SIZE : x * 2;
#define CMAP_IS_MAP_TO_SMALL(occupied, size) ((CMAP_Float)occupied / size) >= (CMAP_GET_GROWTH_FACTOR(size))
#define CMAP_IS_MAP_TO_BIG(occupied, size)   ((CMAP_Float)occupied / size) <= (CMAP_GET_SHRINK_FACTOR(size))

/*=======================================================================*/

typedef struct CMAP_Item {
	CMAP_Char_Ptr key;
	CMAP_Any value;
} CMAP_Item;

typedef struct CMAP_Map {
	CMAP_Item** items;
	CMAP_ULLong size;
	CMAP_ULLong occupied;
} CMAP_Map;

/*=======================================================================*/

/* Hashing and Collision handle functions */
CMAP_Ret_Code cmap_gen_hash(CMAP_Hash* const hash, CMAP_Hash const size, CMAP_Char_Ptr const key);
CMAP_Ret_Code cmap_find_hash(CMAP_Hash* const hash, CMAP_Item** const items, CMAP_ULLong const occupied, CMAP_ULLong const size);
CMAP_Ret_Code cmap_find_hash_key(CMAP_Hash* const hash, CMAP_Item** const items, CMAP_ULLong const occupied, CMAP_ULLong const size, CMAP_Char_Ptr const key);
/* API functions */
CMAP_Ret_Code cmap_init(CMAP_Map* const this, CMAP_ULLong const size);
CMAP_Ret_Code cmap_resize(CMAP_Map* const this, CMAP_UChar const direction);
CMAP_Ret_Code cmap_set(CMAP_Map* const this, CMAP_Char_Ptr const key, CMAP_Any const value, CMAP_UShort const value_size);
CMAP_Ret_Code cmap_delete(CMAP_Map* const this);
CMAP_Ret_Code cmap_get(CMAP_Item** const item, CMAP_Map const map, CMAP_Char_Ptr const key);
CMAP_Ret_Code cmap_delete_item(CMAP_Map* const this, CMAP_Hash* const hash, CMAP_Char_Ptr const key);

/*=======================================================================*/