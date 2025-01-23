#include <stdlib.h>
#include <string.h>

#include "Map.h"

Map_Item_Index generate_map_item_index(const char* item_key, unsigned short map_capacity) {
	if(item_key == NULL)
		return MAP_ILLEGAL_KEY;
	
	unsigned short hash = 0;
	while(*item_key != '\0') {
		hash += (unsigned short)(*item_key);
		item_key++;
	}
	return hash % map_capacity;
}

Map_Exec_Code init_map(Map* map, unsigned short map_capacity) {
	if(map == NULL)
		return MAP_IS_NULL_ERROR;

	if(map_capacity == 0)
		return MAP_ILLEGAL_SIZE;

	map->items = (Map_Item**)calloc(map_capacity, sizeof(Map_Item*));
	if(map->items == NULL)
		return MAP_MEMALLOCATION_ERROR;

	map->capacity = map_capacity;
	map->occupied = 0;

	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code set_to_map(Map* map, const char* item_key, void* item_value, unsigned short item_value_size) {
	if(map == NULL)
		return MAP_IS_NULL_ERROR;
	//Try to find Map_Item with the same item_key.
	//Map_Item was founded, free Map_Item from memory.
	Map_Item_Index map_item_index = generate_map_item_index(item_key, map->capacity);
	if(map->items[map_item_index] != NULL) {
		Map_Exec_Code free_map_item_result = free_map_item(map, NULL, &map_item_index);
		if(free_map_item_result != MAP_EXECUTION_SUCCESS)
			return free_map_item_result;
	}

	//Check if Map must be expand or not.
	unsigned char is_map_must_expand = is_map_to_small(map->occupied, map->capacity);
	if(is_map_must_expand) {
		Map_Exec_Code growth_map_result = growth_map(map);
		if(growth_map_result != MAP_EXECUTION_SUCCESS)
			return growth_map_result;
	}

	//validate Map_Item->key.
	unsigned char item_key_length = strlen(item_key) + 1;
	if(item_key_length > MAX_MAP_KEY_LENGTH)
		return MAP_ILLEGAL_KEY_LENGTH;

	//Save Map_Item, Map_Item->key and Map_Item->value in one memory block [Map_Item | Key | Value]
	unsigned char size_of_map = sizeof(Map);
	Map_Item* map_item = (Map_Item*)malloc(size_of_map + item_key_length + item_value_size);
	if(map_item == NULL) 
		return MAP_MEMALLOCATION_ERROR;

	//Calculate item_key and item_value start position in memory block.
	char* item_key_position = (char*)map_item + size_of_map;
	void* item_value_position = item_key_position + item_key_length;

	map_item->key = item_key_position;
	map_item->value = item_value_position;

	memmove(map_item->key, item_key, item_key_length);
	memmove(map_item->value, item_value, item_value_size);

	//When Map was rehashed then, generate new Map_Item->key for Map_Item.
	map->items[is_map_must_expand ? generate_map_item_index(item_key, map->capacity) : map_item_index] = map_item;
	map->occupied++;
	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code find_in_map(Map* map, Map_Item** map_item, const char* item_key) {
	if(map == NULL)
		return MAP_IS_NULL_ERROR;
		
	*map_item = map->items[generate_map_item_index(item_key, map->capacity)];
	if(*map_item == NULL)
		return MAP_ITEM_NOT_FOUND;
	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code growth_map(Map* map) {
	unsigned short new_map_items_size = (map->capacity + 15 > MAX_MAP_SIZE) ? MAX_MAP_SIZE : map->capacity + 15;
	Map_Item** new_items = (Map_Item**)calloc(new_map_items_size, sizeof(Map*));
	if(new_items == NULL)
		return MAP_MEMALLOCATION_ERROR;

	for(unsigned short index = 0; index < map->capacity; index++) {
		//Rehash and copy only existed elements
    if(map->items[index] != NULL)
			new_items[generate_map_item_index(map->items[index]->key, new_map_items_size)] = map->items[index];
	}

	free(map->items);
  map->items = new_items;
	map->capacity = new_map_items_size;
	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code free_map(Map* map) {
	for(unsigned short index = 0; index < map->capacity; index++) {
		if(map->items[index] != NULL) {
			free(map->items[index]);
			map->items[index] = NULL;
		}	
	}
	free(map);
	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code free_map_item(Map* map, const char* item_key, Map_Item_Index* hash) {
	if(map == NULL && (item_key == NULL || hash == NULL))
		return MAP_FREE_ITEM_ERROR;
	
	Map_Item_Index map_item_index = hash != NULL ? *hash : generate_map_item_index(item_key, map->capacity);

	free(map->items[map_item_index]);
	map->items[map_item_index] = NULL;
	return MAP_EXECUTION_SUCCESS;
}

unsigned char is_map_to_small(unsigned short occupied, unsigned short capacity) {
	if(capacity == MAX_MAP_SIZE)
		return 0;
	return ((occupied / capacity) * 100) >= GROWTH_AT;
}