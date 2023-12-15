//  CPM.h written by Sum1Code, licensed under the MIT license
//  Copyright (c) 2023 Sum1Code

#pragma once
#ifndef __CPM_AVAIL_

#include <dirent.h>
#include <fnmatch.h>
#include <unistd.h>


#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

#define GET_FILE_NAME(name) (strrchr(name, '/') ? strrchr(name, '/') + 1 : name)

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

typedef enum { CPM_LOG, CPM_WARNING, CPM_ERROR } LOG_LEVEL;

#define CPMLOG(loglevel, msg) _cpmlog(loglevel, msg);

typedef enum { EXECUTABLE, OBJECT, DYNLIB } BuildType_e;

typedef enum {
  D_CREATE,
  D_DELETE,
} DirOps_e;

typedef struct {
  BuildType_e type;
  char *srcs;
  char *include_dir;
  char *project_name;
  char *compiler;
  char *flags;
  char *build_dir;
} BuildProperties_t;

typedef struct StringBuilder {
  char *str;
  size_t strsize;
  size_t strcap;
} String;

typedef struct StringSplice {
  char **strsplice;
  char delim;
  size_t splice_count;
} StringSplice_t;

String *sb_new();
void sb_append(String *sb, char c);
void sb_appendstr(String *sb, char *str);
void sb_free(String *sb);
char *sb_to_string(String *sb);
StringSplice_t *sb_split_at(String *builder, char *delim);
String *sb_copy(String *sb);
void sb_patsubst(String *sb, const char *text_to_replace, const char *replacement);

void _cpmlog(LOG_LEVEL level, const char *msg);
void cpm_target(BuildProperties_t *buildprop, BuildType_e type,
                const char *name, const char *build_dir);
void cpm_init(BuildProperties_t *buildprop, char *compiler);
void _cpm_srcs_create(BuildProperties_t *buildprop, ...);
void _cpm_cflags_create(BuildProperties_t *buildprop, ...);
void cpm_dirops(DirOps_e operation, char *dirpath);
char *cpm_glob_dir(char *dirpath, const char *pattern);
bool shouldRecompile(char *srcfile, char *execfile);
void cpm_compile(BuildProperties_t *prop);
pid_t cpm_compile_async(BuildProperties_t *prop);
void cpm_compile_async_poll(pid_t pid);
void cpm_quick_compile(char *compiler, char *srcs, char *build_dir,
                       BuildType_e target, char *name, char *flags);
pid_t cpm_quick_compile_async(char *compiler, char *srcs, char *build_dir,
                              BuildType_e target, char *name, char *flags);

static bool cpm_setup_warning_once = false;
static bool cpm_setup_ran_once = false;

#define cpm_compile_async_poll_many(pid_array)                                 \
  for (size_t i = 0; i < ARRAY_SIZE(pid_array, pid_t); i++) {                  \
    cpm_compile_async_poll(pid_array[i]);                                      \
  }

#define sb_append_strspace(string_builder_ptr, str)                            \
  sb_appendstr(string_builder_ptr, str);                                       \
  sb_append(string_builder_ptr, ' ');

#define cpm_flags(buildprop_ptr, ...)                                          \
  _cpm_cflags_create(buildprop_ptr, __VA_ARGS__, NULL)
#define cpm_srcs(buildprop_ptr, ...)                                           \
  _cpm_srcs_create(buildprop_ptr, __VA_ARGS__, NULL)

#define ARRAY_SIZE(array, type) (sizeof(array) / sizeof(type))

void _cpmlog(LOG_LEVEL level, const char *msg) {
  time_t current_time;
  struct tm *time_info;

  time(&current_time);
  time_info = localtime(&current_time);

  switch (level) {
  case CPM_LOG:
    printf("%sLOG (%02d:%02d:%02d): %s%s\n", KMAG, time_info->tm_hour,
           time_info->tm_min, time_info->tm_sec, msg, KNRM);
    break;
  case CPM_ERROR:
    printf("%sCOMPILATION HALTED: %s%s\n", KRED, msg, KNRM);
    perror("HALTED at:");
    exit(1);
    break;
  case CPM_WARNING:
    printf("%sWARNING (%02d:%02d:%02d): %s%s\n", KYEL, time_info->tm_hour,
           time_info->tm_min, time_info->tm_sec, msg, KNRM);
    break;
  default:
    printf("%sInvalid Log_level%s\n", KMAG, KNRM);
    break;
  }
}

void cpm_target(BuildProperties_t *buildprop, BuildType_e type,
                const char *name, const char *build_dir) {
  buildprop->project_name = (char *)name;
  buildprop->type = type;
  buildprop->build_dir = (char *)build_dir;
}

#define STRING_ALLOC_SIZE 255
void cpm_init(BuildProperties_t *buildprop, char *compiler) {
  buildprop->compiler = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->flags = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->include_dir = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->project_name = (char *)malloc(STRING_ALLOC_SIZE);
  buildprop->srcs = (char *)malloc(STRING_ALLOC_SIZE);

  if (buildprop->compiler == NULL || buildprop->flags == NULL ||
      buildprop->include_dir == NULL || buildprop->project_name == NULL ||
      buildprop->srcs == NULL) {
    perror("Memory allocation error");
    exit(EXIT_FAILURE);
  }

  // Initialize all fields to empty strings (null-terminated)
  buildprop->compiler = compiler;
  buildprop->flags[0] = '\0';
  buildprop->include_dir[0] = '\0';
  buildprop->project_name[0] = '\0';
  buildprop->srcs[0] = '\0';
}

void _cpm_srcs_create(BuildProperties_t *buildprop, ...) {
  va_list args;
  va_start(args, buildprop);
  char *arg = va_arg(args, char *);
  String *builder = sb_new();
  while (arg != NULL) {
    sb_appendstr(builder, arg);
    // char *newSrc = appendToString(buildprop->srcs, arg);
    // free(buildprop
    //          ->srcs);         // Free the old srcs if they were dynamically
    //          allocated
    // buildprop->srcs = newSrc; // Update srcs with the new concatenated string
    arg = va_arg(args, char *);
  }
  va_end(args);
  buildprop->srcs = sb_to_string(builder);
}

void _cpm_cflags_create(BuildProperties_t *buildprop, ...) {
  va_list args;
  va_start(args, buildprop);
  String *resbuild = sb_new();
  char *arg = va_arg(args, char *);
  while (arg != NULL) {
    sb_append_strspace(resbuild, arg);
    arg = va_arg(args, char *);
  }
  va_end(args);
  buildprop->flags = sb_to_string(resbuild);
}

void cpm_quick_compile(char *compiler, char *srcs, char *build_dir,
                       BuildType_e target, char *name, char *flags) {
  BuildProperties_t bp;
  cpm_init(&bp, compiler);
  if (!flags)
    cpm_flags(&bp, " ");
  else
    cpm_flags(&bp, flags);
  cpm_srcs(&bp, srcs);
  cpm_target(&bp, target, name, build_dir);
  cpm_compile(&bp);
}

pid_t cpm_quick_compile_async(char *compiler, char *srcs, char *build_dir,
                              BuildType_e target, char *name, char *flags) {
  BuildProperties_t bp;
  cpm_init(&bp, compiler);
  cpm_flags(&bp, flags);
  cpm_srcs(&bp, srcs);
  cpm_target(&bp, target, name, build_dir);
  int pid = cpm_compile_async(&bp);
  return pid;
}

void sb_patsubst(String *sb, const char *text_to_replace,
                 const char *replacement) {
  size_t text_len = strlen(text_to_replace);
  size_t replace_len = strlen(replacement);

  char *start = sb->str;
  while ((start = strstr(start, text_to_replace)) != NULL) {
    // Calculate the length of the portion before the match
    size_t prefix_len = start - sb->str;

    // Calculate the length of the portion after the match
    size_t suffix_len = sb->strsize - prefix_len - text_len;

    // Allocate a temporary buffer for the modified string
    char *temp_buffer =
        (char *)malloc(prefix_len + replace_len + suffix_len + 1);

    // Copy the prefix
    strncpy(temp_buffer, sb->str, prefix_len);
    temp_buffer[prefix_len] = '\0';

    // Copy the replacement
    strcat(temp_buffer, replacement);

    // Copy the suffix
    strcat(temp_buffer, start + text_len);

    // Update StringBuilder's buffer
    free(sb->str);
    sb->str = temp_buffer;
    sb->strsize = prefix_len + replace_len + suffix_len;

    // Move 'start' ahead to avoid an infinite loop
    start = sb->str + prefix_len + replace_len;
  }
}

void cpm_compile(BuildProperties_t *prop) {
  String *sbuild = sb_new();
  sb_append_strspace(sbuild, prop->compiler);
  sb_append_strspace(sbuild, prop->flags);
  switch (prop->type) {
  case DYNLIB:
    sb_append_strspace(sbuild, "-shared -fpic");
    break;
  case OBJECT:
    sb_append_strspace(sbuild, "-c -fpic");
    break;
  case EXECUTABLE:
    break;
  default:
    CPMLOG(CPM_ERROR, "build type not supported");
    break;
  }
  sb_append_strspace(sbuild, "-o");
  sb_appendstr(sbuild, prop->build_dir);
  sb_appendstr(sbuild, "/");
  sb_append_strspace(sbuild, prop->project_name);
  sb_append_strspace(sbuild, prop->srcs);
  char *cmdstr = strdup(sbuild->str);
  CPMLOG(CPM_LOG, cmdstr);
  system(cmdstr);
}

pid_t cpm_compile_async(BuildProperties_t *prop) {
  pid_t kawaii_child = fork();
  if (kawaii_child == -1) {
    CPMLOG(CPM_ERROR, "My children... No...: ");
  } else if (kawaii_child == 0) {
    cpm_compile(prop);
    exit(0);
  }

  return kawaii_child;
}

void cpm_compile_async_poll(pid_t pid) {
  int status;
  if (waitpid(pid, &status, 0) == -1)
    CPMLOG(CPM_ERROR, "I'm impatient! so is my child (waitpid error)");
  if (WIFSIGNALED(status)) {
    printf("Compilation terminated by signal: %d\n", WTERMSIG(status));
  }
}

#ifdef _WIN32
char *cpm_glob_dir(char *dirpath, const char *pattern) {
  CPMLOG(
      CPM_ERROR,
      "TODO: glob implementation for windows not available yet! sowwyy... >w<")
}
#else
char *cpm_glob_dir(char *dirpath, const char *pattern) {
  DIR *dir;
  struct dirent *entry;
  String *resbuild = sb_new();
  if ((dir = opendir(dirpath)) == NULL) {
    perror("opendir");
    return NULL;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (fnmatch(pattern, entry->d_name, 0) == 0) {
      sb_appendstr(resbuild, dirpath);
      sb_append(resbuild, '/');
      sb_appendstr(resbuild, entry->d_name);
      sb_append(resbuild, ' ');
    }
  }

  closedir(dir);
  char *result = sb_to_string(resbuild);
  return result;
}
#endif
void cpm_dirops(DirOps_e operation, char *dirpath) {
  char command[256];
  switch (operation) {
  case D_CREATE:

    snprintf(command, sizeof(command), "mkdir -p %s", dirpath);

    if (system(command) == 0) {
      CPMLOG(CPM_LOG, command);
    } else {
      CPMLOG(CPM_ERROR, "DIRECTORY CREATION FAILED!");
    }
    break;
  case D_DELETE:
    snprintf(command, sizeof(command), "rm -rf %s", dirpath);

    if (system(command) == 0) {
      CPMLOG(CPM_LOG, command);
    } else {
      CPMLOG(CPM_ERROR, "DIRECTORY DELETION FAILED!");
    }
    break;
  }
}

bool shouldRecompile(char *srcfile, char *execfile) {
  struct stat sourceInfo, executableInfo;

  if (stat(srcfile, &sourceInfo) == -1)
    CPMLOG(CPM_ERROR, "Failed to stat source file");

  if (stat(execfile, &executableInfo) == -1)
    CPMLOG(CPM_ERROR, "Failed to stat executable file");

  return sourceInfo.st_mtime > executableInfo.st_mtime;
}

#define cpm_rebuild_self(argv)                                                 \
  if (shouldRecompile(__FILE__, argv[0])) {                                    \
    char *change_name_cmd = malloc(255);                                       \
    sprintf(change_name_cmd, "mv %s %s.old", argv[0], argv[0]);                \
    CPMLOG(CPM_WARNING, "changing current executable to old");                 \
    system(change_name_cmd);                                                   \
    CPMLOG(CPM_WARNING, "recompiling build system");                           \
    BuildProperties_t prop;                                                    \
    cpm_init(&prop, "cc");                                                     \
    cpm_target(&prop, EXECUTABLE, GET_FILE_NAME(argv[0]), "./");               \
    cpm_srcs(&prop, __FILE__);                                                 \
    cpm_compile(&prop);                                                        \
    CPMLOG(CPM_WARNING, "RUNNING NEW BUILDER\n---------------------\n");       \
    if (system(argv[0]) != 0)                                                  \
      CPMLOG(CPM_ERROR, "CANNOT RUN NEW BUILDER");                             \
    exit(0);                                                                   \
  }

// SB IMPL
String *sb_new() {
  String *sb_res = malloc(sizeof(String));
  if (!sb_res)
    CPMLOG(CPM_ERROR, "String builder failed to create");
  sb_res->str = calloc(STRING_ALLOC_SIZE, sizeof(char));
  sb_res->strcap = STRING_ALLOC_SIZE;
  sb_res->strsize = 0;
  return sb_res;
}
#define SB_VECTOR_LOAD_FACTOR 2
void sb_append(String *sb, char c) {
  sb->str[sb->strsize] = c;
  ++sb->strsize;
  if (sb->strsize == sb->strcap) {
    char *new_str = realloc(sb->str, sb->strcap * SB_VECTOR_LOAD_FACTOR);
    if (!new_str)
      CPMLOG(CPM_ERROR, "FAILED TO REALLOC STRING");
    memset(new_str + sb->strcap, 0, sb->strcap);
    sb->str = new_str;
    sb->strcap *= 2;
  }
}
void sb_appendstr(String *sb, char *str) {
  for (size_t strsize = 0; strsize < strlen(str); strsize++) {
    sb_append(sb, str[strsize]);
  }
}
void sb_free(String *sb) {
  free(sb->str);
  free(sb);
}

/// WARNING: sb_retstr WILL RETURN THE STRING CONTAINED BUT CONSUME THE STRING
/// BUILDER
char *sb_to_string(String *sb) {
  char *ret = strdup(sb->str);
  sb_free(sb);
  return ret;
}

#define STRING_SPLICE_INIT_SIZE 5
/// Borrows StringBuilder and create a unique StringSplice that should be
/// immutable
StringSplice_t *sb_split_at(String *builder, char *delim) {
  StringSplice_t *splice = malloc(sizeof(StringSplice_t));
  splice->splice_count = 0;
  splice->delim = 0;
  splice->strsplice = (char **)calloc(STRING_SPLICE_INIT_SIZE, sizeof(char *));
  char *src_cpy = strdup(builder->str);
  char *tok = strtok(src_cpy, delim);
  while (tok) {
    ++splice->splice_count;
    size_t copytoken_size = strlen(tok);
    splice->strsplice[splice->splice_count - 1] =
        malloc(copytoken_size * sizeof(char));
    splice->strsplice[splice->splice_count - 1] = strdup(tok);
    tok = strtok(NULL, delim);
  }
  return splice;
}
String *sb_copy(String *sb) {
  String *cpy = malloc(sizeof(String));
  cpy->str = malloc(sb->strcap);
  cpy->strcap = sb->strcap;
  cpy->strsize = sb->strsize;
  strncpy(cpy->str, sb->str, sb->strsize);
  return cpy;
}


#define __CPM_AVAIL_
#endif
