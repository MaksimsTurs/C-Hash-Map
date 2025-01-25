#define MAX_MAP_SIZE 4000000000
#define MAX_MAP_KEY_LENGTH 64

#define MAP_MEMALLOCATION_ERROR 0
#define MAP_OVERFLOW_ERROR 1
#define MAP_ITEM_NOT_FOUND_ERROR 3
#define MAP_ILLEGAL_KEY_ERROR 4
#define MAP_ILLEGAL_SIZE_ERROR 5
#define MAP_ILLEGAL_KEY_LENGTH_ERROR 6
#define MAP_EXECUTION_SUCCESS 8
#define MAP_FUNC_ILLEGAL_PARAM 9

#define MAP_KEY_GROWTH_SIZE 1
#define MAP_KEY_SHRINK_SIZE 2

#define MAP_SMALL_SIZE 2000
#define MAP_SMALL_GROWTH_AT 0.7
#define MAP_SMALL_SHRINK_AT 0.4
#define MAP_MEDIUM_SIZE 20000
#define MAP_MEDIUM_GROWTH_AT 0.8
#define MAP_MEDIUM_SHRINK_AT 0.5
#define MAP_BIG_SIZE 200000
#define MAP_BIG_GROWTH_AT 0.9
#define MAP_BIG_SHRINK_AT 0.5

typedef unsigned char Map_Exec_Code;
typedef unsigned int Map_Item_Hash;

typedef struct Map_Item {
	char* key;
	void* value;
} Map_Item;

typedef struct Map {
	struct Map_Item** items;
	Map_Item_Hash capacity;
	Map_Item_Hash occupied;
} Map;

/* Hashing and Collision handle functions */
Map_Exec_Code map_generate_hash(Map_Item_Hash* item_hash, Map_Item_Hash map_capacity, const char* item_key);
Map_Exec_Code map_find_free_hash(Map_Item_Hash* item_hash, Map* map);
Map_Exec_Code map_find_item_hash_by_key(Map_Item_Hash* item_hash, Map* map, const char* item_key);
Map_Exec_Code map_use_hash_algorithm(Map_Item_Hash* item_hash, Map_Item_Hash* index, Map* map);
/* Collision strategies helper functions */
float get_shrink_factor(Map_Item_Hash map_capacity);
float get_growth_factor(Map_Item_Hash map_capacity);
unsigned char is_map_to_small(Map_Item_Hash occupied, Map_Item_Hash capacity);
unsigned char is_map_to_big(Map_Item_Hash occupied, Map_Item_Hash capacity);
Map_Item_Hash get_prime_from_capacity(Map_Item_Hash map_capacity);
/* API functions */
Map_Exec_Code map_init(Map* map, Map_Item_Hash map_capacity);
Map_Exec_Code map_set_item(Map* map, const char* item_key, void* item_value, unsigned short item_value_size);
Map_Exec_Code map_get_item(Map* map, Map_Item** map_item, const char* item_key);
Map_Exec_Code map_free(Map* map);
Map_Exec_Code map_free_item(Map* map, Map_Item_Hash* item_hash, const char* item_key);
Map_Exec_Code map_resize(Map* map, unsigned char direction);