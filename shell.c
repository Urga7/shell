#include "shell.h"
#include "utility.h"

const Color colors[NUM_COLORS] = {
        {"red",     COLOR_RED},
        {"green",   COLOR_GREEN},
        {"yellow",  COLOR_YELLOW},
        {"blue",    COLOR_BLUE},
        {"magenta", COLOR_MAGENTA},
        {"cyan",    COLOR_CYAN},
};

const Command commands[NUM_COMMANDS] = {
        {"debug",      debug_handler,      "Set or see current debug level"},
        {"prompt",     prompt_handler,     "Set or see current prompt text"},
        {"status",     status_handler,     "Get exit status"},
        {"exit",       exit_handler,       "Exit the shell"},
        {"help",       help_handler,       "Display help information"},
        {"print",      print_handler,      "Print the arguments to the standard output"},
        {"echo",       echo_handler,       "Print the arguments and a new line to the standard output"},
        {"len",        len_handler,        "Sum the length of all the arguments"},
        {"sum",        sum_handler,        "Sum all the arguments"},
        {"calc",       calc_handler,       "Perform operation (2nd arg) on the operands (1st and 3rd arg)"},
        {"basename",   basename_handler,   "Print the basename of the path"},
        {"dirname",    dirname_handler,    "Print the directory of the path"},
        {"dirch",      dirch_handler,      "Change the working directory"},
        {"dirwd",      dirwd_handler,      "Print the current working directory"},
        {"dirmk",      dirmk_handler,      "Create a directory"},
        {"dirrm",      dirrm_handler,      "Remove a directory"},
        {"dirls",      dirls_handler,      "Print the contents of the directory"},
        {"rename",     rename_handler,     "Rename the file"},
        {"unlink",     unlink_handler,     "Remove the directory entry"},
        {"remove",     remove_handler,     "Remove the file"},
        {"linkhard",   linkhard_handler,   "Create a hard link"},
        {"linksoft",   linksoft_handler,   "Create a soft link"},
        {"linkread",   linkread_handler,   "Read the destination of the symbolic link"},
        {"linklist",   linklist_handler,   "Find all the hard links to the given file in the current directory"},
        {"cpcat",      cpcat_handler,      "Commands 'cp' and 'cat' merged into one"},
        {"pid",        pid_handler,        "PID of the shell process"},
        {"ppid",       ppid_handler,       "PID of the parent of the shell process"},
        {"uid",        uid_handler,        "UID of the owner of the shell process"},
        {"euid",       euid_handler,       "UID of the active owner of the shell process"},
        {"gid",        gid_handler,        "GID of the group, of which the owner of the shell process is a member of"},
        {"egid",       egid_handler,       "EGID of the group, of which the owner of the shell process is an active member of"},
        {"sysinfo",    sysinfo_handler,    "Displays basic information about the system"},
        {"proc",       proc_handler,       "Set the path to the procfs file system"},
        {"pids",       pids_handler,       "Display the PIDs of the current processes obtained from procfs"},
        {"pinfo",      pinfo_handler,      "Display information about current processes"},
        {"waitone",    waitone_handler,    "Wait for process with specified pid to finish"},
        {"waitall",    waitall_handler,    "Wait for all child processes to finish"},
        {"pipes",      pipes_handler,      "Pipe the commands given as arguments"},
        {"!!",         lastcmd_handler,    "Get the last command used"},
        {"!n",         nthcmd_handler,     "Get the nth last command used"},
        {"history",    history_handler,    "Display the history of commands used"},
        {"alias",      alias_handler,      "Set an alias for a command"},
        {"unalias",    unalias_handler,    "Remove an alias"},
        {"aliaslist",  aliaslist_handler,  "List currently active aliases"},
        {"setcolor",   setcolor_handler,   "Set the color of the prompt text"},
        {"resetcolor", resetcolor_handler, "Reset the color of the prompt text"},
        {"colorlist",  colorlist_handler,  "List the available colors"},
        {"setvar", setvar_handler, "Set the value of a variable"},
        {"freevar", freevar_handler, "Free the space used up by a variable"},
        {"varlist", varlist_handler, "List currently active variables"},
};

Shell* start_shell() {
    Shell* shell = malloc(sizeof(Shell));
    shell->input_stream = stdin;
    shell->output_stream = stdout;
    shell->prompt_text = malloc((PROMPT_TEXT_MAX_LENGTH + 1) * sizeof(char));
    strcpy(shell->prompt_text, DEFAULT_PROMPT_TEXT);
    shell->debug_level = 0;
    shell->exit_status = 0;
    shell->procfs_path = malloc(DIRECTORY_MAX_LENGTH * sizeof(char));
    strcpy(shell->procfs_path, DEFAULT_PROCFS_PATH);
    shell->input_redirect = malloc(PATH_MAX_LENGTH * sizeof(char));
    shell->output_redirect = malloc(PATH_MAX_LENGTH * sizeof(char));
    shell->is_input_redirected = 0;
    shell->is_output_redirected = 0;
    shell->history_count = 0;
    shell->block_prompt = 0;
    shell->history_index = 0;
    shell->alias_count = 0;
    shell->color = NULL;
    shell->color_active = 0;
    shell->variable_count = 0;
    for(int i = 0; i < MAX_TOKENS; ++i)
        shell->is_processed[i] = 0;

    for(int i = 0; i < HISTORY_SIZE; i++)
        shell->history[i] = NULL;

    return shell;
}

void stop_shell() {
    fclose(sh->input_stream);
    fclose(sh->output_stream);
    free(sh->prompt_text);
    free(sh->procfs_path);
    free(sh->input_redirect);
    free(sh->output_redirect);
    for(int i = 0; i < sh->history_count; i++)
        free(sh->history[i]);

    for(int i = 0; i < sh->alias_count; ++i) {
        free(sh->aliases[i].alias);
        free(sh->aliases[i].command);
    }

    free(sh);
}

void save_to_history(char* command) {
    if(sh->history_count < HISTORY_SIZE) {
        sh->history[sh->history_count++] = strdup(command);
    } else {
        free(sh->history[0]);
        for(int i = 1; i < HISTORY_SIZE; ++i)
            sh->history[i - 1] = sh->history[i];

        sh->history[HISTORY_SIZE - 1] = strdup(command);
    }
}

void* find_builtin(char* cmd) {
    for(int c = 0; c < NUM_COMMANDS; ++c)
        if(strcmp(commands[c].name, cmd) == 0)
            return commands[c].function;

    return NULL;
}

char* find_color(char* color_name) {
    for(int c = 0; c < NUM_COLORS; ++c)
        if(strcmp(colors[c].name, color_name) == 0)
            return colors[c].code;

    return NULL;
}

char* get_value(char* varname) {
    for(int v = 0; v < sh->variable_count; ++v)
        if(strcmp(sh->variables[v].name, varname) == 0)
            return sh->variables[v].value;

    return NULL;
}

void print_tokens() {
    for(int t = 0; t < sh->token_count; ++t)
        fprintf(sh->output_stream, "Token %d: '%s'\n", t, sh->tokens[t]);
}

void handle_redirects() {
    sh->background = 0;
    sh->is_input_redirected = 0;
    sh->is_output_redirected = 0;
    for(int t = sh->token_count - 1; t >= 0 && t >= sh->token_count - 3; --t) {
        size_t len = strlen(sh->tokens[t]);
        if(len == 1 && sh->tokens[t][0] == '&') {
            sh->background = 1;
            sh->tokens[t] = NULL;
        } else if(len > 1 && sh->tokens[t][0] == '<') {
            sh->is_input_redirected = 1;
            strcpy(sh->input_redirect, &(sh->tokens[t][1]));
            sh->tokens[t] = NULL;
        } else if(len > 1 && sh->tokens[t][0] == '>') {
            sh->is_output_redirected = 1;
            strcpy(sh->output_redirect, &(sh->tokens[t][1]));
            sh->tokens[t] = NULL;
        } else {
            break;
        }

        --sh->token_count;
    }

    if(sh->debug_level) {
        if(sh->is_input_redirected)
            fprintf(sh->output_stream, "Input redirect: '%s'\n", sh->input_redirect);

        if(sh->is_output_redirected)
            fprintf(sh->output_stream, "Output redirect: '%s'\n", sh->output_redirect);

        fflush(sh->output_stream);
    }
}

void expand_variables(char* buffer) {
    if (buffer == NULL || *buffer == '\0')
        return;

    char buffer_expanded[BUFFER_SIZE];
    char* dest = buffer_expanded;
    for (char* src = buffer; *src != '\0'; ++src) {
        if (*src == '$' && isalpha(*(src + 1))) {
            char var_name[MAX_VARNAME_LENGTH];
            char* var_start = src + 1;
            char* var_end = var_start;
            while (isalnum(*var_end) || *var_end == '_')
                ++var_end;

            int var_length = var_end - var_start;
            strncpy(var_name, var_start, var_length);
            var_name[var_length] = '\0';
            const char* var_value = get_value(var_name);
            if (var_value)
                while (*var_value)
                    *dest++ = *var_value++;

            src = var_end - 1;
        } else {
            *dest++ = *src;
        }
    }

    *dest = '\0';
    strcpy(buffer, buffer_expanded);
}

void tokenize(char* buffer) {
    sh->token_count = 0;
    expand_variables(buffer);
    _Bool quotation_active = 0;
    _Bool reading = 0;
    for(int c = 0; buffer[c] != '\0'; ++c) {
        if(buffer[c] == '"') {
            quotation_active = !quotation_active;
            buffer[c] = '\0';
            continue;
        }

        if(quotation_active && reading)
            continue;

        if(buffer[c] == '#' && !quotation_active && !reading) {
            buffer[c] = '\0';
            return;
        }

        if(quotation_active || buffer[c] != ' ') {
            if(reading)
                continue;

            sh->tokens[sh->token_count++] = &(buffer[c]);
            reading = 1;
        }

        if(quotation_active)
            continue;

        while(buffer[c] == ' ') {
            reading = 0;
            if(buffer[c] == '\0')
                return;

            buffer[c] = '\0';
        }
    }

    sh->tokens[sh->token_count] = NULL;
}

void map_aliases() {
    for(int t = 0; t < sh->token_count; ++t)
        for(int a = 0; a < sh->alias_count; ++a)
            if(strcmp(sh->aliases[a].alias, sh->tokens[t]) == 0) {
                sh->tokens[t] = strdup(sh->aliases[a].command);
                sh->is_processed[t] = 1;
            }
}

void execute_external() {
    fflush(sh->input_stream);
    pid_t pid = fork();
    if(pid == -1) {
        sh->exit_status = errno;
        perror("fork");
        return;
    }

    if(pid == 0) {
        if(sh->is_output_redirected) {
            int fd = open(sh->output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if(fd == -1) {
                sh->exit_status = errno;
                perror("open");
                return;
            }

            fflush(sh->output_stream);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        if(sh->is_input_redirected) {
            int fd = open(sh->input_redirect, O_RDONLY);
            if(fd == -1) {
                sh->exit_status = errno;
                perror("open");
                return;
            }

            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        execvp(sh->tokens[0], sh->tokens);
        sh->exit_status = 127;
        perror("exec");
        _exit(sh->exit_status);
    } else {
        int status;
        if(!sh->background)
            waitpid(pid, &status, 0);

        if(WIFEXITED(status))
            sh->exit_status = WEXITSTATUS(status);
        else
            sh->exit_status = 1;
    }
}

void execute_builtin(FunctionPointer function) {
    if(sh->debug_level) {
        if(sh->background)
            fprintf(sh->output_stream, "Executing builtin '%s' in background\n", sh->tokens[0]);
        else
            fprintf(sh->output_stream, "Executing builtin '%s' in foreground\n", sh->tokens[0]);
    }

    int fd_in_backup;
    if(sh->is_input_redirected) {
        fd_in_backup = dup(STDIN_FILENO);
        int fd = open(sh->input_redirect, O_RDONLY);
        if(fd == -1) {
            sh->exit_status = errno;
            perror("open");
            return;
        }

        fflush(sh->input_stream);
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    int fd_out_backup;
    if(sh->is_output_redirected) {
        fflush(sh->output_stream);
        fd_out_backup = dup(STDOUT_FILENO);
        int fd = open(sh->output_redirect, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if(fd == -1) {
            sh->exit_status = errno;
            perror("open");
            return;
        }

        fflush(sh->output_stream);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    if(sh->background) {
        fflush(sh->input_stream);
        pid_t pid = fork();
        if(pid < 0) {
            sh->exit_status = errno;
            perror("fork");
            return;
        }

        if(pid == 0) {
            function();
            _exit(sh->exit_status);
        }
    } else {
        function();
    }

    if(sh->is_input_redirected) {
        fflush(sh->input_stream);
        dup2(fd_in_backup, STDIN_FILENO);
        close(fd_in_backup);
    }

    if(sh->is_output_redirected) {
        fflush(sh->output_stream);
        dup2(fd_out_backup, STDOUT_FILENO);
        close(fd_out_backup);
    }
}

void varlist_handler() {
    sh->exit_status = 0;
    if(sh->variable_count == 0) {
        fprintf(sh->output_stream, "No variables set\n");
        return;
    }

    for(int v = 0; v < sh->variable_count; ++v)
        fprintf(sh->output_stream, "%s = %s\n", sh->variables[v].name, sh->variables[v].value);
}

void freevar_handler() {
    if(sh->token_count < 2) {
        fprintf(sh->output_stream, "Usage: freevar 'varname'\n");
        sh->exit_status = 1;
        return;
    }

    const char* name = sh->tokens[1];
    for (int i = 0; i < sh->variable_count; ++i) {
        if (strcmp(sh->variables[i].name, name) == 0) {
            free(sh->variables[i].value);
            free(sh->variables[i].name);
            sh->variables[i] = sh->variables[sh->variable_count - 1];
            --sh->variable_count;
            sh->exit_status = 0;
            return;
        }
    }

    fprintf(sh->output_stream, "Variable '%s' wasn't set\n", name);
    sh->exit_status = 1;
}

void setvar_handler() {
    if(sh->token_count < 2) {
        fprintf(sh->output_stream, "Usage: setvar 'varname'='value'\n");
        sh->exit_status = 1;
        return;
    }

    char* equals_sign = strchr(sh->tokens[1], '=');
    const char* name;
    const char* value;
    if (equals_sign != NULL) {
        *equals_sign = '\0';
        name = sh->tokens[1];
        value = equals_sign + 1;
    }

    for (int i = 0; i < sh->variable_count; ++i) {
        if (strcmp(sh->variables[i].name, name) == 0) {
            free(sh->variables[i].value);
            sh->variables[i].value = strdup(value);
            sh->exit_status = 0;
            return;
        }
    }

    if (sh->variable_count >= MAX_VARIABLES) {
        fprintf(sh->output_stream, "Maximum number of variables (32) reached. User 'free varname' to make space for new variables.\n");
        sh->exit_status = 1;
        return;
    }

    sh->variables[sh->variable_count].name = strdup(name);
    sh->variables[sh->variable_count].value = strdup(value);
    ++sh->variable_count;
    sh->exit_status = 0;
}

void colorlist_handler() {
    fprintf(sh->output_stream, "Available colors:\n");
    for(int c = 0; c < NUM_COLORS; ++c)
        fprintf(sh->output_stream, "%s\n", colors[c].name);
}

void resetcolor_handler() {
    if(sh->color != NULL)
        free(sh->color);

    sh->color_active = 0;
    sh->exit_status = 0;
}

void setcolor_handler() {
    if(sh->token_count < 2) {
        fprintf(sh->output_stream, "Usage: setcolor 'color'\n");
        sh->exit_status = 1;
        return;
    }

    char* color_code = find_color(sh->tokens[1]);
    if(color_code == NULL) {
        fprintf(sh->output_stream, "Invalid color. Try 'colorlist'.\n");
        sh->exit_status = 1;
        return;
    }

    if(sh->color != NULL)
        free(sh->color);

    sh->color = strdup(color_code);
    sh->color_active = 1;
    sh->exit_status = 0;
}

void alias_handler() {
    if(sh->token_count < 3) {
        fprintf(sh->output_stream, "Usage: alias 'command name' 'alias name'\n");
        sh->exit_status = 1;
        return;
    }

    if(sh->alias_count >= MAX_ALIASES) {
        fprintf(sh->output_stream, "Alias limit reached\n");
        sh->exit_status = 1;
        return;
    }

    char* command = sh->tokens[1];
    char* name = sh->tokens[2];
    if(find_builtin(name) != NULL) {
        fprintf(sh->output_stream, "Alias name cannot be another built-in command\n");
        sh->exit_status = 1;
        return;
    }

    sh->aliases[sh->alias_count].command = strdup(command);
    sh->aliases[sh->alias_count].alias = strdup(name);
    sh->alias_count++;
    fprintf(sh->output_stream, "Alias '%s' added\n", name);
    sh->exit_status = 0;
}

void unalias_handler() {
    if(sh->token_count < 2) {
        fprintf(sh->output_stream, "Usage: unalias 'alias name'\n");
        sh->exit_status = 1;
        return;
    }

    char* name = sh->tokens[1];
    for(int i = 0; i < sh->alias_count; ++i) {
        if(strcmp(sh->aliases[i].alias, name) == 0) {
            free(sh->aliases[i].alias);
            free(sh->aliases[i].command);
            sh->aliases[i] = sh->aliases[sh->alias_count - 1];
            sh->alias_count--;
            fprintf(sh->output_stream, "Alias '%s' removed\n", name);
            sh->exit_status = 0;
            return;
        }
    }

    fprintf(sh->output_stream, "Alias not found\n");
    sh->exit_status = 1;
}

void aliaslist_handler() {
    if(sh->alias_count == 0)
        fprintf(sh->output_stream, "No active aliases\n");

    for(int i = 0; i < sh->alias_count; ++i)
        fprintf(sh->output_stream, "alias %s='%s'\n", sh->aliases[i].alias, sh->aliases[i].command);

    sh->exit_status = 0;
}

void history_handler() {
    sh->exit_status = 0;
    if(sh->history_count < 2) {
        fprintf(sh->output_stream, "History is empty.\n");
        return;
    }

    for(int i = 0; i < sh->history_count; ++i)
        fprintf(sh->output_stream, "%d: %s\n", sh->history_count - i, sh->history[i]);
}

void nthcmd_handler() {
    if(sh->token_count < 2) {
        fprintf(sh->output_stream, "Not enough input arguments. See 'help'\n");
        sh->exit_status = 1;
        return;
    }

    int n = atoi(sh->tokens[1]);
    if(n > 32) {
        fprintf(sh->output_stream, "Provided n is too high. Maximum value for n is %d\n", HISTORY_SIZE);
        sh->exit_status = 1;
        return;
    }

    if(n > sh->history_count) {
        fprintf(sh->output_stream, "Command number %d does not exit. Currently only %d commands in history\n", n,
                sh->history_count);
        sh->exit_status = 1;
        return;
    }

    sh->block_prompt = 1;
    sh->history_index = sh->history_count - n;
    char* nth_cmd = sh->history[sh->history_index];
    fprintf(sh->output_stream, "%s>%s", sh->prompt_text, nth_cmd);
    sh->exit_status = 0;
}

void lastcmd_handler() {
    if(sh->history_count < 1) {
        fprintf(sh->output_stream, "History empty\n");
        sh->exit_status = 1;
        return;
    }

    sh->block_prompt = 1;
    sh->history_index = sh->history_count - 1;
    fprintf(sh->output_stream, "%s>%s", sh->prompt_text, sh->history[sh->history_index]);
    sh->exit_status = 0;
}

void pipes_handler() {
    if(sh->token_count < 3) {
        fprintf(stderr, "pipes: at least two stages required\n");
        sh->exit_status = 1;
        return;
    }

    int num_commands = sh->token_count - 1;
    int pipe_fds[num_commands - 1][2];
    pid_t pids[num_commands];
    for(int i = 0; i < num_commands - 1; ++i) {
        if(pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            sh->exit_status = errno;
            return;
        }
    }

    for(int i = 0; i < num_commands; ++i) {
        pids[i] = fork();
        if(pids[i] == -1) {
            perror("fork");
            sh->exit_status = errno;
            return;
        }

        if(pids[i] == 0) {
            if(i > 0)
                dup2(pipe_fds[i - 1][0], STDIN_FILENO);

            if(i < num_commands - 1)
                dup2(pipe_fds[i][1], STDOUT_FILENO);

            for(int j = 0; j < num_commands - 1; ++j) {
                close(pipe_fds[j][0]);
                close(pipe_fds[j][1]);
            }

            tokenize(sh->tokens[i + 1]);
            FunctionPointer function = find_builtin(sh->tokens[0]);
            if(function == NULL) {
                execvp(sh->tokens[0], sh->tokens);
                perror("execvp");
                _exit(127);
            } else {
                function();
                fflush(sh->output_stream);
                _exit(sh->exit_status);
            }
        }
    }

    for(int i = 0; i < num_commands - 1; ++i) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    for(int i = 0; i < num_commands; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
        if(WIFEXITED(status)) {
            sh->exit_status = WEXITSTATUS(status);
        } else {
            sh->exit_status = 1;
        }
    }
}

void waitall_handler() {
    int status;
    while(waitpid(WAIT_ANY, &status, 0) > 0) {
        if(WIFEXITED(status))
            sh->exit_status = WEXITSTATUS(status);
        else
            sh->exit_status = 1;
    }
}

void waitone_handler() {
    pid_t pid_to_wait;
    if(sh->token_count > 1)
        pid_to_wait = atoi(sh->tokens[1]);
    else
        pid_to_wait = -1;

    int status;
    int pid = waitpid(pid_to_wait, &status, 0);
    if(pid == -1)
        sh->exit_status = 0;
    else {
        if(WIFEXITED(status))
            sh->exit_status = WEXITSTATUS(status);
        else
            sh->exit_status = 1;
    }
}

void pinfo_handler() {
    FILE* file;
    char filename[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    ProcessInfo processes[MAX_PROCESSES_COUNT];
    int num_processes = 0;

    DIR* dir = opendir(sh->procfs_path);
    if(dir == NULL) {
        perror("opendir");
        sh->exit_status = 1;
        return;
    }

    struct dirent* entry;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            int pid = atoi(entry->d_name);
            snprintf(filename, sizeof(filename), "%s/%d/stat", sh->procfs_path, pid);
            file = fopen(filename, "r");
            if(file == NULL) {
                perror("fopen");
                continue;
            }

            if(fgets(line, sizeof(line), file) != NULL) {
                sscanf(line, "%d %s %c %d", &processes[num_processes].pid, processes[num_processes].name,
                       &processes[num_processes].state, &processes[num_processes].ppid);
                strcpy(processes[num_processes].name, remove_brackets(processes[num_processes].name));
                num_processes++;
            }

            fclose(file);
        }
    }

    closedir(dir);
    qsort(processes, num_processes, sizeof(ProcessInfo), compare_process_info);
    fprintf(sh->output_stream, "%5s %5s %6s %s\n", "PID", "PPID", "STANJE", "IME");
    for(int i = 0; i < num_processes; i++) {
        fprintf(sh->output_stream, "%5d %5d %6c %s\n", processes[i].pid, processes[i].ppid, processes[i].state,
                processes[i].name);
    }

    sh->exit_status = 0;
}

void pids_handler() {
    DIR* dir = opendir(sh->procfs_path);
    if(dir == NULL) {
        sh->exit_status = errno;
        perror("pids");
        return;
    }

    int pids[MAX_PROCESSES_COUNT];
    int pids_count = 0;
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL)
        if(entry->d_type == DT_DIR && atoi(entry->d_name) != 0)
            pids[pids_count++] = atoi(entry->d_name);

    closedir(dir);
    qsort(pids, pids_count, sizeof(int), compare_int);
    for(int p = 0; p < pids_count; ++p)
        fprintf(sh->output_stream, "%d\n", pids[p]);

    fflush(sh->output_stream);
    sh->exit_status = 0;
}

void proc_handler() {
    if(sh->token_count == 1) {
        fprintf(sh->output_stream, "%s\n", sh->procfs_path);
        sh->exit_status = 0;
        return;
    }

    char* new_path = sh->tokens[1];
    if(access(new_path, F_OK | R_OK) != 0) {
        sh->exit_status = 1;
        return;
    }

    strcpy(sh->procfs_path, new_path);
    sh->exit_status = 0;
}

void sysinfo_handler() {
    struct utsname data;
    if(uname(&data) < 0) {
        sh->exit_status = errno;
        perror("uname");
        return;
    }

    fprintf(sh->output_stream, "Sysname: %s\n", data.sysname);
    fprintf(sh->output_stream, "Nodename: %s\n", data.nodename);
    fprintf(sh->output_stream, "Release: %s\n", data.release);
    fprintf(sh->output_stream, "Version: %s\n", data.version);
    fprintf(sh->output_stream, "Machine: %s\n", data.machine);
    sh->exit_status = 0;
}

void egid_handler() {
    fprintf(sh->output_stream, "%d\n", getegid());
    sh->exit_status = 0;
}

void gid_handler() {
    fprintf(sh->output_stream, "%d\n", getgid());
    sh->exit_status = 0;
}

void euid_handler() {
    fprintf(sh->output_stream, "%d\n", geteuid());
    sh->exit_status = 0;
}

void uid_handler() {
    fprintf(sh->output_stream, "%d\n", getuid());
    sh->exit_status = 0;
}

void ppid_handler() {
    fprintf(sh->output_stream, "%d\n", getppid());
    sh->exit_status = 0;
}

void pid_handler() {
    fprintf(sh->output_stream, "%d\n", getpid());
    sh->exit_status = 0;
}

void cpcat_handler() {
    int input_file_desc = STDIN_FILENO;
    int output_file_desc = STDOUT_FILENO;
    if(sh->token_count >= 2 && sh->tokens[1][0] != '-') {
        input_file_desc = open(sh->tokens[1], O_RDONLY);
        if(input_file_desc == -1) {
            sh->exit_status = errno;
            perror("cpcat");
            return;
        }
    }

    if(sh->token_count >= 3 && sh->tokens[2][0] != '-') {
        output_file_desc = open(sh->tokens[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if(output_file_desc == -1) {
            sh->exit_status = errno;
            perror("cpcat");
            return;
        }
    }

    copy_data(input_file_desc, output_file_desc);
    close_file(input_file_desc);
    close_file(output_file_desc);
    sh->exit_status = 0;
}

void linklist_handler() {
    if(sh->token_count < 2) {
        sh->exit_status = 1;
        return;
    }

    DIR* dir = opendir(".");
    if(dir == NULL) {
        sh->exit_status = errno;
        perror("linklist");
        return;
    }

    struct stat target_stat;
    if(lstat(sh->tokens[1], &target_stat) == -1) {
        sh->exit_status = errno;
        perror("linklist");
        closedir(dir);
        return;
    }

    struct dirent* dir_entry;
    _Bool first_entry = 1;
    while((dir_entry = readdir(dir)) != NULL) {
        struct stat file_stat;
        if(lstat(dir_entry->d_name, &file_stat) == -1) {
            sh->exit_status = errno;
            perror("linklist");
            continue;
        }

        if(S_ISREG(file_stat.st_mode) && file_stat.st_dev == target_stat.st_dev &&
           file_stat.st_ino == target_stat.st_ino) {
            if(!first_entry)
                fprintf(sh->output_stream, "  ");

            fprintf(sh->output_stream, "%s", dir_entry->d_name);
            first_entry = 0;
        }
    }

    fputc('\n', sh->output_stream);
    closedir(dir);
}

void linkread_handler() {
    if(sh->token_count < 2) {
        sh->exit_status = 1;
        return;
    }

    char path[DIRECTORY_MAX_LENGTH];
    ssize_t length = readlink(sh->tokens[1], path, sizeof(path));
    if(length == -1) {
        sh->exit_status = errno;
        perror("linkread");
        return;
    }

    path[length] = '\0';
    fprintf(sh->output_stream, "%s\n", path);
    sh->exit_status = 0;
}

void linksoft_handler() {
    if(sh->token_count < 3) {
        sh->exit_status = 1;
        return;
    }

    if(symlink(sh->tokens[1], sh->tokens[2]) != 0) {
        sh->exit_status = errno;
        perror("linksoft");
        return;
    }

    sh->exit_status = 0;
}

void linkhard_handler() {
    if(sh->token_count < 3) {
        sh->exit_status = 1;
        return;
    }

    if(link(sh->tokens[1], sh->tokens[2]) != 0) {
        sh->exit_status = errno;
        perror("linkhard");
        return;
    }

    sh->exit_status = 0;
}

void remove_handler() {
    if(sh->token_count < 2) {
        sh->exit_status = 1;
        return;
    }

    if(remove(sh->tokens[1]) != 0) {
        sh->exit_status = errno;
        perror("remove");
        return;
    }

    sh->exit_status = 0;
}

void unlink_handler() {
    if(sh->token_count < 2) {
        sh->exit_status = 1;
        return;
    }

    if(unlink(sh->tokens[1]) != 0) {
        sh->exit_status = errno;
        perror("unlink");
        return;
    }

    sh->exit_status = 0;
}

void rename_handler() {
    if(sh->token_count < 3) {
        sh->exit_status = 1;
        return;
    }

    if(rename(sh->tokens[1], sh->tokens[2]) != 0) {
        sh->exit_status = errno;
        perror("rename");
        return;
    }

    sh->exit_status = 0;
}

void dirls_handler() {
    char* path = ".";
    if(sh->token_count > 1)
        path = sh->tokens[1];

    DIR* dir = opendir(path);
    if(dir == NULL) {
        sh->exit_status = errno;
        perror("dirls");
        return;
    }

    struct dirent* dir_entry;
    _Bool first_entry = 1;
    while((dir_entry = readdir(dir)) != NULL) {
        if(!first_entry)
            fprintf(sh->output_stream, "  ");

        first_entry = 0;
        fprintf(sh->output_stream, "%s", dir_entry->d_name);
    }

    fputc('\n', sh->output_stream);
    closedir(dir);
    sh->exit_status = 0;
}

void dirrm_handler() {
    if(sh->token_count < 1) {
        sh->exit_status = 1;
        return;
    }

    if(rmdir(sh->tokens[1]) != 0) {
        sh->exit_status = errno;
        perror("dirrm");
        return;
    }

    sh->exit_status = 0;
}

void dirmk_handler() {
    if(sh->token_count < 1) {
        sh->exit_status = 1;
        return;
    }

    if(mkdir(sh->tokens[1], 0777) != 0) {
        sh->exit_status = errno;
        perror("dirmk");
        return;
    }

    sh->exit_status = 0;
}

void dirwd_handler() {
    char* mode = "base";
    if(sh->token_count > 1) {
        mode = sh->tokens[1];
    }

    char cwd[DIRECTORY_MAX_LENGTH];
    if(getcwd(cwd, sizeof(cwd)) == NULL) {
        sh->exit_status = errno;
        perror("dirwd");
        return;
    }

    if(strcmp(mode, "base")) {
        fprintf(sh->output_stream, "%s\n", cwd);
        sh->exit_status = 0;
    } else if(strcmp(mode, "full")) {
        fprintf(sh->output_stream, "%s\n", basename(cwd));
        sh->exit_status = 0;
    } else sh->exit_status = 1;
}

void dirch_handler() {
    if(sh->token_count < 2) {
        if(chdir("/") != 0)
            sh->exit_status = errno;

        sh->exit_status = 0;
        return;
    }

    if(chdir(sh->tokens[1]) != 0) {
        sh->exit_status = errno;
        perror("dirch");
        return;
    }

    sh->exit_status = 0;
}

void dirname_handler() {
    if(sh->token_count < 2) {
        sh->exit_status = 1;
        return;
    }

    fprintf(sh->output_stream, "%s\n", dirname(sh->tokens[1]));
    sh->exit_status = 0;
}

void basename_handler() {
    if(sh->token_count < 2) {
        sh->exit_status = 1;
        return;
    }

    fprintf(sh->output_stream, "%s\n", basename(sh->tokens[1]));
    sh->exit_status = 0;
}

void calc_handler() {
    if(sh->token_count < 4) {
        sh->exit_status = 1;
        return;
    }

    int arg1 = atoi(sh->tokens[1]);
    int arg2 = atoi(sh->tokens[3]);
    int operation = sh->tokens[2][0];
    switch(operation) {
        case '+':
            fprintf(sh->output_stream, "%d\n", arg1 + arg2);
            break;
        case '-':
            fprintf(sh->output_stream, "%d\n", arg1 - arg2);
            break;
        case '*':
            fprintf(sh->output_stream, "%d\n", arg1 * arg2);
            break;
        case '/':
            fprintf(sh->output_stream, "%d\n", arg1 / arg2);
            break;
        case '%':
            fprintf(sh->output_stream, "%d\n", arg1 % arg2);
            break;
    }

    sh->exit_status = 0;
}

void sum_handler() {
    int sum = 0;
    for(int t = 1; t < sh->token_count; ++t)
        sum += atoi(sh->tokens[t]);

    fprintf(sh->output_stream, "%d\n", sum);
    sh->exit_status = 0;
}

void len_handler() {
    size_t arg_length = 0;
    for(int t = 1; t < sh->token_count; ++t)
        arg_length += strlen(sh->tokens[t]);

    fprintf(sh->output_stream, "%zu\n", arg_length);
    sh->exit_status = 0;
}

void echo_handler() {
    print_handler();
    fprintf(sh->output_stream, "\n");
}

void print_handler() {
    for(int t = 1; t < sh->token_count; ++t) {
        fprintf(sh->output_stream, "%s", sh->tokens[t]);
        if(t + 1 != sh->token_count)
            fputc(' ', sh->output_stream);
    }

    sh->exit_status = 0;
}

void status_handler() {
    fprintf(sh->output_stream, "%d\n", sh->exit_status);
}

void debug_handler() {
    if(sh->token_count == 1) {
        fprintf(sh->output_stream, "%d\n", sh->debug_level);
        return;
    }

    sh->debug_level = atoi(sh->tokens[1]);
    sh->exit_status = 0;
}

void prompt_handler() {
    if(sh->token_count == 1) {
        fprintf(sh->output_stream, "%s\n", sh->prompt_text);
        sh->exit_status = 0;
        return;
    }

    if(strlen(sh->tokens[1]) > PROMPT_TEXT_MAX_LENGTH) {
        sh->exit_status = 1;
        return;
    }

    strcpy(sh->prompt_text, sh->tokens[1]);
    sh->exit_status = 0;
}

void exit_handler() {
    if(sh->token_count > 1)
        sh->exit_status = atoi(sh->tokens[1]);

    exit(sh->exit_status);
}

void help_handler() {
    for(int c = 0; c < NUM_COMMANDS; ++c)
        fprintf(sh->output_stream, "%s: %s\n", commands[c].name, commands[c].help_text);

    sh->exit_status = 0;
}