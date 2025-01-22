/*##################################################*/
#define MAX_MAP_ARRAY_SIZE 65530
#define MAX_MAP_KEY_LENGTH 64
/*##################################################*/
#define MAP_MEMALLOCATION_ERROR 0
#define MAP_ARRAY_COLLISION_ERROR 1
#define MAP_ARRAY_OVERFLOW_ERROR 2
#define MAP_ARRAY_IS_NULL_ERROR 3
#define MAP_ARRAY_ITEM_NOT_FOUND 5
#define MAP_ILLEGAL_ARRAY_SIZE 6
#define MAP_ILLEGAL_KEY_LENGTH 7
#define MAP_EXECUTION_SUCCESS 8
/*##################################################*/
#define MAP_CAPACITIY_EXPAND_ON 15
#define GROWTH_AT 0.85
#define SHRINK_AT 0.3

typedef struct Map {
	char* key;
	void* value;
} Map;

typedef struct Map_Array {
	struct Map** items;
	unsigned short capacity;
	unsigned short occupied;
} Map_Array;

unsigned short generate_map_item_index(const char*, unsigned short);
unsigned char init_map_array(Map_Array*, unsigned short);
unsigned char insert_to_map_array(Map_Array*, const char*, void*, unsigned short);
unsigned char find_in_map_array(Map_Array*, Map**, const char*);
unsigned char growth_array_map(Map_Array*);

unsigned char is_map_array_to_small(unsigned short, unsigned short);
void print_map_array(Map_Array*);