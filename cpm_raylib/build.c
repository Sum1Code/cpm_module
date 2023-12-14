#include "CPM.h"
void init() {
  cpm_dirops(D_CREATE, "build/src");
}

void buildobjects() {
  String *sb = sb_new();
  sb_appendstr(sb, cpm_glob_dir("./src", "*.c"));
  StringSplice_t *srcs = sb_split_at(sb, " ");
  sb_patsubst(sb, ".c", ".o");
  StringSplice_t *objs = sb_split_at(sb, " ");
  sb_free(sb);

  pid_t compilation_proc[srcs->splice_count];
  for (size_t i = 0; i < srcs->splice_count; i++) {
    compilation_proc[i] = cpm_quick_compile_async(
        "clang", srcs->strsplice[i], "build", OBJECT, objs->strsplice[i],
        "-DPLATFORM_DESKTOP -D_GNU_SOURCE "
        "-I./src/external/glfw/include ");
  }

  cpm_compile_async_poll_many(compilation_proc);
}

void cpm_build_raylib() {
    String* str = sb_new();
    sb_append_strspace(str, "ar");
    sb_append_strspace(str, "rcs");
    sb_append_strspace(str, "libraylib.a");
    sb_append_strspace(str, cpm_glob_dir("build/src", "*.o"));
    system(sb_to_string(str));
}

int main(){
    regen();
    buildobjects();
    cpm_build_raylib();
}