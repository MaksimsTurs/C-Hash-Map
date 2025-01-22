#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "./Map.h"

unsigned short generate_map_item_index(const char* key, unsigned short map_array_capacity) {
	unsigned short hash = 0;
	while(*key != '\0') {
		hash += (unsigned short)(*key);
		key++;
	}
	return hash % map_array_capacity;
}

unsigned char init_map_array(Map_Array* map_array, unsigned short map_array_capacity) {
	if(map_array == NULL)
		return MAP_ARRAY_IS_NULL_ERROR;

	if(map_array_capacity == 0)
		return MAP_ILLEGAL_ARRAY_SIZE;

	map_array->items = (Map**)calloc(map_array_capacity, sizeof(Map*));
	if(map_array->items == NULL)
		return MAP_MEMALLOCATION_ERROR;

	map_array->capacity = map_array_capacity;
	map_array->occupied = 0;

	return MAP_EXECUTION_SUCCESS;
}

unsigned char insert_to_map_array(Map_Array* map_array, const char* item_key, void* item_value, unsigned short item_value_size) {
	//Try to find collisions.
	unsigned char is_map_array_must_expand = is_map_array_to_small(map_array->occupied, map_array->capacity);
	unsigned short map_item_index = generate_map_item_index(item_key, map_array->capacity);
	if(map_array->items[map_item_index] != NULL)
		return MAP_ARRAY_COLLISION_ERROR;

	//Expand Map_Array->items when needed.
	if(is_map_array_must_expand)
		growth_array_map(map_array);

	unsigned char item_key_length = strlen(item_key) + 1;
	if(item_key_length > MAX_MAP_KEY_LENGTH)
		return MAX_MAP_KEY_LENGTH;

	//| Map | Key | Value
	unsigned char size_of_map = sizeof(Map);
	Map* map_item = (Map*)malloc(size_of_map + item_key_length + item_value_size);
	if(map_item == NULL) 
		return MAP_MEMALLOCATION_ERROR;
			
	char* item_key_position = (char*)map_item + size_of_map;
	void* item_value_position = item_key_position + item_key_length;

	map_item->key = item_key_position;
	map_item->value = item_value_position;

	memmove(map_item->key, item_key, item_key_length);
	memmove(map_item->value, item_value, item_value_size);

	map_array->items[is_map_array_must_expand ? generate_map_item_index(item_key, map_array->capacity) : map_item_index] = map_item;
	map_array->occupied++;

	return MAP_EXECUTION_SUCCESS;
}

unsigned char find_in_map_array(Map_Array* map_array, Map** map, const char* item_key) {
	*map = map_array->items[generate_map_item_index(item_key, map_array->capacity)];
	if(*map == NULL)
		return MAP_ARRAY_ITEM_NOT_FOUND;

	return MAP_EXECUTION_SUCCESS;
}

unsigned char growth_array_map(Map_Array* map_array) {
	unsigned short new_map_items_size = (map_array->capacity + 15 > MAX_MAP_ARRAY_SIZE) ? MAX_MAP_ARRAY_SIZE : map_array->capacity + 15;
	Map** new_items = (Map**)calloc(new_map_items_size, sizeof(Map*));
	if(new_items == NULL)
		return MAP_MEMALLOCATION_ERROR;

	for(unsigned short index = 0; index < map_array->capacity; index++) {
		//Rehash and copy only existed elements
    if(map_array->items[index] != NULL)
			new_items[generate_map_item_index(map_array->items[index]->key, new_map_items_size)] = map_array->items[index];
	}

	free(map_array->items);
  map_array->items = new_items;
	map_array->capacity = new_map_items_size;

	return MAP_EXECUTION_SUCCESS;
}

unsigned char is_map_array_to_small(unsigned short occupied, unsigned short capacity) {
	if(capacity == MAX_MAP_ARRAY_SIZE)
		return 0;
	return ((occupied / capacity) * 100) >= GROWTH_AT;
}

void print_map_array(Map_Array* map_array) {
	printf("MAP REF: %p MAP SIZE: %i MAP COUNT: %i\n", &map_array, map_array->capacity, map_array->occupied);
	for(unsigned short index = 0; index < map_array->capacity; index++) {
		if(map_array->items[index] != NULL) {
			printf(
				"[%i]: (%s) -> (%s, %p, %i)\n", 
				index, 
				(char*)map_array->items[index]->key, 
				(char*)map_array->items[index]->value, 
				map_array->items[index],
				generate_map_item_index(map_array->items[index]->key, map_array->capacity)
			);
		}
	}
}