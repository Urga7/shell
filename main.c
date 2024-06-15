#include "shell.h"
#include "utility.h"

Shell* sh;

void repl(_Bool interactive) {
    while(1) {
        fflush(sh->output_stream);
        if(interactive && !sh->block_prompt) {
            if(sh->color_active)
                fprintf(sh->output_stream, "%s%s%s>", sh->color, sh->prompt_text, COLOR_RESET);
            else
                fprintf(sh->output_stream, "%s>", sh->prompt_text);

            fflush(sh->output_stream);
        }

        int history_cmd_offset = 0;
        if(sh->block_prompt) {
            strcpy(sh->buffer, sh->history[sh->history_index]);
            history_cmd_offset = strlen(sh->history[sh->history_index]);
            sh->block_prompt = 0;
        }

        if(fgets(sh->buffer + history_cmd_offset, BUFFER_SIZE - history_cmd_offset, sh->input_stream) == NULL) {
            if(feof(sh->input_stream))
                break;
            else {
                sh->exit_status = errno;
                perror("fgets");
                break;
            }
        }

        remove_newline(sh->buffer);
        if(strcmp(trim_spaces(sh->buffer), "history") && strcmp(trim_spaces(sh->buffer), "!!") &&
           strncmp(trim_spaces(sh->buffer), "!n", 2))
            save_to_history(sh->buffer);

        if(sh->debug_level)
            fprintf(sh->output_stream, "Input line: '%s'\n", sh->buffer);

        tokenize(sh->buffer);
        if(sh->token_count && strcmp(sh->tokens[0], "unalias"))
            map_aliases();

        if(sh->debug_level)
            print_tokens();

        if(sh->token_count) {
            handle_redirects();
            FunctionPointer func = find_builtin(sh->tokens[0]);
            if(func == NULL)
                execute_external();
            else
                execute_builtin(func);
        }

        for(int t = 0; t < sh->token_count; ++t)
            if(sh->is_processed[t]) {
                free(sh->tokens[t]);
                sh->is_processed[t] = 0;
            }
    }
}

int main() {
    signal(SIGCHLD, sigchld_handler);
    sh = start_shell();
    repl(isatty(STDIN_FILENO));
    int exit_status = sh->exit_status;
    stop_shell();
    return exit_status;
}
