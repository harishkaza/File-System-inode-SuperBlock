#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "initfs.c"
#include "quitProgram.c"
#include "util.c"

void main(){
    while(1){
        printf("Please enter a command to continue\n");
        char *inputline, token;
        int argv;
        char **argc;

        //get the input from user
        inputline = getInputFromCLI();

        //split the input by space
        argc = splitInput(inputline);

        //handle user input
        handleCommand(argc);

    }
}
