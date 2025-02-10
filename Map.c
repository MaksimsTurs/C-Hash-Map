#include "Map.h"

/*
############################################
   Hashing and Collision handle functions
###########################################
*/

Map_Return_Code map_generate_hash(Map_Hash* hash, Map_UInt size, Map_CString key) {
	if(hash == NULL || key == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;
	
	Map_Hash local_hash = 5381;
	
	while(*key++)
		local_hash = ((local_hash << 5) + local_hash) + (char)(*key);
	*hash = local_hash % size;
	
	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_find_free_hash(Map_Hash* hash, Map map) {
	if(hash == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;
	else if(map.occupied == MAP_MAX_SIZE)
		return MAP_ERROR_OVERFLOW;

	Map_UInt index = 0;
	Map_Hash local_hash = *hash;

	while(map.items[local_hash] != NULL) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		map_reset_hash_if(&local_hash, map.size);
		map_use_hash_algorithm(&local_hash, &index, map);
	}

	*hash = local_hash;

	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_find_hash_by_key(Map_Hash* hash, Map map, Map_CString key) {
	if(hash == NULL || key == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;
	
	Map_UInt index = 1;
	Map_UInt probe_count = 0;
	Map_Hash local_hash = *hash;
	Map_Return_Code exec_code = 0;

	// In worst case, if we have two different keys with two identical hashes,
	// this loop will go through the entire map.
	while(probe_count <= map.size) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		map_reset_hash_if(&local_hash, map.size);
		if(map.items[local_hash] != NULL && strcmp(key, map.items[local_hash]->key) == 0) {
			// Hash was found.
			*hash = local_hash;
			return MAP_EXECUTION_SUCCESS;
		}
		// Hash was not found, calculate next hash.
		exec_code = map_use_hash_algorithm(&local_hash, &index, map);
		if(exec_code != MAP_EXECUTION_SUCCESS)
			return exec_code;
		probe_count++;
	}

	// Hash was not found.
	return MAP_ERROR_ITEM_NOT_FOUND;
}

Map_Return_Code map_use_hash_algorithm(Map_Hash* hash, Map_Hash* index, Map map) {
	if(hash == NULL || 
		// If map is bigger than MAP_SMALL_SIZE and index was not passed
		// you can not use Quadratic probe.
		(map.size > MAP_SMALL_SIZE && index == NULL))
		return MAP_ERROR_ILLEGAL_PARAM;

	if(map.size <= MAP_SMALL_SIZE) {
		// Linear probe.
		*hash += 1;
	}	else {
		//Quadratic probe.
		*hash = (*hash + (*index ^ 2)) % map.size;
		*index += 1;
	}	

	return MAP_EXECUTION_SUCCESS;
}

/*
	############################################
		 Collision strategies helper functions
	###########################################
*/

float get_shrink_factor(Map_UInt size) {
	if(size >= MAP_BIG_SIZE)
		return MAP_BIG_SHRINK_AT;
	else if(size >= MAP_MEDIUM_SIZE)
		return MAP_MEDIUM_SHRINK_AT;
	else if(size <= MAP_SMALL_SIZE)
		return MAP_SMALL_SHRINK_AT;
	return MAP_SMALL_SHRINK_AT;
}

float get_growth_factor(Map_UInt size) {
	if(size >= MAP_BIG_SIZE)
		return MAP_BIG_GROWTH_AT;
	else if(size >= MAP_MEDIUM_SIZE)
		return MAP_MEDIUM_GROWTH_AT;
	else if(size <= MAP_SMALL_SIZE)
		return MAP_SMALL_GROWTH_AT;
	return MAP_SMALL_GROWTH_AT;
}

Map_UInt get_prime_from(Map_UInt from) {
	return (from * 2) >= MAP_MAX_SIZE ? MAP_MAX_SIZE : from * 2;
}

Map_Bool is_map_to_small(Map_UInt occupied, Map_UInt size) {
	if(occupied == MAP_MAX_SIZE || occupied == 0)
		return 0;
	return ((float)occupied / size) >= get_growth_factor(size);
}

Map_Bool is_map_to_big(Map_UInt occupied, Map_UInt size) {
	if(occupied == MAP_MAX_SIZE || occupied == 0)
		return 0;
	return ((float)occupied / size) <= get_shrink_factor(size);
}

/*
	############################################
								API functions
	###########################################
*/

Map_Return_Code map_collect(Map map, Map_Collection* iterator) {
	Map_UInt iterator_index = 0;

	iterator->items = malloc(map.occupied * sizeof(Map_Item*));
	if(iterator->items == NULL)
		return MAP_ERROR_MEMALLOCATION;

	for(Map_UInt index = 0; index < map.size; index++) {
		if(map.items[index] != NULL) {
			iterator->items[iterator_index] = map.items[index];
			iterator_index++;
		}
	}

	iterator->size = map.occupied;

	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_delete_collection(Map_Collection* iterator) {
	if(iterator->items == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;

	free(iterator->items);
	
	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_init(Map* map, Map_UInt size) {
	if(map == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;
	else if(size == 0)
		return MAP_ERROR_ILLEGAL_SIZE;

	map->items = (Map_Item**)calloc(size, sizeof(Map_Item*));
	if(map->items == NULL)
		return MAP_ERROR_MEMALLOCATION;

	map->size = size;
	map->occupied = 0;

	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_set_item(Map* map, Map_CString key, Map_Any value, Map_UShort value_size) {
	if(map == NULL || key == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;
	else if(map->occupied >= MAP_MAX_SIZE)
		return MAP_ERROR_OVERFLOW;

	// Map element is stored together with its key and value to 
	// reduce amount of memory allocation and memory free execution [Map | Key | Value].
	Map_Item* item = NULL;
	Map_Hash item_hash = 0;
	Map_Return_Code exec_code = 0;
	Map_UChar key_length = strlen(key) + 1;
	Map_UChar size_of_map = sizeof(Map_Item);
	if(key_length > MAP_MAX_KEY_LENGTH)
		return MAP_ERROR_ILLEGAL_KEY_LENGTH;

	// Rehash map elements and enlarge map if necessary.
	if(is_map_to_small(map->occupied + 1, map->size)) {
		exec_code = map_resize(map, MAP_KEY_GROWTH_SIZE);
		if(exec_code != MAP_EXECUTION_SUCCESS)
			return exec_code;
	} 

	exec_code = map_generate_hash(&item_hash, map->size, key);
	if(exec_code != MAP_EXECUTION_SUCCESS)
		return exec_code;

	// Collision was found.
	if(map->items[item_hash] != NULL) {
		// Check if there is an element with the same key.
		exec_code = map_find_hash_by_key(&item_hash, *map, key);
		// Element was not found and executuin was not ended successfuly.
		if(exec_code != MAP_EXECUTION_SUCCESS && exec_code != MAP_ERROR_ITEM_NOT_FOUND)
			return exec_code;

		if(exec_code == MAP_EXECUTION_SUCCESS) {
			// Element with the same key was found, realloc memory block, 
			// and save new value.
			item = (Map_Item*)realloc(map->items[item_hash], size_of_map + key_length + value_size);
			if(item == NULL)
				return MAP_ERROR_MEMALLOCATION;
		} else {
			// Element with same key was not found but hashes are equal.
			// Find next free hash for new element.
			exec_code = map_find_free_hash(&item_hash, *map);
			if(exec_code != MAP_EXECUTION_SUCCESS)
				return exec_code;
			item = (Map_Item*)malloc(size_of_map + key_length + value_size);
			if(item == NULL)
				return MAP_ERROR_MEMALLOCATION;
			map->occupied++;
		}
	} else {
		// No collision was found, alloc memory for new element.
		item = (Map_Item*)malloc(size_of_map + key_length + value_size);
		if(item == NULL)
			return MAP_ERROR_MEMALLOCATION;
		map->occupied++;
	}

	// Calculating the start position in memory block for key and value.
	Map_String item_key_position = (Map_String)item + size_of_map;
	Map_Any item_value_position = (Map_Any)item_key_position + key_length;

	item->key = item_key_position;
	item->value = item_value_position;

	memmove((Map_Any)item->key, key, key_length);
	memmove(item->value, value, value_size);

	map->items[item_hash] = item;
	
	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_resize(Map* map, Map_UChar direction) {
	if(map == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;

	Map_UInt old_capacity = map->size;
	Map_Hash hash = 0;
	Map_Return_Code exec_code = 0;
	Map_Item** new_items = {0};

	if(direction == MAP_KEY_GROWTH_SIZE)
		map->size = get_prime_from(map->size);
	else if(direction == MAP_KEY_SHRINK_SIZE)
		map->size = get_prime_from(map->occupied);
	else
		return MAP_ERROR_ILLEGAL_PARAM;

	new_items = (Map_Item**)calloc(map->size, sizeof(Map_Item*));
	if(new_items == NULL)
		return MAP_ERROR_MEMALLOCATION;

	for(Map_UInt index = 0; index < old_capacity; index++) {
		// Copy all existed elements in map.
    if(map->items[index] != NULL) {
			exec_code = map_generate_hash(&hash, map->size, map->items[index]->key);
			if(exec_code != MAP_EXECUTION_SUCCESS)
				return exec_code;
			// Check if elements with same key are exist in new map.
			// When exist, find free hash.
			if(new_items[hash] != NULL) {
				exec_code = map_find_free_hash(&hash, *map);
				if(exec_code != MAP_EXECUTION_SUCCESS)
					return exec_code;
			}

			new_items[hash] = map->items[index];
		}
	}

	free(map->items);
  map->items = new_items;

	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_get_item(Map map, Map_Item** item, Map_CString key) {
	if(item == NULL || key == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;

	Map_Hash hash = 0;
	Map_Return_Code exec_code = 0;

	// Get start position.
	exec_code = map_generate_hash(&hash, map.size, key);
	if(exec_code != MAP_EXECUTION_SUCCESS) {
		*item = NULL;
		return exec_code;
	}
	
	// Find element with same key from start position.
	exec_code = map_find_hash_by_key(&hash, map, key);
	if(exec_code != MAP_EXECUTION_SUCCESS) {
		*item = NULL;
		return exec_code;
	}

	*item = map.items[hash];

	return *item == NULL ? MAP_ERROR_ITEM_NOT_FOUND : MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_delete(Map* map) {
	for(Map_UInt index = 0; index < map->size; index++) {
		if(map->items[index] != NULL) {
			free(map->items[index]);
			map->items[index] = NULL;
		}	
	}

	return MAP_EXECUTION_SUCCESS;
}

Map_Return_Code map_delete_item(Map* map, Map_Hash* hash, Map_CString key) {
	if(map == NULL && key == NULL)
		return MAP_ERROR_ILLEGAL_PARAM;
	
	Map_Hash local_hash = hash == NULL ? 0 : *hash;
	Map_Return_Code exec_code = 0;

	if(hash != NULL && map->items[local_hash] == NULL) {
		return MAP_ERROR_ITEM_NOT_FOUND;
	} else if(hash != NULL && map->items[local_hash] != NULL) {
		// Delete map item by defined hash.
		free(map->items[local_hash]);
		map->items[local_hash] = NULL;
		map->occupied--;
	} else {	
		// Delete map item by key.
		exec_code = map_generate_hash(&local_hash, map->size, key);
		if(exec_code != MAP_EXECUTION_SUCCESS)
			return exec_code;
		
		exec_code = map_find_hash_by_key(&local_hash, *map, key);
		if(exec_code != MAP_EXECUTION_SUCCESS)
			return exec_code;

		free(map->items[local_hash]);
		map->items[local_hash] = NULL;
		map->occupied--;
	}

	if(is_map_to_big(map->occupied, map->size)) {
		exec_code = map_resize(map, MAP_KEY_SHRINK_SIZE);
		if(exec_code != MAP_EXECUTION_SUCCESS)
			return exec_code;
	}

	return MAP_EXECUTION_SUCCESS;
}

/*
	############################################
							Utility functions
	###########################################
*/

void map_reset_hash_if(Map_Hash* hash, Map_UInt size) {
	if(*hash + 1 >= size)
		*hash = 0;
}