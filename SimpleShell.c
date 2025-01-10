#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define TOTAL_INPUT_LENGTH 1024
#define MAX_HISTORY_SIZE 50

// Function to execute a command in a child process
int command_execution(char *command) {
    pid_t pid = fork();

    if (pid == 0) {
        char *args[TOTAL_INPUT_LENGTH / 2];
        int arg_count = 0;
        char *token = strtok(command, " ");
        while (token != NULL) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;
        if (execvp(args[0], args) == -1) {
            perror("SimpleShell");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {         // Error forking
        perror("SimpleShell");
        return -1;                // Return an error code
    } else {                      // Parent process
        int status;
        waitpid(pid, &status, 0);
        switch (status) {
            case 0:
                printf("\nChild process terminated successfully.\n");
                break;
            case -1:
                perror("Error executing child process");
                break;
            default:
                printf("Child process terminated with status %d\n", status);
        }
        return status;           // Return the exit status of the child process
    }
}

// Function to display command execution details
void execution_detail(const char *command, clock_t start_time) {
    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Command Entered: %s\n", command);
    printf("Process ID: %d\n", getpid());
    printf("Starting Time of command: %ld\n", start_time);
    printf("Ending Time of command: %ld\n", end_time);
    printf("Execution Time: %.2f seconds\n", execution_time);
}

// Function to launch a command and return its exit status
int launch(char *command) {
    clock_t start_time = clock();
    int status = command_execution(command);
    execution_detail(command, start_time);
    return status;
}

int check_echo_command(char *command) {
    if (strchr(command, '/') != NULL || strstr(command, "\"\"") != NULL || strchr(command, '\'') != NULL) {
        return -1; // Error if found
    }
    return 0; // No error
}

int main() {
    char input[TOTAL_INPUT_LENGTH];
    char history[MAX_HISTORY_SIZE][TOTAL_INPUT_LENGTH];
    int history_number = 0;
    int status = 1;

    while (status) {
        printf("SimpleShell> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';

        if (strcmp(input, "history") == 0) {
            printf("Command History:\n");
            int i = 0;
            while (i < history_number) {
                printf("%d: %s\n", i + 1, history[i]);
                i++;
            }
        } else if (strstr(input, "|") != NULL) {
            char *pipe_token;
            char *pipe_commands[10];
            int num_pipes = 0;

            pipe_token = strtok(input, "|");
            while (pipe_token != NULL) {
                pipe_commands[num_pipes++] = pipe_token;
                pipe_token = strtok(NULL, "|");
            }
            if (num_pipes < 2) {
                printf("Error: At least two commands are required for pipes.\n");
                continue;
            }

            int pipes[num_pipes - 1][2];
            int stdin_copy = dup(0);  // Save a copy of stdin

            for (int i = 0; i < num_pipes; i++) {
                if (i < num_pipes - 1) {
                    if (pipe(pipes[i]) == -1) { // Create a pipe
                        perror("pipe");
                        return -1;
                    }
                }

                if (fork() == 0) {
                    if (i > 0) {
                        dup2(pipes[i - 1][0], 0); 
                    }
                    if (i < num_pipes - 1) {
                        dup2(pipes[i][1], 1); 
                    }
                    for (int j = 0; j < num_pipes - 1; j++) {
                        if (j != i - 1) {
                            close(pipes[j][0]);
                        }
                        if (j != i) {
                            close(pipes[j][1]);
                        }
                    }
                    int exit_status = command_execution(pipe_commands[i]);
                    exit(exit_status);
                }
            }
            for (int i = 0; i < num_pipes - 1; i++) {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            for (int i = 0; i < num_pipes; i++) {
                wait(NULL);
            }
            dup2(stdin_copy, 0);  // Restore stdin
            close(stdin_copy);

        } else if (strncmp(input, "echo", 4) == 0) {
            if (check_echo_command(input + 5) == -1) {
                printf("Error: Invalid usage of 'echo' command.\n");
                continue;
            }

            int exit_status = launch(input);
            if (history_number < MAX_HISTORY_SIZE) {
                strcpy(history[history_number], input);
                history_number++;
            } else {
                int i = 0;
                while (i < MAX_HISTORY_SIZE - 1) {
                    strcpy(history[i], history[i + 1]);
                    i++;
                }
                strcpy(history[MAX_HISTORY_SIZE - 1], input);
            }

            if (exit_status == -1) {
                printf("Error executing the command.\n");
            } else {
                printf("Exit Status: %d\n", exit_status);
            }
        } else {
            int exit_status = launch(input);
            if (history_number < MAX_HISTORY_SIZE) {
                strcpy(history[history_number], input);
                history_number++;
            } else {
                int i = 0;
                while (i < MAX_HISTORY_SIZE - 1) {
                    strcpy(history[i], history[i + 1]);
                    i++;
                }
                strcpy(history[MAX_HISTORY_SIZE - 1], input);
            }

            if (exit_status == -1) {
                printf("Error executing the command.\n");
            } else {
                printf("Exit Status: %d\n", exit_status);
            }
        }
    }

    return 0;
}
