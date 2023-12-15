#include "cpm_string.h"

void string_init(String_t* str)
{
    str->size = 0;
    str->capacity = 5;
    str->inner_str = malloc(str->capacity);
}
void string_init_cstr(String_t* str, char* cstr)
{
    size_t cstr_size = strlen(cstr);
    str->capacity = cstr_size * 2;
    str->inner_str = malloc(str->capacity);
    if(str->inner_str){
        strcpy(str->inner_str,cstr);
        str->size = cstr_size;
    }

}
 
bool string_concat(String_t* dest, String_t* src)
{
    if (dest->capacity < dest->size + src->size + 1)
    {
        while (dest->capacity < dest->size + 1) dest->capacity *= 2;
        char* new_str = realloc(dest->inner_str, dest->capacity + 1);
        if (!new_str) return false;
        dest->inner_str = new_str; 
    }
    
    strcpy(dest->inner_str + dest->size, src->inner_str);
    dest->size += src->size;
    return true;
}

bool string_concat_cstr(String_t* dest,  const char* src)
{
    size_t src_len = strlen(src);
    dest->size += src_len;
    if (dest->capacity < dest->size + 2)
    {
        while (dest->capacity < dest->size + 2) dest->capacity *= 2;
        char* newstr = realloc(dest->inner_str, dest->capacity);
        if (!newstr) return false;
        dest->inner_str = newstr;
    }
    
    dest->inner_str = strncat(dest->inner_str, src, dest->capacity - dest->size - 1);

    
    return true;
    
}


const char* string_get(String_t* str) // CONSUMES THE String_t
{
    
    return str->inner_str;
}  

String_t string_copy(String_t* str)
{
    String_t res;
    res.capacity = str->capacity;
    res.size = str->size;
    char* res_new_ptr = strdup(str->inner_str);
    if(!res_new_ptr){ res_new_ptr = malloc(sizeof(res.capacity));}
    res.inner_str = res_new_ptr;

    return res; 
}
size_t string_length(String_t* str)
{
    return str->size;
}

size_t string_capacity (String_t* str)
{
    return str->capacity;
}

void string_clear(String_t* str)
{
    for(int i=0; i < str->size; i++) str->inner_str[i] = '\0';
}
void string_free(String_t* str)
{
    free(str->inner_str);
    free(str);
}