#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

int parse_line(char *line, char **argv) {
    int argc = 0;
    char *token = strtok(line, " \t\r\n");
    while (token != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    argv[argc] = NULL;
    return argc;
}

int extract_redirection(char **argv, int *argc, char **outfile) {
    *outfile = NULL;
    for (int i = 0; i < *argc; i++) {
        if (strcmp(argv[i], ">") == 0) {
            if (i + 1 >= *argc) {
                fprintf(stderr, "msh: syntax error near unexpected token `newline'\n");
                return -1;
            }
            *outfile = argv[i + 1];
            argv[i] = NULL;
            *argc = i;
            break;
        }
    }
    return 0;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    char *argv[MAX_ARGS];
    int last_status = 0;

    signal(SIGINT, SIG_IGN);

    while (1) {
        while (waitpid(-1, NULL, WNOHANG) > 0) {
        }

        printf("msh[%d]> ", last_status);
        fflush(stdout);

        if (getline(&line, &len, stdin) == -1) {
            printf("\n");
            break;
        }

        int argc = parse_line(line, argv);
        if (argc == 0) {
            continue;
        }

        int background = 0;
        if (strcmp(argv[argc - 1], "&") == 0) {
            background = 1;
            argv[argc - 1] = NULL;
            argc--;
            if (argc == 0) {
                continue;
            }
        }

        char *outfile = NULL;
        if (extract_redirection(argv, &argc, &outfile) < 0) {
            last_status = 1;
            continue;
        }
        if (argc == 0) {
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            free(line);
            return last_status;
        } else if (strcmp(argv[0], "help") == 0) {
            printf("msh - minimal Unix shell\n");
            printf("Built-ins: cd [dir], help, exit\n");
            last_status = 0;
            continue;
        } else if (strcmp(argv[0], "cd") == 0) {
            const char *target = argv[1];
            if (target == NULL) {
                target = getenv("HOME");
                if (target == NULL) {
                    fprintf(stderr, "cd: HOME not set\n");
                    last_status = 1;
                    continue;
                }
            }
            if (chdir(target) != 0) {
                perror("cd");
                last_status = 1;
            } else {
                last_status = 0;
            }
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            last_status = 1;
            continue;
        } else if (pid == 0) {
            if (outfile != NULL) {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror(outfile);
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2");
                    close(fd);
                    exit(1);
                }
                close(fd);
            }
            execvp(argv[0], argv);
            perror(argv[0]);
            exit(127);
        } else {
            if (background) {
                printf("[bg] PID: %d\n", pid);
                last_status = 0;
            } else {
                int status;
                if (waitpid(pid, &status, 0) < 0) {
                    perror("waitpid");
                    last_status = 1;
                } else if (WIFEXITED(status)) {
                    last_status = WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    last_status = 128 + WTERMSIG(status);
                }
            }
        }
    }

    free(line);
    return last_status;
}