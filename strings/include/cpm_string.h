#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct StringType{
  char* inner_str;
  size_t capacity;
  size_t size;
} String_t;

void string_init(String_t* str);
void string_init_cstr(String_t* str, char* cstr);
bool string_concat(String_t* dest, String_t* src);
bool string_concat_cstr(String_t* dest,  const char* src);
const char* string_get(String_t* str); // CONSUMES THE String_t passed to it! 
void string_copy(String_t* dest, String_t* src);
size_t string_length(String_t* str);
size_t string_capacity (String_t* str);
void string_clear(String_t* str); // Will clear the underlying string but won't delete the memory its in.
void string_free(String_t* str); // deletes the underlying memory the inner string resides in