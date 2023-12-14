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

String_t string_new();
String_t string_from_cstr(const char* str);
String_t string_concat(String_t str);
const char* string_get(String_t str); // CONSUMES THE String_t 
String_t string_copy(String_t str);
size_t string_length(String_t str);
size_t string_capacity (String_t str);
bool string_clear(String_t str);