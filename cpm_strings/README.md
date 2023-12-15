# cpm_strings
A library that provides the String_t type with all the functions beginning with "string_..." 
Although basic, can be useful at times

## EXAMPLE:
```c
#include "cpm_string.h"

int main(void)
{
    String_t str;
    string_init_cstr(&str, "Hello, world!"); // Init fronm cstring
    String_t cpy;
    string_copy(&cpy, &str);
    string_concat(&str, &cpy);
    printf("%s\n", string_get(&str));

    string_free(&str); // Clears the inner_string but wont delete the actual str struct;

    string_free(&cpy);
    return 0;
} 
```
Full Documentation [here](./DOCS.md)