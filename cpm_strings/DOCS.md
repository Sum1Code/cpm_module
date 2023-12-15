# cpm_strings docs

### provided structs:
```c
typedef struct StringType {
  char* inner_str;
  size_t capacity;
  size_t size;
} String_t;
```

### provided functions:
```c
void string_init(String_t* str);
void string_init_cstr(String_t* str, char* cstr);
bool string_concat(String_t* dest, String_t* src);
bool string_concat_cstr(String_t* dest,  const char* src);
const char* string_get(String_t* str);
void string_copy(String_t* dest, String_t* src);
size_t string_length(String_t* str);
size_t string_capacity (String_t* str);
void string_clear(String_t* str); 
extern void string_free(String_t* str); 
```
### NOTES:
---
- `extern void string_free(String_t* str)` 
<br> will only frees the inner_string of the String_t struct, it will not free the passed str pointer **It's the caller's responsibility to free the struct if its allocated in the heap**
<br> <br>
- `void string_clear(String_t* str)`
<br> will only clear the inner_string and set the size to 0, capacity will stay the same. this function does not free anything