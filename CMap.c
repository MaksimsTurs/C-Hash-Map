#include "CMap.h"

/*
############################################
   Hashing and Collision handle functions
###########################################
*/

CMAP_Ret_Code cmap_gen_hash(CMAP_Hash* const hash, const CMAP_Char* key, CMAP_Hash size) {
	// hash and key can not be null.
	CMAP_FAIL_IF(hash == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);
	// When size is zero, it mean that map is not initialized.
	CMAP_FAIL_IF(size == 0                  , CMAP_ERROR_INVALID_SIZE);

	CMAP_Hash l_hash = 14695981039346656037UL;
	CMAP_Char ch;

	while(ch != CMAP_NULL_TERMINATOR) {
		ch = *key;

		l_hash ^= ch;
		l_hash *= 1099511628211UL;
		
		key++;
	}

	*hash = l_hash % size;
	
	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_find_hash(CMAP_Hash* const hash, const CMAP_Item** items, CMAP_ULLong occupied, CMAP_ULLong size) {
	// hash and items can not be null.
	CMAP_FAIL_IF(hash == NULL || items == NULL, CMAP_ERROR_INVALID_PTR);
	// If occupied is equal to size it mean that map have no more capacity.
	CMAP_FAIL_IF(occupied == size             , CMAP_ERROR_OVERFLOW);
	// If size is equal to zero it mean that map was not initialized.
	CMAP_FAIL_IF(size == 0                    , CMAP_ERROR_INVALID_SIZE);

	CMAP_Hash l_hash = *hash;
	CMAP_ULLong index = 0;

	while(items[l_hash] != NULL) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		CMAP_RESET_HASH_IF_BIGGER_THAN_SIZE(l_hash, size);
		// If map is bigger than MAP_SMALL_SIZE and index was not passed
		// you can not use Quadratic probe.
		CMAP_USE_HASH_ALGORITHM(l_hash, index, size);
	}

	*hash = l_hash;

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_find_hash_key(CMAP_Hash* const hash, const CMAP_Char* key, const CMAP_Item** items, CMAP_ULLong occupied, CMAP_ULLong size) {
	// hash, key and items can not be null.
	CMAP_FAIL_IF(hash == NULL || key == NULL || items == NULL, CMAP_ERROR_INVALID_PTR);
	// If occupied is equal to size it mean that map have no more capacity.
	CMAP_FAIL_IF(occupied == size                            , CMAP_ERROR_OVERFLOW);
	// If size is equal to zero it mean that map was not initialized.
	CMAP_FAIL_IF(size == 0                                   , CMAP_ERROR_INVALID_SIZE);
	
	CMAP_Hash l_hash = *hash;
	CMAP_ULLong index = 1;
	CMAP_ULLong probe_count = 0;
	CMAP_Ret_Code exec_code = 0;

	// In worst case, if we have two different keys with two identical hashes,
	// this loop will go through the entire 
	while(probe_count <= size) {
		// Because iteration can start from any location, 
		// set hash to 0 to iterate from the start again.
		CMAP_RESET_HASH_IF_BIGGER_THAN_SIZE(l_hash, size);
		if(items[l_hash] != NULL && cstr_comp(key, items[l_hash]->key) == CSTR_TRUE) {
			// Hash was found.
			*hash = l_hash;
			return CMAP_ITEM_FOUND;
		}
		// Hash was not found, calculate next hash.
		// If map is bigger than MAP_SMALL_SIZE and index was not passed
		CMAP_USE_HASH_ALGORITHM(l_hash, index, size);
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

CMAP_Ret_Code cmap_init(CMAP_Map* const this, CMAP_ULLong size) {
	// this can not be null.
	CMAP_FAIL_IF(this == NULL, CMAP_ERROR_INVALID_PTR);
	// Inital size can not be zero.
	CMAP_FAIL_IF(size == 0,    CMAP_ERROR_INVALID_SIZE);

	this->items = (CMAP_Item**)calloc(size, sizeof(CMAP_Item*));
	CMAP_FAIL_IF(this->items == NULL, CMAP_ERROR_MEMALLOC);

	this->size = size;
	this->occupied = 0;

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_set(CMAP_Map* const this, const CMAP_Char* key, const CMAP_Any value, CMAP_UShort value_size) {
	// this and key can not be null.
	CMAP_FAIL_IF(this == NULL || key == NULL     , CMAP_ERROR_INVALID_PTR);
	// size can not be as max map size.
	CMAP_FAIL_IF(this->size >= CMAP_MAX_SIZE     , CMAP_ERROR_OVERFLOW);
	// Value size can not be null but value size is zero.
	CMAP_FAIL_IF(value != NULL && value_size == 0, CMAP_ERROR_INVALID_SIZE);
	// Map element is stored together with its key and value to 
	// reduce amount of memory allocation and memory free execution [Map | Key | Value].
	CMAP_Item* item = NULL;
	CMAP_Hash hash = 0;
	CMAP_Ret_Code exec_code = 0;
	CMAP_UChar key_length = cstr_len(key) + 1;
	CMAP_FAIL_IF(key_length > CMAP_MAX_KEY_LENGTH, CMAP_ERROR_INVALID_KEY_LENGTH);
	// Rehash map elements and enlarge map if necessary.
	if(CMAP_IS_MAP_TO_SMALL(this->occupied + 1, this->size))
		CMAP_SAFE_CALL(cmap_resize(this, CMAP_KEY_GROWTH_SIZE))
		
	CMAP_SAFE_CALL(cmap_gen_hash(&hash, key, this->size));
	// Collision was found.
	if(this->items[hash] != NULL) {
		// Check if there is an element with the same key.
		exec_code = cmap_find_hash_key(&hash, key, (const CMAP_Item**)this->items, this->occupied, this->size);
		// Element was not found and executuin was not ended successfuly.
		if(exec_code == CMAP_ITEM_FOUND) {
			// Element with the same key was found, realloc memory block, 
			// and save new value.
			item = (CMAP_Item*)realloc(this->items[hash], sizeof(CMAP_Item) + key_length + value_size);
			CMAP_FAIL_IF(item == NULL, CMAP_ERROR_MEMALLOC);
		} else {
			// Element with same key was not found but hashes are equal.
			// Find next free hash for new element.
			CMAP_SAFE_CALL(cmap_find_hash(&hash, (const CMAP_Item**)this->items, this->occupied, this->size));	

			item = (CMAP_Item*)malloc(sizeof(CMAP_Item) + key_length + value_size);
			CMAP_FAIL_IF(item == NULL, CMAP_ERROR_MEMALLOC);
			this->occupied++;
		}
	} else {
		// No collision was found, alloc memory for new element.
		item = (CMAP_Item*)malloc(sizeof(CMAP_Item) + key_length + value_size);
		CMAP_FAIL_IF(item == NULL, CMAP_ERROR_MEMALLOC);
		this->occupied++;
	}
	// Calculating the start position in memory block for key and value.
	item->key = (CMAP_Char*)item + sizeof(CMAP_Item);
	item->value = item->key + key_length;

	cstr_cpy(item->key, key, key_length);
	cstr_cpy(item->value, value, value_size);

	this->items[hash] = item;
	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_resize(CMAP_Map* const this, CMAP_UChar direction) {
	// this can not be null.
	CMAP_FAIL_IF(this == NULL, CMAP_ERROR_INVALID_PTR);

	CMAP_ULLong old_size = this->size;
	CMAP_ULLong index = 0;
	CMAP_Hash hash = 0;
	CMAP_Ret_Code exec_code = 0;
	CMAP_Item** new_items = {0};

	if(direction == CMAP_KEY_GROWTH_SIZE)
		this->size = CMAP_GET_PRIME_FROM(this->size);
	else if(direction == CMAP_KEY_SHRINK_SIZE)
		this->size = CMAP_GET_PRIME_FROM(this->occupied);
	else
		return CMAP_ERROR_INVALID_RESIZE_DIRECTION;

	new_items = (CMAP_Item**)calloc(this->size, sizeof(CMAP_Item*));
	CMAP_FAIL_IF(new_items == NULL, CMAP_ERROR_MEMALLOC);

	for(index = 0; index < old_size; index++) {
		// Copy all existed elements in map.
    if(this->items[index] != NULL) {
			CMAP_SAFE_CALL(cmap_gen_hash(&hash, this->items[index]->key, this->size));
			// Check if elements with same key are exist in new map.
			// When exist, find free hash.
			if(new_items[hash] != NULL)
				CMAP_SAFE_CALL(cmap_find_hash(&hash, (const CMAP_Item**)new_items, this->occupied, this->size));
			new_items[hash] = this->items[index];
		}
	}

	free(this->items);
  this->items = new_items;

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_get(const CMAP_Item** item, const CMAP_Char* key, CMAP_Map map) {
	// item and key can not be null.
	CMAP_FAIL_IF(item == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);

	CMAP_Hash hash = 0;
	CMAP_Ret_Code exec_code = 0;
	// Get start position.
	exec_code = cmap_gen_hash(&hash, key, map.size);
	if(exec_code != CMAP_SUCCESS) {
		*item = NULL;
		return exec_code;
	}
	// Find element with same key from start position.
	exec_code = cmap_find_hash_key(&hash, key, (const CMAP_Item**)map.items, map.occupied, map.size);
	if(exec_code != CMAP_ITEM_FOUND) {
		*item = NULL;
		return exec_code;
	}

	*item = map.items[hash];

	return *item == NULL ? CMAP_ERROR_ITEM_NOT_FOUND : CMAP_ITEM_FOUND;
}

CMAP_Ret_Code map_delete(CMAP_Map* this) {
	for(CMAP_ULLong index = 0; index < this->size; index++) {
		if(this->items[index] != NULL) {
			free(this->items[index]);
			this->items[index] = NULL;
		}
	}

	return CMAP_SUCCESS;
}

CMAP_Ret_Code cmap_delete_item(const CMAP_Hash* hash, const CMAP_Char* key, CMAP_Map* const this) {
	// this can not be null, user can delete map item with providing a hash, when hash
	// was not provided user must provide key.
	CMAP_FAIL_IF(this == NULL && (key == NULL || hash == NULL), CMAP_ERROR_INVALID_PTR);

	CMAP_Hash l_hash = hash == NULL ? 0 : *hash;
	CMAP_Ret_Code exec_code = 0;
	CMAP_FAIL_IF(hash != NULL && this->items[l_hash] == NULL,   CMAP_ERROR_ITEM_NOT_FOUND);	
	
	if(hash == NULL) {
		// Delete map item by key.
		CMAP_SAFE_CALL(cmap_gen_hash(&l_hash, key, this->size));		
		if(cmap_find_hash_key(&l_hash, key, (const CMAP_Item**)this->items, this->occupied, this->size) == CMAP_ERROR_ITEM_NOT_FOUND)
			return CMAP_ERROR_ITEM_NOT_FOUND;		
	}
	
	free(this->items[l_hash]);
	this->items[l_hash] = NULL;
	this->occupied--;

	if(CMAP_IS_MAP_TO_BIG(this->occupied, this->size))
		CMAP_SAFE_CALL(cmap_resize(this, CMAP_KEY_SHRINK_SIZE));

	return CMAP_SUCCESS;
}