#include "utility.h"

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

void sigchld_handler() {
    int pid;
    int status;
    int serrno;
    serrno = errno;
    while(1) {
        pid = waitpid(WAIT_ANY, &status, WNOHANG);
        if(pid <= 0)
            break;
    }

    errno = serrno;
}

void remove_newline(char* str) {
    int c;
    for(c = 0; str[c] != '\n' && str[c] != '\r'; ++c);
    str[c] = '\0';
}

char* remove_brackets(char* str) {
    str[strlen(str) - 1] = '\0';
    return str + 1;
}

char* trim_spaces(char* str) {
    char* end;
    while(*str != '\0' && isspace((unsigned char) *str))
        str++;

    if(*str == '\0')
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char) *end))
        end--;

    *(end + 1) = '\0';
    return str;
}

int compare_int(const void* a, const void* b) {
    return (*(int*) a - *(int*) b);
}

int compare_process_info(const void* a, const void* b) {
    return ((ProcessInfo*) a)->pid - ((ProcessInfo*) b)->pid;
}

void close_file(int fd) {
    if(fd != STDIN_FILENO && fd != STDOUT_FILENO && close(fd) == -1) {
        perror("close");
        exit(errno);
    }
}

void copy_data(int input_fd, int output_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_written;
    while((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(output_fd, buffer, bytes_read);
        if(bytes_written != bytes_read) {
            perror("write");
            exit(errno);
        }
    }
}