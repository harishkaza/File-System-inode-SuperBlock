#include <stdio.h>
#include <string.h>

const char *INITFS_COMMAND = "initfs";
const char *QUIT_COMMAND = "q";
const char *LIST_ALL_FREE_BLOCKS = "listfree";
const char *TEST_INIT = "testinit";
const char *LIST_ALL_FREE_INODES = "listifree";
const char *LOADFS_COMMAND = "loadfs";


//this method is used to get the input from user
char *getInputFromCLI(){
    int position = 0;
    char *buffer = malloc(sizeof(char) * 100);
    int c;

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;
    }
}

//split the input from user into an array to strings
char **splitInput(char *input){
    char **tokens = malloc(100 * sizeof(char*));
    char *token;
    int position = 0;

    token = strtok(input, " \t\r\n\a");

    while(token!=NULL){
        tokens[position] = token;
        position++;
        token = strtok(NULL, " \t\r\n\a");
    }

    tokens[position] = NULL;
    return tokens;
}

//handle the user's input
void handleCommand(char **argc){
    if(argc[0] == NULL){
        printf("No input\n");
        return;
    }

    if(strcmp(argc[0],INITFS_COMMAND) == 0){//call the initfs method
        initfs(argc[1], atoi(argc[2]), atoi(argc[3]));
    }
    else if(strcmp(argc[0],QUIT_COMMAND) == 0){//save everything and quit
        quitProgram();
    }
    else if(strcmp(argc[0], LIST_ALL_FREE_BLOCKS) == 0){//list all the free data blocks
        get_all_free_blocks();
    }
    else if(strcmp(argc[0], TEST_INIT) == 0){//test if init worked correctly
        test_init();
    }
    else if(strcmp(argc[0], LIST_ALL_FREE_INODES) == 0){//list all the free inodes
        get_free_inodes();
    }
    else if(strcmp(argc[0],LOADFS_COMMAND) == 0){//call the initfs method
        loadfs(argc[1]);
    }
    else{//unknown command
        printf("Invalid command\n");
    }
}