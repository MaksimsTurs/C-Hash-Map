# C Hash Map
## Hashing and collision handle functions
Goes through entire string and calculate a new hash with DJB2 algorithm. At the end, divide the calculated hash by `Map_UInt size`.
```c
Map_Return_Code map_generate_hash(Map_Hash* hash, Map_UInt size, Map_CString key);
```

Finds next free hash using linear or quadratic probe (depending on `Map map.size`).
```c
Map_Return_Code map_find_free_hash(Map_Hash* hash, Map map);
```

Searches for hash of an element in the `Map map` that has the same `key`.
```c
Map_Return_Code map_find_hash_by_key(Map_Hash* hash, Map map, Map_CString key);
```

In based on different `Map map.capacity` uses different open addressing algorithm.
```c
Map_Return_Code map_use_hash_algorithm(Map_Hash* hash, Map_Hash* index, Map map);
```

## Collision strategies helper functions
In based on `Map_UInt size` returns different shrink factor
```c
float get_shrink_factor(Map_UInt size);
```

In based on `size` returns different growth factor
```c
float get_growth_factor(Map_UInt size);
```

Check if map should be enlarged.
```c
Map_Bool is_map_to_small(Map_UInt occupied, Map_UInt size);
```

Check if map should be reduced.
```c
Map_Bool is_map_to_big(Map_UInt occupied, Map_UInt size);
```

Returns new map size.
```c
Map_UInt get_prime_from(Map_UInt from);
```

## API functions
Allocates memory for `Map* map->item` with a certain number, also initializes number of occupied elements in Map (0).
```c
Map_Return_Code map_init(Map* map, Map_UInt size);
```

If element with the same key does not exist, adds new element, if exists, deletes it first and then creates with new value. Increases map if necessary. Allocates a memory block for `Map* map->items`, `Map_CString key` and `Map_Any value`.
```c
Map_Return_Code map_set_item(Map* map, Map_CString key, Map_Any value, Map_UShort value_size);
```

Find `Map_Item** item` with same `Map_CString key`.
```c
Map_Return_Code map_get_item(Map map, Map_Item** item, Map_CString key);
```

Allocates new memory with new number of Map_Item, copies all items from old memory to new allocated memory and at the end frees memory from Map->items.
```c
Map_Return_Code map_resize(Map* map, Map_UChar direction);
```

Remove `Map* map` and their elements.
```c
Map_Return_Code map_delete(Map* map);
```

First searches for element with the same `Map_CString key` if correct `Map_Hash* hash` was not passed and then deletes it from memory, if correct `Map_Hash* hash` was passed deletes the element directly.
```c
Map_Return_Code map_delete_item(Map* map, Map_Hash* hash, Map_CString key);
```

Create a smaller array of existed elements in `Map map` in one collection.
```c
Map_Return_Code map_collect(Map map, Map_Collection* iterator);
```

Remove collection.
```c
Map_Return_Code map_delete_collection(Map_Collection* iterator);
```

## Example
```c
#include "Map.h"

#include <stdio.h>

void print_map_array(Map* map_array) {
	printf("MAP REF: %p MAP SIZE: %i MAP COUNT: %i\n", &map_array, map_array->size, map_array->occupied);
	for(Map_UInt index = 0; index < map_array->size; index++) {
		Map_Hash hash = 0;

		if(map_array->items[index] != NULL) {
			map_generate_hash(&hash, map_array->size, map_array->items[index]->key);
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
	Map_Collection user_iterator = {0};

	map_init(&user, 3);

	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Content-Type", "123\0", 22));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "First name", "Max\0", 5));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Second name", "Bubkin\0", 8));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Age", "22\0", 4));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Full name", "Max Bubkin\0", 12));
	printf("SET RESULT CODE: %i\n", map_set_item(&user, "Age", "23\0", 4));
	
	Map_Return_Code get_user_age_result = map_get_item(user, &age, "Age\0");
	printf("SEARCH RESULT CODE: %i SEARCH RESULT: %s -> %s\n", get_user_age_result, age->key, (char*)age->value);
	
	Map_Return_Code get_user_first_name_result = map_get_item(user, &first_name, "First name\0");
	printf("SEARCH RESULT CODE: %i SEARCH RESULT: %s -> %s\n", get_user_first_name_result, first_name->key, (char*)first_name->value);

	Map_Return_Code get_undefined_element = map_get_item(user, &undefined, "undefined\0");
	printf("SEARCH RESULT CODE: %i\n", get_undefined_element);

	print_map_array(&user);
	Map_Return_Code remove_age_result = map_delete_item(&user, NULL, "Age\0");
	printf("REMOVE RESULT CODE: %i\n", remove_age_result);
	print_map_array(&user);

	map_collect(user, &user_iterator);

	for(int index = 0; index < user_iterator.size ; index++) {
		printf("%s %s\n", user_iterator.items[index]->key, user_iterator.items[index]->value);
	}
	
	return 0;
}
```