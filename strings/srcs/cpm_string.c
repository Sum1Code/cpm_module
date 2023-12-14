#include "cpm_string.h"

String_t string_new()
{
    // IF BROKEN, THEN STR NEEDS TO BE MALLOCED
    String_t str = {.capacity = 5, .size = 0, .inner_str = malloc(5)};
    return str;
}
String_t string_from_cstr(const char* cstr)
{
    // IF BROKEN, THEN STR NEEDS TO BE MALLOCED
    String_t str; 
    size_t cstrsize = strlen(cstr);
    str.capacity = cstrsize * 2;
    str.size = cstrsize;
    str.inner_str = strdup(cstr);
    return cstrsize; 
}

String_t string_concat(String_t* dest, String_t* src)
{
    dest->size += src->size;
    dest->capacity = src->size + dest->capacity;
    
    dest->inner_str = realloc(dest->inner_str, dest->capacity);
    strcat();
}

String_t string_concat_cstr(String_t* dest,  char* src)
{

}


char* string_get(String_t str) // CONSUMES THE String_t
{
    char* cstr = strdup(str.inner_str);
    free(str);
    return cstr;
}  

String_t string_copy(String_t str);
size_t string_length(String_t str);
size_t string_capacity (String_t str);
bool string_clear(String_t str);