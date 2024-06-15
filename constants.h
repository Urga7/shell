#ifndef MYSHELL_CONSTANTS_H
#define MYSHELL_CONSTANTS_H

#define BUFFER_SIZE 512
#define MAX_TOKENS 64
#define NUM_COMMANDS 50
#define PROMPT_TEXT_MAX_LENGTH 8
#define PATH_MAX_LENGTH 128
#define DIRECTORY_MAX_LENGTH 1024
#define DEFAULT_PROMPT_TEXT "mysh"
#define DEFAULT_PROCFS_PATH "/proc"
#define MAX_LINE_LENGTH 1024
#define MAX_PROCESSES_COUNT 4096
#define HISTORY_SIZE 32
#define MAX_ALIASES 32
#define MAX_VARIABLES 32
#define MAX_VARNAME_LENGTH 32
#define NUM_COLORS 6
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RESET   "\033[0m"

#endif //MYSHELL_CONSTANTS_H
