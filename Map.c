#include "Map.h"

/*
############################################
   Hashing and Collision handle functions
###########################################
*/

Map_Return_Code map_generate_hash(Map_Hash* hash, const Map_UInt size, const Map_String key) {
	if(hash == NULL || key == NULL)
		return MAP_ERROR_INVALID_PTR;

	Map_Hash l_hash = 5381;
	Map_String c_key = key;

	while(*c_key) {
		l_hash = ((l_hash << 5) + l_hash) + (Map_Char)(*c_key);
		c_key++;
	}
	
	*hash = l_hash % size;
	
	return MAP_SUCCESS;
}

Map_Return_Code map_find_free_hash(Map_Hash* hash, const Map map) {
	if(hash == NULL)
		return MAP_ERROR_INVALID_PTR;
	else if(map.occupied == MAP_MAX_SIZE)
		return MAP_ERROR_OVERFLOW;

	Map_Hash l_hash = *hash;
	Map_UInt index = 0;

	while(map.items[l_hash] != NULL) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		map_reset_hash_if(&l_hash, map.size);
		map_use_hash_algorithm(&l_hash, &index, map);
	}

	*hash = l_hash;

	return MAP_SUCCESS;
}

Map_Return_Code map_find_hash_by_key(Map_Hash* hash, const Map map, const Map_String key) {
	if(hash == NULL || key == NULL)
		return MAP_ERROR_INVALID_PTR;
	
	Map_Hash l_hash = *hash;
	Map_UInt index = 1;
	Map_UInt probe_count = 0;
	Map_Return_Code exec_code = 0;

	// In worst case, if we have two different keys with two identical hashes,
	// this loop will go through the entire map.
	while(probe_count <= map.size) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		map_reset_hash_if(&l_hash, map.size);
		if(map.items[l_hash] != NULL && sb_string_compare(key, map.items[l_hash]->key) == SB_TRUE) {
			// Hash was found.
			*hash = l_hash;
			return MAP_SUCCESS;
		}
		// Hash was not found, calculate next hash.
		exec_code = map_use_hash_algorithm(&l_hash, &index, map);
		if(exec_code != MAP_SUCCESS)
			return exec_code;
		probe_count++;
	}

	// Hash was not found.
	return MAP_ERROR_ITEM_NOT_FOUND;
}

Map_Return_Code map_use_hash_algorithm(Map_Hash* hash, Map_Hash* index, const Map map) {
	if(hash == NULL || 
		// If map is bigger than MAP_SMALL_SIZE and index was not passed
		// you can not use Quadratic probe.
		(map.size > MAP_SMALL_SIZE && index == NULL))
		return MAP_ERROR_INVALID_PTR;

	if(map.size <= MAP_SMALL_SIZE) {
		// Linear probe.
		*hash += 1;
	}	else {
		//Quadratic probe.
		*hash = (*hash + (*index ^ 2)) % map.size;
		*index += 1;
	}	

	return MAP_SUCCESS;
}

/*
	############################################
		 Collision strategies helper functions
	###########################################
*/

float get_shrink_factor(const Map_UInt size) {
	if(size >= MAP_BIG_SIZE)
		return MAP_BIG_SHRINK_AT;
	else if(size >= MAP_MEDIUM_SIZE)
		return MAP_MEDIUM_SHRINK_AT;
	else if(size <= MAP_SMALL_SIZE)
		return MAP_SMALL_SHRINK_AT;
	return MAP_SMALL_SHRINK_AT;
}

float get_growth_factor(const Map_UInt size) {
	if(size >= MAP_BIG_SIZE)
		return MAP_BIG_GROWTH_AT;
	else if(size >= MAP_MEDIUM_SIZE)
		return MAP_MEDIUM_GROWTH_AT;
	else if(size <= MAP_SMALL_SIZE)
		return MAP_SMALL_GROWTH_AT;
	return MAP_SMALL_GROWTH_AT;
}

Map_UInt get_prime_from(const Map_UInt from) {
	return(from * 2) >= MAP_MAX_SIZE ? MAP_MAX_SIZE : from * 2;
}

Map_Bool is_map_to_small(const Map_UInt occupied, const Map_UInt size) {
	if(occupied == MAP_MAX_SIZE || occupied == 0)
		return 0;
	return((float)occupied / size) >= get_growth_factor(size);
}

Map_Bool is_map_to_big(const Map_UInt occupied, const Map_UInt size) {
	if(occupied == MAP_MAX_SIZE || occupied == 0)
		return 0;
	return((float)occupied / size) <= get_shrink_factor(size);
}

/*
	############################################
								API functions
	###########################################
*/

Map_Return_Code map_collect(Map_Collection* collection, const Map map) {
	Map_UInt collection_index = 0;

	collection->items = malloc(map.occupied * sizeof(Map_Item*));
	if(collection->items == NULL)
		return MAP_ERROR_MEMALLOC;

	for(Map_UInt index = 0; index < map.size; index++) {
		if(map.items[index] != NULL) {
			collection->items[collection_index] = map.items[index];
			collection_index++;
		}
	}

	collection->size = map.occupied;

	return MAP_SUCCESS;
}

Map_Return_Code map_delete_collection(Map_Collection* collection) {
	if(collection->items == NULL)
		return MAP_ERROR_INVALID_PTR;

	free(collection->items);
	
	return MAP_SUCCESS;
}

Map_Return_Code map_init(Map* map, const Map_UInt size) {
	if(map == NULL)
		return MAP_ERROR_INVALID_PTR;
	else if(size == 0)
		return MAP_ERROR_INVALID_SIZE;

	map->items = (Map_Item**)calloc(size, sizeof(Map_Item*));
	if(map->items == NULL)
		return MAP_ERROR_MEMALLOC;

	map->size = size;
	map->occupied = 0;

	return MAP_SUCCESS;
}

Map_Return_Code map_set_item(Map* map, const Map_String key, const Map_Any value, const Map_UShort value_size) {
	if(map == NULL || key == NULL)
		return MAP_ERROR_INVALID_PTR;
	else if(map->occupied >= MAP_MAX_SIZE)
		return MAP_ERROR_OVERFLOW;

	// Map element is stored together with its key and value to 
	// reduce amount of memory allocation and memory free execution [Map | Key | Value].
	Map_Item* item = NULL;
	Map_Hash item_hash = 0;
	Map_Return_Code exec_code = 0;
	Map_UChar key_length = strlen(key) + 1;
	if(key_length > MAP_MAX_KEY_LENGTH)
		return MAP_ERROR_INVALID_KEY_LENGTH;

	// Rehash map elements and enlarge map if necessary.
	if(is_map_to_small(map->occupied + 1, map->size)) {
		exec_code = map_resize(map, MAP_KEY_GROWTH_SIZE);
		if(exec_code != MAP_SUCCESS)
			return exec_code;
	} 

	exec_code = map_generate_hash(&item_hash, map->size, key);
	if(exec_code != MAP_SUCCESS)
		return exec_code;

	// Collision was found.
	if(map->items[item_hash] != NULL) {
		// Check if there is an element with the same key.
		exec_code = map_find_hash_by_key(&item_hash, *map, key);
		// Element was not found and executuin was not ended successfuly.
		if(exec_code != MAP_SUCCESS && exec_code != MAP_ERROR_ITEM_NOT_FOUND)
			return exec_code;

		if(exec_code == MAP_SUCCESS) {
			// Element with the same key was found, realloc memory block, 
			// and save new value.
			item = (Map_Item*)realloc(map->items[item_hash], sizeof(Map_Item) + key_length + value_size);
			if(item == NULL)
				return MAP_ERROR_MEMALLOC;
		} else {
			// Element with same key was not found but hashes are equal.
			// Find next free hash for new element.
			exec_code = map_find_free_hash(&item_hash, *map);
			if(exec_code != MAP_SUCCESS)
				return exec_code;
			item = (Map_Item*)malloc(sizeof(Map_Item) + key_length + value_size);
			if(item == NULL)
				return MAP_ERROR_MEMALLOC;
			map->occupied++;
		}
	} else {
		// No collision was found, alloc memory for new element.
		item = (Map_Item*)malloc(sizeof(Map_Item) + key_length + value_size);
		if(item == NULL)
			return MAP_ERROR_MEMALLOC;
		map->occupied++;
	}

	// Calculating the start position in memory block for key and value.
	item->key = (Map_String)item + sizeof(Map_Item);
	item->value = item->key + key_length;

	memcpy((Map_Any)item->key, key, key_length);
	memcpy(item->value, value, value_size);

	map->items[item_hash] = item;
	
	return MAP_SUCCESS;
}

Map_Return_Code map_resize(Map* map, const Map_UChar direction) {
	if(map == NULL)
		return MAP_ERROR_INVALID_PTR;

	Map_UInt old_capacity = map->size;
	Map_Hash hash = 0;
	Map_Return_Code exec_code = 0;
	Map_Item** new_items = {0};

	if(direction == MAP_KEY_GROWTH_SIZE)
		map->size = get_prime_from(map->size);
	else if(direction == MAP_KEY_SHRINK_SIZE)
		map->size = get_prime_from(map->occupied);
	else
		return MAP_ERROR_INVALID_RESIZE_DIRECTION;

	new_items = (Map_Item**)calloc(map->size, sizeof(Map_Item*));
	if(new_items == NULL)
		return MAP_ERROR_MEMALLOC;

	for(Map_UInt index = 0; index < old_capacity; index++) {
		// Copy all existed elements in map.
    if(map->items[index] != NULL) {
			exec_code = map_generate_hash(&hash, map->size, map->items[index]->key);
			if(exec_code != MAP_SUCCESS)
				return exec_code;
			// Check if elements with same key are exist in new map.
			// When exist, find free hash.
			if(new_items[hash] != NULL) {
				exec_code = map_find_free_hash(&hash, *map);
				if(exec_code != MAP_SUCCESS)
					return exec_code;
			}

			new_items[hash] = map->items[index];
		}
	}

	free(map->items);
  map->items = new_items;

	return MAP_SUCCESS;
}

Map_Return_Code map_get_item(Map_Item** item, const Map map, const Map_String key) {
	if(item == NULL || key == NULL)
		return MAP_ERROR_INVALID_PTR;

	Map_Hash hash = 0;
	Map_Return_Code exec_code = 0;

	// Get start position.
	exec_code = map_generate_hash(&hash, map.size, key);
	if(exec_code != MAP_SUCCESS) {
		*item = NULL;
		return exec_code;
	}
	
	// Find element with same key from start position.
	exec_code = map_find_hash_by_key(&hash, map, key);
	if(exec_code != MAP_SUCCESS) {
		*item = NULL;
		return exec_code;
	}

	*item = map.items[hash];

	return *item == NULL ? MAP_ERROR_ITEM_NOT_FOUND : MAP_SUCCESS;
}

Map_Return_Code map_delete(Map* map) {
	for(Map_UInt index = 0; index < map->size; index++) {
		if(map->items[index] != NULL) {
			free(map->items[index]);
			map->items[index] = NULL;
		}
	}

	return MAP_SUCCESS;
}

Map_Return_Code map_delete_item(Map* map, const Map_Hash* hash, const Map_String key) {
	if(map == NULL && (key == NULL && hash == NULL))
		return MAP_ERROR_INVALID_PTR;
	
	Map_Hash l_hash = hash == NULL ? 0 : *hash;
	Map_Return_Code exec_code = 0;

	if(hash != NULL && map->items[l_hash] == NULL) {
		return MAP_ERROR_ITEM_NOT_FOUND;
	} else if(hash != NULL && map->items[l_hash] != NULL) {
		// Delete map item by defined hash.
		free(map->items[l_hash]);
		map->items[l_hash] = NULL;
		map->occupied--;
	} else {	
		// Delete map item by key.
		exec_code = map_generate_hash(&l_hash, map->size, key);
		if(exec_code != MAP_SUCCESS)
			return exec_code;
		
		exec_code = map_find_hash_by_key(&l_hash, *map, key);
		if(exec_code != MAP_SUCCESS)
			return exec_code;

		free(map->items[l_hash]);
		map->items[l_hash] = NULL;
		map->occupied--;
	}

	if(is_map_to_big(map->occupied, map->size)) {
		exec_code = map_resize(map, MAP_KEY_SHRINK_SIZE);
		if(exec_code != MAP_SUCCESS)
			return exec_code;
	}

	return MAP_SUCCESS;
}

/*
	############################################
							Utility functions
	###########################################
*/

void map_reset_hash_if(Map_Hash* hash, const Map_UInt size) {
	if(*hash + 1 >= size)
		*hash = 0;
}