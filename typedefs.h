#ifndef MYSHELL_TYPEDEFS_H
#define MYSHELL_TYPEDEFS_H

#include "constants.h"
#include <stdio.h>

typedef void (* FunctionPointer)();

typedef struct {
    char* name;
    char* value;
} Variable;

typedef struct {
    char* name;
    char* code;
} Color;

typedef struct {
    char* alias;
    char* command;
} Alias;

typedef struct {
    char* name;
    FunctionPointer function;
    char* help_text;
} Command;

typedef struct {
    int pid;
    int ppid;
    char state;
    char name[MAX_LINE_LENGTH];
} ProcessInfo;

typedef struct {
    char buffer[BUFFER_SIZE];
    char* tokens[MAX_TOKENS];
    _Bool is_processed[MAX_TOKENS];
    int token_count;
    FILE* input_stream;
    FILE* output_stream;
    char* prompt_text;
    int debug_level;
    int exit_status;
    _Bool background;
    char* input_redirect;
    char* output_redirect;
    char* procfs_path;
    _Bool is_input_redirected;
    _Bool is_output_redirected;
    char* history[HISTORY_SIZE];
    int history_count;
    _Bool block_prompt;
    int history_index;
    Alias aliases[MAX_ALIASES];
    int alias_count;
    char* color;
    _Bool color_active;
    Variable variables[MAX_VARIABLES];
    int variable_count;
} Shell;

#endif //MYSHELL_TYPEDEFS_H
