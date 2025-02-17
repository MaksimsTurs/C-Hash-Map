#include "SBString.h"

SB_Return_Code sb_string_create(SB_String* self, const SB_String_Ptr string) {
	if(self == NULL || string == NULL)
		return SB_ERROR_INVALID_PTR;

	SB_ULLong length = 0;

	length = sb_string_length(string);
	self->value = (SB_String_Ptr)malloc(length + 1);
	if(self->value == NULL)
		return SB_ERROR_MEMALLOC;
	
	memcpy(self->value, string, length);
	self->value[length] = '\0';
	self->length = length;

	return SB_SUCCESS;
}

SB_Return_Code sb_string_free(SB_String* self) {
	if(self == NULL || self->value == NULL)
		return SB_ERROR_INVALID_PTR;

	free(self->value);
	self->value = NULL;
	self->length = 0;

	return SB_SUCCESS;
}

SB_Return_Code sb_string_split(SB_Split* split, const SB_String_Ptr to_split, const SB_Char spliter) {
	if(split == NULL || to_split == NULL)
		return SB_ERROR_INVALID_PTR;

	SB_Char ch = ' ';
	SB_String_Ptr item = NULL;
	SB_String_Ptr* items = NULL;
	SB_String_Ptr c_to_split = NULL;
	SB_UShort splited_length = 0;
	
	split->items = (SB_String_Ptr*)malloc(1);
	c_to_split = to_split;

	while(SB_TRUE) {
		ch = *c_to_split;
		if((ch == spliter || ch == SB_NULL_TERMINATOR) && splited_length > 0) {
			// Skip spliter.
			c_to_split++;

			// Realloc memory for one new element.
			items = (SB_String_Ptr*)realloc(split->items, (sizeof(SB_String_Ptr) * split->size) + 1);
			if(items == NULL)
				return SB_ERROR_MEMALLOC;
			split->items = items;

			// Alloc memory for new array item.
			item = (SB_String_Ptr)malloc(splited_length);
			if(item == NULL)
				return SB_ERROR_MEMALLOC;

			memcpy(item, (c_to_split - splited_length) - 1, splited_length);
			item[splited_length] = '\0';
			
			splited_length = 0;

			split->items[split->size++] = item;
		}

		if(ch == SB_NULL_TERMINATOR)
			break;

		splited_length++;
		c_to_split++;
	}

	return SB_SUCCESS;
}

SB_Return_Code sb_string_split_free(SB_Split* self) {
	if(self == NULL || self->items == NULL)
		return SB_ERROR_INVALID_PTR;

	for(SB_UShort index = 0; index < self->size; index++) {
		if(self->items[index] != NULL) {
			free(self->items[index]);
			self->items[index] = NULL;
		}
	}

	free(self->items);
	self->items = NULL;

	return SB_SUCCESS;
}

SB_Return_Code sb_string_append(SB_String* self, const SB_String_Ptr append_string, const SB_UChar count) {
	if(self == NULL || self->value == NULL || append_string == NULL)
		return SB_ERROR_INVALID_PTR;
	else if(count == 0)
		return SB_ERROR_INVALID_COUNT;

	SB_ULLong append_length = 0;
	SB_ULLong string_new_length = 0;
	SB_UShort index = 0;
	SB_String_Ptr new_value = NULL;
	SB_String_Ptr dest = NULL;

	append_length = sb_string_length(append_string);
	string_new_length = self->length + (append_length * count);

	new_value = (SB_String_Ptr)realloc(self->value, string_new_length + 1);
	if(new_value == NULL)
		return SB_ERROR_MEMALLOC;
	self->value = new_value;
	
	// Set position to end of the old string.
	dest = new_value + self->length;
	for(index = 0; index < count; index++)
		memcpy(dest + (append_length * index), append_string, append_length);

	self->length = string_new_length;
	self->value[self->length] = '\0';

	return SB_SUCCESS;
}

SB_Bool sb_string_compare(const SB_String_Ptr str_one, const SB_String_Ptr str_two) {
	if(str_one == NULL || str_two == NULL)
		return SB_FALSE;

	const SB_UChar *str_ptr_one = (const SB_UChar*)str_one;
	const SB_UChar *str_ptr_two = (const SB_UChar*)str_two;

	SB_UChar ch_one = 0;
	SB_UChar ch_two = 0;
	SB_Bool is_equal = SB_TRUE;

	do {
		ch_one = (SB_UChar)*str_ptr_one++;
		ch_two = (SB_UChar)*str_ptr_two++;

		if(ch_one != ch_two) {
			is_equal = SB_FALSE;
			break;
		}
	} while(*str_ptr_one != SB_NULL_TERMINATOR || *str_ptr_two != SB_NULL_TERMINATOR);

	return is_equal;
}

SB_ULLong sb_string_length(const SB_String_Ptr self) {
	if(self == NULL)
		return 0;

	SB_String_Ptr string_cp = self;

	while(*string_cp != SB_NULL_TERMINATOR)
		string_cp++;
	
	return(string_cp - self);
}

SB_LLong sb_string_index_of(SB_String* self, const SB_Char ch, const SB_ULLong start_position) {
	if(self == NULL || self->value == NULL)
		return SB_ERROR_INVALID_PTR;

	SB_String_Ptr self_cp = self->value + start_position;
	SB_LLong ch_position = -1;

	while(SB_TRUE) {
		if(*self_cp == SB_NULL_TERMINATOR && ch != SB_NULL_TERMINATOR) {
			break;
		}	else if(*self_cp == ch) {
			ch_position = self_cp - self->value;
			break;
		}
		self_cp++;
	}

	return ch_position;
}

SB_Return_Code sb_string_slice(SB_String* self, const SB_ULLong start, const SB_ULLong end) {
	if(start > self->length || end > self->length || start > end)
		return SB_ERROR_INVALID_POSITION;
	else if(self == NULL || self->value == NULL)
		return SB_ERROR_INVALID_PTR;

	SB_ULLong new_string_length = (end - start);
	SB_String_Ptr new_str = NULL;

	self->length = new_string_length;
	new_str = (SB_String_Ptr)malloc(new_string_length + 1);
	if(new_str == NULL)
		return SB_ERROR_MEMALLOC;

	memcpy(new_str, self->value + start, new_string_length);
	free(self->value);
	
	self->value = new_str;
	self->value[self->length] = '\0';

	return SB_SUCCESS;
}