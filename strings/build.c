#include "CPM.h"

#define NO_CLI 

int main(void){
    BuildProperties_t buildprop;
    cpm_init(&buildprop, "clang");
    cpm_flags(&buildprop, "-I./include -g -fsanitize=leak");
    cpm_srcs(&buildprop, "./srcs/cpm_string.c");
    cpm_target(&buildprop, OBJECT, "libstrings.a", ".");
    cpm_compile(&buildprop);

    cpm_quick_compile("clang", "./test.c", ".", EXECUTABLE, "main", "-I./include -L. -lstrings -g ");
}
