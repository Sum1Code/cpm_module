#include "cpm_string.h"

int main(void)
{
    String_t str;
    string_init_cstr(&str, "Hello, world!");
    String_t cpy;
    string_copy(&cpy, &str);
    string_concat(&str, &cpy);
    printf("%s\n", string_get(&str));

    string_free(&str);
    string_free(&cpy);
    return 0;
}