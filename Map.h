#pragma once
/*=======================================================================*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./libs/SBString.h"
/*=======================================================================*/
typedef unsigned char      Map_Return_Code;
typedef unsigned char      Map_Bool;
typedef unsigned char      Map_UChar;
typedef char               Map_Char;
typedef char*              Map_String;

typedef void*              Map_Any;

typedef unsigned int       Map_UInt;
typedef unsigned int       Map_Hash;

typedef unsigned short     Map_UShort;

typedef unsigned long long Map_ULLong;
/*=======================================================================*/
// Restriction constants
#define MAP_MAX_SIZE                       (Map_ULLong)4000000000
#define MAP_MAX_KEY_LENGTH                 (Map_UChar)64
// Error/success constants.
#define MAP_ERROR_MEMALLOC                 (Map_UChar)0
#define MAP_ERROR_OVERFLOW                 (Map_UChar)1
#define MAP_ERROR_ITEM_NOT_FOUND           (Map_UChar)2
#define MAP_ERROR_INVALID_RESIZE_DIRECTION (Map_UChar)3
#define MAP_ERROR_INVALID_KEY              (Map_UChar)4
#define MAP_ERROR_INVALID_SIZE             (Map_UChar)5
#define MAP_ERROR_INVALID_KEY_LENGTH       (Map_UChar)6
#define MAP_ERROR_INVALID_PTR              (Map_UChar)7
#define MAP_SUCCESS                        (Map_UChar)8
// Map resize constants
#define MAP_KEY_GROWTH_SIZE                (Map_UChar)1
#define MAP_KEY_SHRINK_SIZE                (Map_UChar)2
// Map factors and size constants
#define MAP_SMALL_SIZE                     (Map_UShort)2000
#define MAP_SMALL_GROWTH_AT                (float)0.7
#define MAP_SMALL_SHRINK_AT                (float)0.4
#define MAP_MEDIUM_SIZE                    (Map_UShort)20000
#define MAP_MEDIUM_GROWTH_AT               (float)0.8
#define MAP_MEDIUM_SHRINK_AT               (float)0.5
#define MAP_BIG_SIZE                       (Map_UShort)200000
#define MAP_BIG_GROWTH_AT                  (float)0.9
#define MAP_BIG_SHRINK_AT                  (float)0.5
#define MAP_TRUE                           (Map_UChar)1
#define MAP_FALSE                          (Map_UChar)0
/*=======================================================================*/
typedef struct Map_Item {
	Map_String key;
	Map_Any value;
} Map_Item;

typedef struct Map_Collection {
	struct Map_Item** items;
	Map_UInt size;
} Map_Collection;

typedef struct Map {
	struct Map_Item** items;
	Map_UInt size;
	Map_UInt occupied;
} Map;
/*=======================================================================*/
/* Hashing and Collision handle functions */
Map_Return_Code map_generate_hash(Map_Hash* hash, const Map_UInt size, const Map_String key);
Map_Return_Code map_find_free_hash(Map_Hash* hash, const Map map);
Map_Return_Code map_find_hash_by_key(Map_Hash* hash, const Map map, const Map_String key);
Map_Return_Code map_use_hash_algorithm(Map_Hash* hash, Map_Hash* index, const Map map);
/* API functions */
Map_Return_Code map_init(Map* map, const Map_UInt size);
Map_Return_Code map_set_item(Map* map, const Map_String key, const Map_Any value, const Map_UShort value_size);
Map_Return_Code map_get_item(Map_Item** item, const Map map, const Map_String key);
Map_Return_Code map_delete(Map* map);
Map_Return_Code map_delete_item(Map* map, const Map_Hash* hash, const Map_String key);
Map_Return_Code map_resize(Map* map, const Map_UChar direction);
Map_Return_Code map_collect(Map_Collection* collection, const Map map);
Map_Return_Code map_delete_collection(Map_Collection* iterator);
/* Collision strategies helper functions */
float get_shrink_factor(const Map_UInt size);
float get_growth_factor(const Map_UInt size);
Map_UInt get_prime_from(const Map_UInt from);
Map_Bool is_map_to_small(const Map_UInt occupied, const Map_UInt size);
Map_Bool is_map_to_big(const Map_UInt occupied, const Map_UInt size);
/* Utility functions */
void map_reset_hash_if(Map_Hash* hash, const Map_UInt size);
/*=======================================================================*/