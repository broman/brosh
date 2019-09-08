/**
 * @author broman
 * @since 01/09/2019
 * BROSH - The Broman Shell
 */

#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define clear() printf("\033[H\033[J")
#define MAXLN 1000 // Maximum command length

void init() {
    clear();
    char* username = getenv("USER");
    printf("Welcome to the Broman Shell\n");
    printf("User: @%s\n", username);
    sleep(1);
}

int getInput(char *str, char *machine, char *username) {
    char* buf;
    printf("[%s@%s]> ", username, machine);
    buf = readline("");
    if(strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}

int handle(char* inputString) {
    //TODO accept more than just zero-length commands lol
    char* argv[2];
    argv[0] = inputString;
    argv[1] = NULL;
    pid_t child;
    child = fork();
    if(child == 0) {
        execvp(inputString, argv);
    }
    return 0;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
int main() {
    char *machine;
    gethostname(machine, 1023);
    char* username = getenv("USER");
    char inputString[MAXLN];
    init();
    while(1) {
        if(getInput(inputString, username, machine)) continue;
        handle(inputString);
    }
    #pragma clang diagnostic pop
    return EXIT_SUCCESS;
}