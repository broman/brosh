/**
 * @author broman
 * @since 01/09/2019
 * BROSH - The Broman Shell
 */

#include <dirent.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>



#define clear() printf("\033[H\033[J")
#define MAXLN 1000 // Maximum command length

struct ctx {
    char* username;
    char *hostname;
};

void init() {
    clear();
    char* username = getenv("USER");
    printf("Welcome to the Broman Shell\n");
    printf("User: @%s\n", username);
    sleep(1);
    //clear();
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

void pwd() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

void ls() {
    DIR *dir;
    struct dirent *file;

    dir = opendir(".");
    while((file = readdir(dir)) != NULL) {
        printf("%s  ", file->d_name);
    }
    printf("\n");
    closedir(dir);
}

int commandHandler(char* input) {
    int amnt_cmds = 4, i, switchArg = 0;
    char* cmd_list[amnt_cmds];
    cmd_list[0] = "exit";
    cmd_list[1] = "pwd";
    cmd_list[2] = "clear";
    cmd_list[3] = "ls";
    cmd_list[4] = "echo";

    for(i = 0; i < amnt_cmds; i++)
        if(strcmp(input, cmd_list[i]) == 0)
            switchArg = i + 1;

    switch(switchArg) {
        case 1:
            exit(0);
        case 2:
            pwd(); return 1;
        case 3:
            clear(); return 1;
        case 4:
            ls(); return 1;
        default:
            printf("Command not found."); return 1;
    }
}

int main() {
    char* machine;
    gethostname(machine, 1023);
    struct ctx context = {.username = getenv("USER"), .hostname = machine};
    char inputString[MAXLN];
    init();
    while(1) {
        //pwd();
        if (getInput(inputString, context.hostname, context.username))
            continue;
        commandHandler(inputString);
    }
}
