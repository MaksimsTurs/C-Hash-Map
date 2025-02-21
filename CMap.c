#include "CMap.h"

/*
############################################
   Hashing and Collision handle functions
###########################################
*/

CMAP_Ret_Code cmap_gen_hash(CMAP_Hash* const hash, CMAP_Hash const size, CMAP_Char_Ptr const key) {
	CMAP_SAFE_CHECK(hash == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);

	CMAP_Hash l_hash = 14695981039346656037UL;
	CMAP_Char_Ptr c_key = key;
	CMAP_Char ch = ' ';

	while(ch != CMAP_NULL_TERMINATOR) {
		ch = *c_key;
		l_hash ^= ch;
		l_hash *= 1099511628211UL;
		c_key++;
	}

	*hash = l_hash % size;
	
	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_find_hash(CMAP_Hash* const hash, CMAP_Item** const items, CMAP_ULLong const occupied, CMAP_ULLong const size) {
	CMAP_SAFE_CHECK(hash == NULL || items == NULL, CMAP_ERROR_INVALID_PTR);
	CMAP_SAFE_CHECK(occupied == size, CMAP_ERROR_OVERFLOW);
	CMAP_SAFE_CHECK(occupied == 0 || size == 0, CMAP_ERROR_INVALID_SIZE);

	CMAP_Hash l_hash = *hash;
	CMAP_ULLong index = 0;

	while(items[l_hash] != NULL) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		CMAP_RESET_IF_BIGGER_THAN_SIZE(l_hash);
		// If map is bigger than MAP_SMALL_SIZE and index was not passed
		// you can not use Quadratic probe.
		CMAP_USE_HASHING_ALGORITH(l_hash, index, size);
	}

	*hash = l_hash;

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_find_hash_key(CMAP_Hash* const hash, CMAP_Item** const items, CMAP_ULLong const occupied, CMAP_ULLong const size, CMAP_Char_Ptr const key) {
	CMAP_SAFE_CHECK(hash == NULL || key == NULL || items == NULL, CMAP_ERROR_INVALID_PTR);
	CMAP_SAFE_CHECK(occupied == size, CMAP_ERROR_OVERFLOW);
	CMAP_SAFE_CHECK(occupied == 0 || size == 0, CMAP_ERROR_INVALID_SIZE);
	
	CMAP_Hash l_hash = *hash;
	CMAP_ULLong index = 1;
	CMAP_ULLong probe_count = 0;
	CMAP_Ret_Code exec_code = 0;

	// In worst case, if we have two different keys with two identical hashes,
	// this loop will go through the entire 
	while(probe_count <= size) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		CMAP_RESET_IF_BIGGER_THAN_SIZE(l_hash);
		if(items[l_hash] != NULL && cstr_comp(key, items[l_hash]->key) == CSTR_TRUE) {
			// Hash was found.
			*hash = l_hash;
			return CMAP_ITEM_FOUND;
		}
		// Hash was not found, calculate next hash.
		// If map is bigger than MAP_SMALL_SIZE and index was not passed
		CMAP_USE_HASHING_ALGORITH(l_hash, index, size);
		probe_count++;
	}

	// Hash was not found.
	return CMAP_ERROR_ITEM_NOT_FOUND;
}

/*
	############################################
								API functions
	###########################################
*/

CMAP_Ret_Code cmap_init(CMAP_Map* const this, CMAP_ULLong const size) {
	CMAP_SAFE_CHECK(this == NULL, CMAP_ERROR_INVALID_PTR);
	CMAP_SAFE_CHECK(size == 0, CMAP_ERROR_INVALID_SIZE);

	this->items = (CMAP_Item**)calloc(size, sizeof(CMAP_Item*));
	CMAP_SAFE_CHECK(this->items == NULL, CMAP_ERROR_MEMALLOC);

	this->size = size;
	this->occupied = 0;

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_set(CMAP_Map* const this, CMAP_Char_Ptr const key, CMAP_Any const value, CMAP_UShort const value_size) {
	CMAP_SAFE_CHECK(this == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);
	CMAP_SAFE_CHECK(this->occupied >= CMAP_MAX_SIZE, CMAP_ERROR_OVERFLOW);
	CMAP_SAFE_CHECK(value != NULL && value_size == 0, CMAP_ERROR_INVALID_SIZE);

	// Map element is stored together with its key and value to 
	// reduce amount of memory allocation and memory free execution [Map | Key | Value].
	CMAP_Item* item = NULL;
	CMAP_Hash hash = 0;
	CMAP_Ret_Code exec_code = 0;
	CMAP_UChar key_length = cstr_len(key) + 1;
	CMAP_SAFE_CHECK(key_length > CMAP_MAX_KEY_LENGTH, CMAP_ERROR_INVALID_KEY_LENGTH);
		
	// Rehash map elements and enlarge map if necessary.
	if(is_map_to_small(this->occupied + 1, this->size))
		CMAP_SAFE_CALL(cmap_resize(this, CMAP_KEY_GROWTH_SIZE))
		
	CMAP_SAFE_CALL(cmap_gen_hash(&hash, this->size, key));
	
	// Collision was found.
	if(this->items[hash] != NULL) {
		// Check if there is an element with the same key.
		exec_code = cmap_find_hash_key(&hash, this->items, this->occupied, this->size, key);
		// Element was not found and executuin was not ended successfuly.
		if(exec_code == CMAP_ITEM_FOUND) {
			// Element with the same key was found, realloc memory block, 
			// and save new value.
			item = (CMAP_Item*)realloc(this->items[hash], sizeof(CMAP_Item) + key_length + value_size);
			CMAP_SAFE_CHECK(item == NULL, CMAP_ERROR_MEMALLOC);
		} else {
			// Element with same key was not found but hashes are equal.
			// Find next free hash for new element.
			CMAP_SAFE_CALL(cmap_find_hash(&hash, this->items, this->occupied, this->size));	

			item = (CMAP_Item*)malloc(sizeof(CMAP_Item) + key_length + value_size);
			CMAP_SAFE_CHECK(item == NULL, CMAP_ERROR_MEMALLOC);
			this->occupied++;
		}
	} else {
		// No collision was found, alloc memory for new element.
		item = (CMAP_Item*)malloc(sizeof(CMAP_Item) + key_length + value_size);
		CMAP_SAFE_CHECK(item == NULL, CMAP_ERROR_MEMALLOC);
		this->occupied++;
	}

	// Calculating the start position in memory block for key and value.
	item->key = (CMAP_Char_Ptr)item + sizeof(CMAP_Item);
	item->value = item->key + key_length;

	cstr_cpy(item->key, key, key_length);
	memcpy(item->value, value, value_size);

	this->items[hash] = item;
	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_resize(CMAP_Map* const this, CMAP_UChar const direction) {
	CMAP_SAFE_CHECK(this == NULL, CMAP_ERROR_INVALID_PTR);

	CMAP_ULLong old_size = this->size;
	CMAP_Hash hash = 0;
	CMAP_Ret_Code exec_code = 0;
	CMAP_Item** new_items = {0};

	if(direction == CMAP_KEY_GROWTH_SIZE) {
		this->size = CMAP_GET_PRIME_FROM(this->size);
	}	else if(direction == CMAP_KEY_SHRINK_SIZE) {
		this->size = CMAP_GET_PRIME_FROM(this->occupied);
	}	else
		return CMAP_ERROR_INVALID_RESIZE_DIRECTION;

	new_items = (CMAP_Item**)calloc(this->size, sizeof(CMAP_Item*));
	CMAP_SAFE_CHECK(new_items == NULL, CMAP_ERROR_MEMALLOC);

	for(CMAP_ULLong index = 0; index < old_size; index++) {
		// Copy all existed elements in map.
    if(this->items[index] != NULL) {
			CMAP_SAFE_CALL(cmap_gen_hash(&hash, this->size, this->items[index]->key));
			// Check if elements with same key are exist in new map.
			// When exist, find free hash.
			if(new_items[hash] != NULL)
				CMAP_SAFE_CALL(cmap_find_hash(&hash, new_items, this->occupied, this->size));
			new_items[hash] = this->items[index];
		}
	}

	free(this->items);
  this->items = new_items;

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_get(CMAP_Item** const item, CMAP_Map const map, CMAP_Char_Ptr const key) {
	CMAP_SAFE_CHECK(item == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);

	CMAP_Hash hash = 0;
	CMAP_Ret_Code exec_code = 0;

	// Get start position.
	exec_code = cmap_gen_hash(&hash, map.size, key);
	if(exec_code != CMAP_SUCCESS) {
		*item = NULL;
		return exec_code;
	}
	
	// Find element with same key from start position.
	exec_code = cmap_find_hash_key(&hash, map.items, map.occupied, map.size, key);
	if(exec_code != CMAP_ITEM_FOUND) {
		*item = NULL;
		return exec_code;
	}

	*item = map.items[hash];

	return *item == NULL ? CMAP_ERROR_ITEM_NOT_FOUND : CMAP_SUCCESS;
}

CMAP_Ret_Code map_delete(CMAP_Map* const this) {
	for(CMAP_ULLong index = 0; index < this->size; index++) {
		if(this->items[index] != NULL) {
			free(this->items[index]);
			this->items[index] = NULL;
		}
	}

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_delete_item(CMAP_Map* const this, CMAP_Hash* const hash, CMAP_Char_Ptr const key) {
	CMAP_SAFE_CHECK(this == NULL && (key == NULL && hash == NULL), CMAP_ERROR_INVALID_PTR);
	
	CMAP_Hash l_hash = hash == NULL ? 0 : *hash;
	CMAP_Ret_Code exec_code = 0;

	if(hash != NULL && this->items[l_hash] == NULL) {
		return CMAP_ERROR_ITEM_NOT_FOUND;
	} else if(hash != NULL && this->items[l_hash] != NULL) {
		// Delete map item by defined hash.
		free(this->items[l_hash]);
		this->items[l_hash] = NULL;
		this->occupied--;
	} else {	
		// Delete map item by key.
		CMAP_SAFE_CALL(cmap_gen_hash(&l_hash, this->size, key));		
		if(cmap_find_hash_key(&l_hash, this->items, this->occupied, this->size, key) == CMAP_ERROR_ITEM_NOT_FOUND)
			return CMAP_ERROR_ITEM_NOT_FOUND;

		free(this->items[l_hash]);
		this->items[l_hash] = NULL;
		this->occupied--;
	}

	if(is_map_to_big(this->occupied, this->size))
		CMAP_SAFE_CALL(cmap_resize(this, CMAP_KEY_SHRINK_SIZE));

	return CMAP_SUCCESS;
}