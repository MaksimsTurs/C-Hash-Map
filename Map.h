#define MAX_MAP_SIZE 65530
#define MAX_MAP_KEY_LENGTH 64

#define MAP_MEMALLOCATION_ERROR 0
#define MAP_COLLISION_ERROR 1
#define MAP_OVERFLOW_ERROR 2
#define MAP_IS_NULL_ERROR 3
#define MAP_ITEM_NOT_FOUND 5
#define MAP_ILLEGAL_SIZE 6
#define MAP_ILLEGAL_KEY_LENGTH 7
#define MAP_ILLEGAL_KEY 8
#define MAP_FREE_ITEM_ERROR 9
#define MAP_EXECUTION_SUCCESS 10

#define MAP_CAPACITIY_EXPAND_ON 15
#define GROWTH_AT 0.85
#define SHRINK_AT 0.3

typedef unsigned char Map_Exec_Code;
typedef unsigned short Map_Item_Index;

typedef struct Map_Item {
	char* key;
	void* value;
} Map_Item;

typedef struct Map {
	struct Map_Item** items;
	unsigned short capacity;
	unsigned short occupied;
} Map;

Map_Item_Index generate_map_item_index(const char* item_key, unsigned short map_capacity);
Map_Exec_Code init_map(Map* map, unsigned short map_capacity);
Map_Exec_Code set_to_map(Map* map, const char* item_key, void* item_value, unsigned short item_value_size);
Map_Exec_Code find_in_map(Map* map, Map_Item** map_item, const char* item_key);
Map_Exec_Code free_map(Map* map);
/*You can remove map with set of parameters, map + item_key or map + hash.*/
Map_Exec_Code free_map_item(Map* map, const char* item_key, Map_Item_Index* hash);
Map_Exec_Code growth_map(Map* map);

unsigned char is_map_to_small(unsigned short occupied, unsigned short capacity);