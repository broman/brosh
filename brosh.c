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
#define MAXARGS 1000 // Maximum command arguments
#define MAXSTR 1000 // Maximum command length

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

void parseInput(char* inputString, char* retval[]) {
    // Receives an input in the form of a string and separates them by spaces
    char* nextToken = strtok(inputString, " ");
    int i = 0;
    while(nextToken != NULL) {
        retval[i++] = nextToken;
        nextToken = strtok(NULL, " ");
    }
}

int handle(char* inputString) {
    //TODO accept more than just zero-length commands lol
    char argv[MAXARGS];
    parseInput(inputString, &argv);
    pid_t child;
    child = fork();
    if(child == 0) {
        execvp(inputString, argv);
    }
    return 0;
}

int main() {
    char machine[1023];
    gethostname(machine, 1023);
    char* username = getenv("USER");
    char inputString[MAXARGS];
    init();
    while(1) {
        if(getInput(inputString, username, machine)) continue;
        handle(inputString);
    }
    return EXIT_SUCCESS;
}