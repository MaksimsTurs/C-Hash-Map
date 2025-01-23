# C Hash Map
API Description:
\
In case of errors, functions will return a specific code (Map_Exec_Code).
\

Goes through entire string, converts each character to an unsigned short and adds it to an existing hash. At the end, divide the calculated hash by Map->capacity.
```c
Map_Item_Index generate_map_item_index(const char* item_key, unsigned short map_capacity);
```
Allocates memory for Map->items with a certain number, also initializes number of occupied elements in Map (default 0).
```c
Map_Exec_Code init_map(Map* map, unsigned short map_capacity);
```
If a Map_Item with the same key already exists, memory is freed.
Then, if necessary, the map is either enlarged or reduced (in process).
After that, the key for the Map_Item is validated, ONE memory block is allocated for Map_Item, Map_Item->key and Map_Item->value.
If the map has been rehashed, a new hash is generated for the new Map_Item.
```c
Map_Exec_Code set_to_map(Map* map, const char* item_key, void* item_value, unsigned short item_value_size);
```
Generates hash and stores found element in passed Map_Item.
```c
Map_Exec_Code find_in_map(Map* map, Map_Item** map_item, const char* item_key);
```
Allocates new memory with new number of Map_Item, copies all items from old memory to new allocated memory and at the end frees memory from Map->items.
```c
Map_Exec_Code growth_map(Map* map);
```
Frees memory from Map_Item and Map.
```c
Map_Exec_Code free_map(Map* map);
```
Frees memory from a specific Map_Item, you can free memory by passing either the index of the element and the map itself or by passing the map itself and the key of the Map_Item.
```c
Map_Exec_Code free_map_item(Map* map, const char* item_key, Map_Item_Index* hash);
```
Check if map should be enlarged.
```c
unsigned char is_map_to_small(unsigned short occupied, unsigned short capacity);
```

## Example
```c
#include <stdio.h>

#include "Map.h"

int main() {
	Map user = {0};
	Map_Item *age = {0};

	//Init map with 65530 elements.
	init_map(&user, MAX_MAP_SIZE);

	//Set items.
	set_to_map(&user, "_id", "ccf3-r4fw-f23f4gew-f", 22);
	set_to_map(&user, "First name", "Max", 5);
	set_to_map(&user, "Second name", "Bubkin", 6);
	set_to_map(&user, "Age", "22", 3);
	set_to_map(&user, "Age", "23", 3); //Will update "Age"

	//Find "Age" in user and save in age struct.
	find_in_map(&user, &age, "Age");

	//Remove Map_Item["Age"] from "user" Map.
	free_map_item(&user, "Age", NULL);

	return 0;
}
```