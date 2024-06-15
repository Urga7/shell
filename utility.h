#ifndef MYSHELL_UTILITY_H
#define MYSHELL_UTILITY_H

#include "typedefs.h"
#include "constants.h"

void sigchld_handler();
void remove_newline(char* str);
char* remove_brackets(char* str);
char* trim_spaces(char* str);
int compare_int(const void* a, const void* b);
int compare_process_info(const void* a, const void* b);
void close_file(int fd);
void copy_data(int input_fd, int output_fd);

#endif //MYSHELL_UTILITY_H
