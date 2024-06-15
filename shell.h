#ifndef MYSHELL_SHELL_H
#define MYSHELL_SHELL_H

#include "constants.h"
#include "typedefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>

Shell* start_shell();
void stop_shell();
void save_to_history(char* command);
void* find_builtin(char* cmd);
char* find_color(char* color_name);
char* get_value(char* varname);
void print_tokens();
void handle_redirects();
void expand_variables(char* buffer);
void tokenize(char* buffer);
void map_aliases();
void execute_external();
void execute_builtin(FunctionPointer function);

void status_handler();
void exit_handler();
void help_handler();
void debug_handler();
void prompt_handler();
void print_handler();
void echo_handler();
void len_handler();
void sum_handler();
void calc_handler();
void basename_handler();
void dirname_handler();
void dirch_handler();
void dirwd_handler();
void dirmk_handler();
void dirrm_handler();
void dirls_handler();
void rename_handler();
void unlink_handler();
void remove_handler();
void linkhard_handler();
void linksoft_handler();
void linkread_handler();
void linklist_handler();
void cpcat_handler();
void pid_handler();
void ppid_handler();
void uid_handler();
void euid_handler();
void gid_handler();
void egid_handler();
void sysinfo_handler();
void proc_handler();
void pids_handler();
void pinfo_handler();
void waitone_handler();
void waitall_handler();
void pipes_handler();
void lastcmd_handler();
void nthcmd_handler();
void history_handler();
void alias_handler();
void unalias_handler();
void aliaslist_handler();
void setcolor_handler();
void resetcolor_handler();
void colorlist_handler();
void setvar_handler();
void freevar_handler();
void varlist_handler();

extern const Color colors[NUM_COLORS];
extern const Command commands[NUM_COMMANDS];

extern Shell* sh;

#endif //MYSHELL_SHELL_H
