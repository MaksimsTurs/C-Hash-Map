#include <stdlib.h>
#include <string.h>

// Restriction constants
#define MAP_MAX_SIZE 4000000000
#define MAP_MAX_KEY_LENGTH 64

// Error constants
#define MAP_ERROR_MEMALLOCATION 0
#define MAP_ERROR_OVERFLOW 1
#define MAP_ERROR_ITEM_NOT_FOUND 2
#define MAP_ERROR_ILLEGAL_KEY 3
#define MAP_ERROR_ILLEGAL_SIZE 4
#define MAP_ERROR_ILLEGAL_KEY_LENGTH 5
#define MAP_ERROR_ILLEGAL_PARAM 6
#define MAP_EXECUTION_SUCCESS 7

// Map resize constants
#define MAP_KEY_GROWTH_SIZE 1
#define MAP_KEY_SHRINK_SIZE 2

// Map factors and size constants
#define MAP_SMALL_SIZE 2000
#define MAP_SMALL_GROWTH_AT 0.7
#define MAP_SMALL_SHRINK_AT 0.4
#define MAP_MEDIUM_SIZE 20000
#define MAP_MEDIUM_GROWTH_AT 0.8
#define MAP_MEDIUM_SHRINK_AT 0.5
#define MAP_BIG_SIZE 200000
#define MAP_BIG_GROWTH_AT 0.9
#define MAP_BIG_SHRINK_AT 0.5

#define MAP_TRUE 1
#define MAP_FALSE 0

/*===========================================*/
typedef unsigned char Map_Return_Code;
typedef unsigned char Map_Bool;
typedef unsigned char Map_UChar;
/*===========================================*/
typedef const char* Map_CString;
typedef char* Map_String;
/*===========================================*/
typedef void* Map_Any;
/*===========================================*/
typedef unsigned int Map_UInt;
typedef unsigned int Map_Hash;
/*===========================================*/
typedef unsigned short Map_UShort;
/*===========================================*/

typedef struct Map_Item {
	Map_CString key;
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

/* Hashing and Collision handle functions */
Map_Return_Code map_generate_hash(Map_Hash* hash, Map_UInt size, Map_CString key);
Map_Return_Code map_find_free_hash(Map_Hash* hash, Map map);
Map_Return_Code map_find_hash_by_key(Map_Hash* hash, Map map, Map_CString key);
Map_Return_Code map_use_hash_algorithm(Map_Hash* hash, Map_Hash* index, Map map);
/* API functions */
Map_Return_Code map_init(Map* map, Map_UInt size);
Map_Return_Code map_set_item(Map* map, Map_CString key, Map_Any value, Map_UShort value_size);
Map_Return_Code map_get_item(Map map, Map_Item** item, Map_CString key);
Map_Return_Code map_delete(Map* map);
Map_Return_Code map_delete_item(Map* map, Map_Hash* hash, Map_CString key);
Map_Return_Code map_resize(Map* map, Map_UChar direction);
Map_Return_Code map_collect(Map map, Map_Collection* iterator);
Map_Return_Code map_delete_collection(Map_Collection* iterator);
/* Collision strategies helper functions */
float get_shrink_factor(Map_UInt size);
float get_growth_factor(Map_UInt size);
Map_Bool is_map_to_small(Map_UInt occupied, Map_UInt size);
Map_Bool is_map_to_big(Map_UInt occupied, Map_UInt size);
Map_UInt get_prime_from(Map_UInt from);
/* Utility functions */
void map_reset_hash_if(Map_Hash* hash, Map_UInt size);