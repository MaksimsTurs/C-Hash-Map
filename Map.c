#include <stdlib.h>
#include <string.h>

#include "Map.h"

/*
	############################################
		Hashing and Collision handle functions
	###########################################
*/

Map_Exec_Code map_generate_hash(Map_Item_Hash* item_hash, Map_Item_Hash map_capacity, const char* item_key) {
	if(item_hash == NULL || item_key == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	}
	
	*item_hash = 5381;	

	while(*item_key++) {
		*item_hash = ((*item_hash << 5) + *item_hash) + (char)(*item_key);
	}

	*item_hash = *item_hash % map_capacity;

	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code map_find_free_hash(Map_Item_Hash* item_hash, Map* map) {
	if(map == NULL || item_hash == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	} else if(map->occupied == MAX_MAP_SIZE) {
		return MAP_OVERFLOW_ERROR;
	}

	Map_Item_Hash index = 0;

	while(map->items[*item_hash] != NULL) {
		// Because iteration can start from any location, I reset item_hash to iterate from the start again.
		if(*item_hash + 1 >= map->capacity) {
			*item_hash = 0;
		}
		map_use_hash_algorithm(item_hash, &index, map);
	}

	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code map_find_item_hash_by_key(Map_Item_Hash* item_hash, Map* map, const char* item_key) {
	if(item_hash == NULL || map == NULL || item_key == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	}
	
	Map_Item_Hash index = 1;
	Map_Item_Hash probe_count = 0;

	// In worst case, if we have two different keys with two identical hashes,
	// this loop will go through the entire map.
	while(probe_count <= map->capacity) {
		// Because iteration can start from any location, I reset item_hash to iterate from the start again.
		if(*item_hash + 1 >= map->capacity) {
			*item_hash = 0;
		}
		if(map->items[*item_hash] != NULL && strcmp(item_key, map->items[*item_hash]->key) == 0) {
			return MAP_EXECUTION_SUCCESS;
		}
		// Recalculate item_hash.
		Map_Exec_Code hash_calc_result = map_use_hash_algorithm(item_hash, &index, map);
		probe_count++;
		if(hash_calc_result != MAP_EXECUTION_SUCCESS) {
			return hash_calc_result;
		}
	}
	return MAP_ITEM_NOT_FOUND_ERROR;
}

Map_Exec_Code map_use_hash_algorithm(Map_Item_Hash* item_hash, Map_Item_Hash* index, Map* map) {
	if(item_hash == NULL || map == NULL || (map->capacity > MAP_SMALL_SIZE && index == NULL)) {
		return MAP_FUNC_ILLEGAL_PARAM;
	}

	if(map->capacity <= MAP_SMALL_SIZE) {
		// Linear probe.
		*item_hash += 1;
	}	else {
		//Quadratic probe.
		*item_hash = (*item_hash + (*index ^ 2)) % map->capacity;
		*index += 1;
	}	
	return MAP_EXECUTION_SUCCESS;
}

/*
	############################################
		 Collision strategies helper functions
	###########################################
*/

float get_shrink_factor(Map_Item_Hash map_capacity) {
	if(map_capacity >= MAP_BIG_SIZE) {
		return MAP_BIG_SHRINK_AT;
	} else if(map_capacity >= MAP_MEDIUM_SIZE) {
		return MAP_MEDIUM_SHRINK_AT;
	} else if(map_capacity <= MAP_SMALL_SIZE) {
		return MAP_SMALL_SHRINK_AT;
	}
	return MAP_SMALL_SHRINK_AT;
}

float get_growth_factor(Map_Item_Hash map_capacity) {
	if(map_capacity >= MAP_BIG_SIZE) {
		return MAP_BIG_GROWTH_AT;
	} else if(map_capacity >= MAP_MEDIUM_SIZE) {
		return MAP_MEDIUM_GROWTH_AT;
	} else if(map_capacity <= MAP_SMALL_SIZE) {
		return MAP_SMALL_GROWTH_AT;
	}
	return MAP_SMALL_GROWTH_AT;
}

Map_Item_Hash get_prime_from_capacity(Map_Item_Hash map_capacity) {
	return (map_capacity * 2) > MAX_MAP_SIZE ? MAX_MAP_SIZE : map_capacity * 2;
}

unsigned char is_map_to_small(Map_Item_Hash occupied, Map_Item_Hash capacity) {
	if(occupied == MAX_MAP_SIZE || occupied == 0) {
		return 0;
	}
	return ((float)occupied / capacity) >= get_growth_factor(capacity);
}

unsigned char is_map_to_big(Map_Item_Hash occupied, Map_Item_Hash capacity) {
	if(occupied == MAX_MAP_SIZE || occupied == 0) {
		return 0;
	}
	return ((float)occupied / capacity) <= get_shrink_factor(capacity);
}

/*
	############################################
								API functions
	###########################################
*/

Map_Exec_Code map_init(Map* map, Map_Item_Hash map_capacity) {
	if(map == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	} else if(map_capacity == 0) {
		return MAP_ILLEGAL_SIZE_ERROR;
	}

	map->items = (Map_Item**)calloc(map_capacity, sizeof(Map_Item*));
	if(map->items == NULL) {
		return MAP_MEMALLOCATION_ERROR;
	}

	map->capacity = map_capacity;
	map->occupied = 0;

	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code map_set_item(Map* map, const char* item_key, void* item_value, unsigned short item_value_size) {
	if(map == NULL || item_key == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	} else if(map->occupied >= MAX_MAP_SIZE) {
		return MAP_OVERFLOW_ERROR;
	}

	// Rehash map elements and enlarge map if necessary.
	if(is_map_to_small(map->occupied + 1, map->capacity)) {
		Map_Exec_Code map_resize_result = map_resize(map, MAP_KEY_GROWTH_SIZE);
		if(map_resize_result != MAP_EXECUTION_SUCCESS) {
			return map_resize_result;
		}
	}
		
	// Generate map item hash.
	Map_Item_Hash map_item_hash = 0;
	Map_Exec_Code map_generate_hash_result = map_generate_hash(&map_item_hash, map->capacity, item_key);
	if(map_generate_hash_result != MAP_EXECUTION_SUCCESS) {
		return map_generate_hash_result;
	}

	if(map->items[map_item_hash] != NULL) {
		// Check if there is an element with the same key.
		Map_Exec_Code map_find_item_hash_by_key_result = map_find_item_hash_by_key(&map_item_hash, map, item_key);
		if(map_find_item_hash_by_key_result != MAP_EXECUTION_SUCCESS && map_find_item_hash_by_key_result != MAP_ITEM_NOT_FOUND_ERROR) {
			return map_find_item_hash_by_key_result;
		}
		// Element with same key was found, remove element from memory, 
		// it will be created a element with same key but new value.
		if(map_find_item_hash_by_key_result == MAP_EXECUTION_SUCCESS) {
			Map_Exec_Code free_item_result = map_free_item(map, &map_item_hash, item_key);
			if(free_item_result != MAP_EXECUTION_SUCCESS) {
				return free_item_result;
			}
			map->occupied--;
		// Element with same key was not found but hashes are equal.
		// Find next free hash for new element.
		} else {
			Map_Exec_Code find_free_hash_result = map_find_free_hash(&map_item_hash, map);
			if(find_free_hash_result != MAP_EXECUTION_SUCCESS) {
				return find_free_hash_result;
			}
		}
	}

	unsigned char item_key_length = strlen(item_key) + 1;
	if(item_key_length > MAX_MAP_KEY_LENGTH) {
		return MAP_ILLEGAL_KEY_LENGTH_ERROR;
	}

	// Map element is stored together with its key and value to 
	// reduce amount of memory allocation [Map | Key | Value].
	unsigned char size_of_map = sizeof(Map_Item);
	Map_Item* map_item = (Map_Item*)malloc(size_of_map + item_key_length + item_value_size);
	if(map_item == NULL) {
		return MAP_MEMALLOCATION_ERROR;
	}

	// Calculating the start position in memory block for key and value.
	char* item_key_position = (char*)map_item + size_of_map;
	void* item_value_position = (void*)item_key_position + item_key_length;

	map_item->key = item_key_position;
	map_item->value = item_value_position;

	memmove(map_item->key, item_key, item_key_length);
	memmove(map_item->value, item_value, item_value_size);

	map->items[map_item_hash] = map_item;
	map->occupied++;
	
	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code map_resize(Map* map, unsigned char direction) {
	if(map == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	}

	Map_Item_Hash old_capacity = map->capacity;
	Map_Item** new_items = {0};

	if(direction == MAP_KEY_GROWTH_SIZE) {
		map->capacity = get_prime_from_capacity(map->capacity);
		new_items = (Map_Item**)calloc(map->capacity, sizeof(Map_Item*));
	} else if(direction == MAP_KEY_SHRINK_SIZE) {
		map->capacity = get_prime_from_capacity(map->capacity) / 2;
		new_items = (Map_Item**)realloc(map->items, map->capacity * sizeof(Map_Item*));
	} else {
		return MAP_FUNC_ILLEGAL_PARAM;
	}

	if(new_items == NULL) {
		return MAP_MEMALLOCATION_ERROR;
	}

	for(Map_Item_Hash index = 0; index < old_capacity; index++) {
		// Copy all existed elements
    if(map->items[index] != NULL) {
			Map_Item_Hash map_item_hash = 0;
			Map_Exec_Code generate_hash_result = map_generate_hash(&map_item_hash, map->capacity, map->items[index]->key);
			if(generate_hash_result != MAP_EXECUTION_SUCCESS) {
				return generate_hash_result;
			}
			// Check for collision in new items array.
			if(new_items[map_item_hash] != NULL) {
				// Recalculate new item hash when collision was found.
				Map_Exec_Code find_free_hash_result = map_find_free_hash(&map_item_hash, map);
				if(find_free_hash_result != MAP_EXECUTION_SUCCESS) {
					return find_free_hash_result;
				}
			}
			new_items[map_item_hash] = map->items[index];
		}
	}

	free(map->items);
  map->items = new_items;

	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code map_get_item(Map* map, Map_Item** map_item, const char* item_key) {
	if(map == NULL || map_item == NULL || item_key == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	}

	Map_Item_Hash map_item_hash = 0;

	Map_Exec_Code generate_hash_result = map_generate_hash(&map_item_hash, map->capacity, item_key);
	if(generate_hash_result != MAP_EXECUTION_SUCCESS) {
		*map_item = NULL;
		return generate_hash_result;
	}
	
	Map_Exec_Code find_item_by_key_result = map_find_item_hash_by_key(&map_item_hash, map, item_key);
	if(find_item_by_key_result != MAP_EXECUTION_SUCCESS) {
		*map_item = NULL;
		return find_item_by_key_result;
	}

	*map_item = map->items[map_item_hash];
	if(*map_item == NULL) {
		*map_item = NULL;
		return MAP_ITEM_NOT_FOUND_ERROR;
	}

	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code map_free(Map* map) {
	for(Map_Item_Hash hash = 0; hash < map->capacity; hash++) {
		if(map->items[hash] != NULL) {
			free(map->items[hash]);
			map->items[hash] = NULL;
		}	
	}

	free(map);
	return MAP_EXECUTION_SUCCESS;
}

Map_Exec_Code map_free_item(Map* map, Map_Item_Hash* item_hash, const char* item_key) {
	if(map == NULL && item_key == NULL) {
		return MAP_FUNC_ILLEGAL_PARAM;
	}

	if(item_hash != NULL && map->items[*item_hash] == NULL) {
		return MAP_ITEM_NOT_FOUND_ERROR;
	} else if(item_hash != NULL && map->items[*item_hash] != NULL) {
		free(map->items[*item_hash]);
		map->items[*item_hash] = NULL;
		return MAP_EXECUTION_SUCCESS;	
	}
	
	Map_Item_Hash map_item_hash = 0;

	Map_Exec_Code generate_hash_result = map_generate_hash(&map_item_hash, map->capacity, item_key);
	if(generate_hash_result != MAP_EXECUTION_SUCCESS) {
		return generate_hash_result;
	}
	
	Map_Exec_Code find_item_by_key_result = map_find_item_hash_by_key(&map_item_hash, map, item_key);
	if(find_item_by_key_result != MAP_EXECUTION_SUCCESS) {
		return find_item_by_key_result;
	}

	free(map->items[map_item_hash]);
	map->items[map_item_hash] = NULL;

	if(is_map_to_big(map->occupied, map->capacity)) {
		Map_Exec_Code shrink_map_result = map_resize(map, MAP_KEY_SHRINK_SIZE);
		if(shrink_map_result != MAP_EXECUTION_SUCCESS) {
			return shrink_map_result;
		}
	}

	return MAP_EXECUTION_SUCCESS;
}