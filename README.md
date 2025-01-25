# C Hash Map
Library contains two custom types `Map_Item_Hash` (aka unsigned int) for hash and `Map_Exec_Code` (aka unsigned char) for different execution codes.
## Hashing and collision handle functions
Goes through entire string and calculate a new hash with DJB2 algorithm. At the end, divide the calculated hash by `map_capacity`.
```c
Map_Exec_Code map_generate_hash(Map_Item_Hash* item_hash, Map_Item_Hash map_capacity, const char* item_key);
```
Finds next free hash using linear or quadratic probe (depending on `map->capacity`).
```c
Map_Exec_Code map_find_free_hash(Map_Item_Hash* item_hash, Map* map);
```
Searches for hash of an element in the `map` that has the same `item_key`.
```c
Map_Exec_Code map_find_item_hash_by_key(Map_Item_Hash* item_hash, Map* map, const char* item_key);
```
In based on different `map->capacity` uses different open addressing algorithm.
```c
Map_Exec_Code map_use_hash_algorithm(Map_Item_Hash* item_hash, Map_Item_Hash* index, Map* map);
```
## Collision strategies helper functions
In based on `map_capacity` returns different shrink factor
```c
float get_shrink_factor(Map_Item_Hash map_capacity);
```
In based on `map_capacity` returns different growth factor
```c
float get_growth_factor(Map_Item_Hash map_capacity);
```
Check if map should be enlarged.
```c
unsigned char is_map_to_small(Map_Item_Hash occupied, Map_Item_Hash capacity);
```
Check if map should be reduced.
```c
unsigned char is_map_to_big(Map_Item_Hash occupied, Map_Item_Hash capacity);
```
Returns new map size.
```c
Map_Item_Hash get_prime_from_capacity(Map_Item_Hash map_capacity);
```
## API functions
Allocates memory for `map->item` with a certain number, also initializes number of occupied elements in Map (0).
```c
Map_Exec_Code map_init(Map* map, Map_Item_Hash map_capacity);
```
If element with the same key does not exist, adds new element, if exists, deletes it first and then creates with new value. Increases map if necessary. Allocates a memory block for `map->items`, `item_key` and `item_value`.
```c
Map_Exec_Code map_set_item(Map* map, const char* item_key, void* item_value, unsigned short item_value_size);
```
Find `map_item` with same `item_key`.
```c
Map_Exec_Code map_get_item(Map* map, Map_Item** map_item, const char* item_key);
```
Allocates new memory with new number of Map_Item, copies all items from old memory to new allocated memory and at the end frees memory from Map->items.
```c
Map_Exec_Code growth_map(Map* map);
```
Remove `map` and their elements.
```c
Map_Exec_Code map_free(Map* map);
```
First searches for element with the same `item_key` if correct `item_hash` was not passed and then deletes it from memory, if correct `item_hash` was passed deletes the element directly.
```c
Map_Exec_Code map_free_item(Map* map, Map_Item_Hash* item_hash, const char* item_key);
```
Zooms in or out of the map depending on `direction`.
```c
Map_Exec_Code map_resize(Map* map, unsigned char direction);
```

## Example
```c
#include <string.h>
#include <stdio.h>

#include "Map.h"

void print_map_array(Map* map_array) {
	printf("MAP REF: %p MAP SIZE: %i MAP COUNT: %i\n", &map_array, map_array->capacity, map_array->occupied);
	for(Map_Item_Hash index = 0; index < map_array->capacity; index++) {
		Map_Item_Hash hash = 0;

		if(map_array->items[index] != NULL) {
			map_generate_hash(&hash, map_array->capacity, map_array->items[index]->key);
			printf(
				"[%i]: (%s) -> (%s | %p | %u)\n", 
				index, 
				(char*)map_array->items[index]->key, 
				(char*)map_array->items[index]->value, 
				map_array->items[index], 
				hash
			);
		}
	}
}


int main() {
	Map user = {0};
	Map_Item* age = {0};
	Map_Item* first_name = {0};
	Map_Item* undefined = {0};

	map_init(&user, 3);

	printf("SET RESULT CODE: %i\n", map_set_item(&user, "_id", "123\0", 22));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "First name", "Max\0", 5));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Second name", "Bubkin\0", 8));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Age", "22\0", 4));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Full name", "Max Bubkin\0", 12));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Age", "23\0", 3));

	Map_Exec_Code get_user_age_result = map_get_item(&user, &age, "Age\0");
	printf("SEARCH RESULT CODE: %i SEARCH RESULT: %s -> %s\n", get_user_age_result, age->key, (char*)age->value);
	
	Map_Exec_Code get_user_first_name_result = map_get_item(&user, &first_name, "First name\0");
	printf("SEARCH RESULT CODE: %i SEARCH RESULT: %s -> %s\n", get_user_first_name_result, first_name->key, (char*)first_name->value);

	Map_Exec_Code get_undefined_element = map_get_item(&user, &undefined, "undefined\0");
	printf("SEARCH RESULT CODE: %i\n", get_undefined_element);

	Map_Exec_Code remove_age_result = map_free_item(&user, NULL, "Age\0");
	printf("REMOVE RESULT CODE: %i\n", remove_age_result);
	print_map_array(&user);

	return 0;
}
}
```