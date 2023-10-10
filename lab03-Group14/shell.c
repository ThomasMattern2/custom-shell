#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024
#define MAX_BG_PROCESSES 10

// Function to check and print the status of completed background processes
void checkBackgroundProcesses(pid_t background_processes[], int *num_background_processes) {
    int status;
    pid_t pid;

    for (int i = 0; i < *num_background_processes; i++) {
        pid = waitpid(background_processes[i], &status, WNOHANG);

        if (pid == -1) {
            perror("waitpid");
        } else if (pid == 0) {
            continue;
        } else {
            printf("\nBackground process %d terminated\n", pid);

            for (int j = i; j < (*num_background_processes) - 1; j++) {
                background_processes[j] = background_processes[j + 1];
            }
            (*num_background_processes)--;
        }
    }
}
int main() {
    char buffer[BUFLEN];
    char* parsedinput;
    char* args[20];
    pid_t background_processes[MAX_BG_PROCESSES];
    int num_background_processes = 0;

    printf("Welcome to the Group 14 shell! Enter commands, enter 'quit' to exit\n");
    while (1) {
        // FEATURE 1
        printf("$ ");
        char* input = fgets(buffer, sizeof(buffer), stdin);
        if (!input) {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }
        parsedinput = (char*)malloc(BUFLEN * sizeof(char));
        size_t parselength = trimstring(parsedinput, input, BUFLEN);

        if (strcmp(parsedinput, "quit") == 0) {
            printf("Bye!!\n");
            break;
        } 
        // FEATURE 3, 4 and 5 BELOW
        else {
            int background = runinbackground(parsedinput, strlen(parsedinput));

            if (background) {
                parsedinput[strlen(parsedinput) - 1] = '\0';
            }

            int pipe_pos = findpipe(parsedinput, strlen(parsedinput));
            if (pipe_pos != -1) {
                parsedinput[pipe_pos] = '\0';
                char* first_command = parsedinput;
                char* second_command = parsedinput + pipe_pos + 1;

                trimstring(first_command, first_command, strlen(first_command));
                trimstring(second_command, second_command, strlen(second_command));

                int pipefd[2];
                if (pipe(pipefd) == -1) {
                    perror("pipe");
                    continue;
                }

                pid_t child_pid1, child_pid2;

                if ((child_pid1 = fork()) == -1) {
                    perror("fork");
                    continue;
                }

                if (child_pid1 == 0) {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);

                    close(pipefd[1]);

                    char* token = strtok(first_command, " ");
                    int i = 0;
                    while (token != NULL) {
                        args[i] = token;
                        token = strtok(NULL, " ");
                        i++;
                    }
                    args[i] = NULL;

                    execvp(args[0], args);
                    perror("execve");
                    exit(1);
                } else {
                    if ((child_pid2 = fork()) == -1) {
                        perror("fork");
                        continue;
                    }
                    if (child_pid2 == 0) {
                        close(pipefd[1]);  // Close the write end of the pipe

                        dup2(pipefd[0], STDIN_FILENO);

                        close(pipefd[0]);  // Close the read end of the pipe

                        char* token = strtok(second_command, " ");
                        int i = 0;
                        while (token != NULL) {
                            args[i] = token;
                            token = strtok(NULL, " ");
                            i++;
                        }
                        args[i] = NULL;

                        execvp(args[0], args);
                        perror("execve");
                        exit(1);
                    } else {
                        close(pipefd[0]);
                        close(pipefd[1]);

                        if (background) {
                            background_processes[num_background_processes++] = child_pid1;
                            background_processes[num_background_processes++] = child_pid2;
                        } else {
                            waitpid(child_pid1, NULL, 0);
                            waitpid(child_pid2, NULL, 0);
                        }
                    }
                }
            } else {
                char* token = strtok(parsedinput, " ");
                int i = 0;
                while (token != NULL) {
                    args[i] = token;
                    token = strtok(NULL, " ");
                    i++;
                }
                args[i] = NULL;

                pid_t forkV = fork();
                if (forkV == 0) {
                if (execvp(args[0], args) == -1) {
                    fprintf(stderr, "Error running command in execve\n");
                    exit(1);
                }
                } else if (forkV == -1) {
                    perror("fork");
                } else {
                    if (background) {
                        background_processes[num_background_processes++] = forkV;
                    } else {
                        waitpid(forkV, NULL, 0);
                    }

                    checkBackgroundProcesses(background_processes, &num_background_processes);
                }
            }
        }
        free(parsedinput);
    }

    return 0;
}