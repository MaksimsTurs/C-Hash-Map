# CMap Library

The `CMap` library provides a hash map implementation in C, offering functionalities like adding, retrieving, deleting key-value pairs, and handling hash collisions using open addressing. It supports dynamic resizing and memory management.

## Features

- **Hash Map Implementation:** Efficient key-value storage with collision handling and dynamic resizing.
- **Customizable Hashing:** Provides control over the hashing function and resizing mechanism.
- **Memory Management:** Handles memory allocation, reallocation, and cleanup for map elements.
- **Error Handling:** Uses error codes to manage invalid operations and memory issues.

## Functions

### `cmap_gen_hash(CMAP_Hash* const hash, CMAP_Hash const size, CMAP_Char_Ptr const key)`
Generates a hash value for the given key.

- **Parameters:**
  - `hash`: Pointer to store the resulting hash.
  - `size`: The size of the hash map.
  - `key`: The string key to hash.
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `cmap_find_hash(CMAP_Hash* const hash, CMAP_Item** const items, CMAP_ULLong const occupied, CMAP_ULLong const size)`
Finds the next available position in the hash map to resolve a hash collision.

- **Parameters:**
  - `hash`: The hash value to start from.
  - `items`: Pointer to the hash map's items array.
  - `occupied`: The number of occupied slots in the map.
  - `size`: The size of the map.
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `cmap_find_hash_key(CMAP_Hash* const hash, CMAP_Item** const items, CMAP_ULLong const occupied, CMAP_ULLong const size, CMAP_Char_Ptr const key)`
Finds the position of a specific key in the hash map.

- **Parameters:**
  - `hash`: The starting hash value.
  - `items`: Pointer to the hash map's items array.
  - `occupied`: The number of occupied slots.
  - `size`: The size of the hash map.
  - `key`: The key to search for.
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `cmap_init(CMAP_Map* const this, CMAP_ULLong const size)`
Initializes a new hash map with the specified size.

- **Parameters:**
  - `this`: A pointer to the `CMAP_Map` object to initialize.
  - `size`: The initial size of the hash map.
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `cmap_set(CMAP_Map* const this, CMAP_Char_Ptr const key, CMAP_Any const value, CMAP_UShort const value_size)`
Sets a key-value pair in the hash map. If the key already exists, the value is updated.

- **Parameters:**
  - `this`: A pointer to the `CMAP_Map` object.
  - `key`: The key to insert or update.
  - `value`: The value associated with the key.
  - `value_size`: The size of the value.
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `cmap_resize(CMAP_Map* const this, CMAP_UChar const direction)`
Resizes the hash map based on the given direction (growth or shrink).

- **Parameters:**
  - `this`: A pointer to the `CMAP_Map` object.
  - `direction`: The direction to resize (growth or shrink).
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `cmap_get(CMAP_Item** const item, CMAP_Map const map, CMAP_Char_Ptr const key)`
Retrieves a key-value pair from the hash map by key.

- **Parameters:**
  - `item`: A pointer to store the retrieved item.
  - `map`: The hash map to retrieve from.
  - `key`: The key to search for.
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `map_delete(CMAP_Map* const this)`
Deletes all items in the hash map and frees the memory used by the map.

- **Parameters:**
  - `this`: A pointer to the `CMAP_Map` object.
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

### `cmap_delete_item(CMAP_Map* const this, CMAP_Hash* const hash, CMAP_Char_Ptr const key)`
Deletes a specific key-value pair from the hash map.

- **Parameters:**
  - `this`: A pointer to the `CMAP_Map` object.
  - `hash`: A pointer to the hash value, or `NULL` to use the key.
  - `key`: The key of the item to delete (if `hash` is `NULL`).
  
- **Returns:** `CMAP_Ret_Code` (success or error code)

## Example Usage

```c
#include "CMap.h"

int main() {
    // Initialize the map
    CMAP_Map my_map;
    cmap_init(&my_map, 10);

    // Set a key-value pair
    cmap_set(&my_map, "key1", "value1", 6);

    // Get a value by key
    CMAP_Item* item;
    cmap_get(&item, my_map, "key1");
    if (item != NULL) {
        printf("Key: %s, Value: %s\n", item->key, item->value);
    }

    // Delete an item
    cmap_delete_item(&my_map, NULL, "key1");

    // Delete the entire map
    map_delete(&my_map);

    return 0;
}
