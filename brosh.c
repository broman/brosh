/*
 * Ryan Broman
 * Brosh - The Broman Shell
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAXLINE 80
#define MAXARGS 20
#define MAX_PATH_LENGTH 50
#define TRUE 1

int parseline(char *cmdline, char **argv);

void process_input(int argc, char **argv);
void handle_redir(int argc, char *argv[]);

int main(void)
{
    char cmdline[MAXLINE];
    char *argv[MAXARGS];
    int argc;
    int status;
    pid_t pid;

    while (TRUE)
    {
        printf("brosh> ");

        /* Read the command line */
        fgets(cmdline, MAXLINE, stdin);

        /* Call parseline to build argc/argv */
        argc = parseline(cmdline, argv);
        if(argc == 0) {
            continue;
        }

        if(strcmp(argv[0], "exit") == 0) {
            exit(EXIT_SUCCESS);
        } else if(strcmp(argv[0], "pwd") == 0) {
            char cwd[MAX_PATH_LENGTH];
            getcwd(cwd, MAX_PATH_LENGTH);
            printf("%s\n", cwd);
            continue;
        } else if(strcmp(argv[0], "cd") == 0) {
            char* dir;
            if(argc == 1) dir = getenv("HOME");
            else dir = argv[1];
            if(chdir(dir) < 0) {
                printf("Unable to change directory.");
                continue;
            }
        } else {
            pid = fork();
            switch(pid) {
                case -1:
                    perror("Shell Program fork error");
                    exit(EXIT_FAILURE);
                case 0:
                    process_input(argc, argv);
                    break;
                default:
                    if (wait(&status) == -1)
                        perror("Parent Process error");
                    break;
            }
        }
    }
}

int parseline(char *cmdline, char **argv)
{
    int argc = 0;
    char *separator = " \n\t"; /* Includes space, Enter, Tab */

    argv[argc] = strtok(cmdline, separator);

    while ((argv[argc] != NULL) && (argc+1 < MAXARGS))
        argv[++argc] = strtok((char *) 0, separator);

    return argc;
}

void process_input(int argc, char **argv) {
    handle_redir(argc, argv);

    int ret;
    if ((ret = execvp(argv[0], argv)) == -1) {
        fprintf(stderr, "Error on the exec call\n");
        _exit(EXIT_FAILURE);
    }

}
/* ----------------------------------------------------------------- */
void handle_redir(int argc, char *argv[]) {
    int in_loc = 0, out_loc = 0;
    int i;
    /* Determine the locations of < and > in argv */
    for(i = 0; i < argc; i++) {
        if(strcmp(">", argv[i]) == 0) {
            if(out_loc != 0) {
                printf("Cannot output to more than one file.\n");
                _exit(EXIT_FAILURE);
            } else if(i == 0) {
                printf("No command entered.\n");
                _exit(EXIT_FAILURE);
            }
            out_loc = i;
        } else if(strcmp("<", argv[i]) == 0) {
            if(in_loc != 0) {
                printf("Cannot take input from more than one file.\n");
                _exit(EXIT_FAILURE);
            } else if(i == 0) {
                printf("No command entered.\n");
                _exit(EXIT_FAILURE);
            }
            in_loc = i;
        }
    }
    /* Open the input and output files and redirect stdin and stdout to their file descriptors */
    if(out_loc != 0) {
        if(argv[out_loc + 1] == NULL) {
            printf("No output file provided.\n");
            _exit(EXIT_FAILURE);
        }
        int fd = open(
                argv[out_loc + 1],
                O_RDWR | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR
        );
        // Write-only file, create if not exist, truncate if exists
        // Read/write permission for user only
        if(fd == -1) {
            printf("Error opening output file.\n");
            _exit(EXIT_FAILURE);
        }
        dup2(fd, 1);
        close(fd);
        argv[out_loc] = NULL;
    }
    if(in_loc != 0) {
        if(argv[in_loc - 1] == NULL) {
            printf("No input file provided.\n");
            _exit(EXIT_FAILURE);
        }
        int fd = open(argv[in_loc + 1], O_RDONLY);
        // Read-only file
        if(fd == -1) {
            printf("Error opening input file.\n");
            _exit(EXIT_FAILURE);
        }
        dup2(fd, 0);
        close(fd);
        argv[in_loc] = NULL;
    }
}