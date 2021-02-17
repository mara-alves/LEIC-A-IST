#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <pthread.h>
#include "fs/operations.h"

#define MAX_COMMANDS 10
#define MAX_INPUT_SIZE 100

FILE *inputfile;
FILE *outputfile;
int numberThreads = 0;

char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;
int headQueue = 0;
int tailQueue = 0;

pthread_mutex_t accessVector;
pthread_cond_t canRemove;
pthread_cond_t canInsert;

int insertCommand(char* data) {
    pthread_mutex_lock(&accessVector);

    while(numberCommands == MAX_COMMANDS)
        pthread_cond_wait(&canInsert, &accessVector);

    strcpy(inputCommands[tailQueue], data);

    tailQueue++;

    if (tailQueue == MAX_COMMANDS) 
        tailQueue = 0;

    numberCommands++;

    pthread_cond_signal(&canRemove);
    pthread_mutex_unlock(&accessVector);
    return 1;
}

char* removeCommand() {
    int commandIndex;

    while(numberCommands == 0)
        pthread_cond_wait(&canRemove, &accessVector);

    commandIndex = headQueue;
    headQueue++;

    if (headQueue == MAX_COMMANDS) {
        headQueue = 0;
    }
    
    numberCommands--;

    return inputCommands[commandIndex];  
}

void errorParse(){
    fprintf(stderr, "Error: command invalid\n");
    exit(EXIT_FAILURE);
}

void processInput(){
    char line[MAX_INPUT_SIZE];

    /* break loop with ^Z or ^D */
    while (fgets(line, sizeof(line)/sizeof(char), inputfile)) {
        char token;
        char name[MAX_INPUT_SIZE];
        char name2[MAX_INPUT_SIZE];

        int numTokens = sscanf(line, "%c %s %s", &token, name, name2);

        /* perform minimal validation */
        if (numTokens < 1) {
            continue;
        }
        switch (token) {
            case 'c':
                if(numTokens != 3)
                    errorParse();
                if(insertCommand(line))
                    break;
                return;
            
            case 'l':
                if(numTokens != 2)
                    errorParse();
                if(insertCommand(line))
                    break;
                return;
            
            case 'd':
                if(numTokens != 2)
                    errorParse();
                if(insertCommand(line))
                    break;
                return;
            
            case 'm':
                if(numTokens != 3)
                    errorParse();
                if(insertCommand(line))
                    break;
                return;

            case '#':
                break;
            
            default: { /* error */
                errorParse();
            }
        }
    }
    insertCommand("End of file");
    return;
}

void * applyCommands(){
    while (1){
        char commandcpy[MAX_INPUT_SIZE];

        pthread_mutex_lock(&accessVector);
        const char* command = removeCommand();
        strcpy(commandcpy, command);
        pthread_cond_signal(&canInsert);
        pthread_mutex_unlock(&accessVector);

        /* 
         * when a thread reaches end of file, end that thread
         * and insert "End of file" again to make sure
         * all threads are going to end
         */
        if (strcmp(commandcpy, "End of file")==0){
            insertCommand("End of file");
            return NULL;
        }

        char token;
        char name[MAX_INPUT_SIZE], name2[MAX_INPUT_SIZE];
        int numTokens = sscanf(commandcpy, "%c %s %s", &token, name, name2);
        char type = name2[0];

        if (numTokens < 2) {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        int searchResult;
        switch (token) {
            case 'c':
                if(name2[1] == '\0') {
                    switch (type) {
                        case 'f':
                            printf("Create file: %s\n", name);
                            create(name, T_FILE);
                            break;
                        case 'd':
                            printf("Create directory: %s\n", name);
                            create(name, T_DIRECTORY);
                            break;
                        default:
                            fprintf(stderr, "Error: invalid node type\n");
                            exit(EXIT_FAILURE);
                    }
                }
                else {
                    fprintf(stderr, "Error: invalid node type\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'l':
                searchResult = lookup_operation(name);
                if (searchResult >= 0)
                    printf("Search: %s found\n", name);
                else
                    printf("Search: %s not found\n", name);
                break;
            case 'd':
                printf("Delete: %s\n", name);
                delete(name);
                break;
            case 'm':
                printf("Move: %s to %s\n", name, name2);
                move(name, name2);
                break;
            default: { /* error */
                fprintf(stderr, "Error: command to apply\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    struct timeval start, end;
    int i;

    /* tecnicofs inputfile outputfile numthreads */
    if(argc != 4) {
        fprintf(stderr, "Error: wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }

    /* number of threads */
    numberThreads = *argv[3] - '0';
    if(numberThreads < 1) {
        fprintf(stderr, "Error: invalid number of threads\n");
        exit(EXIT_FAILURE);
    }

    /* open the input and output files */
    inputfile = fopen(argv[1], "r");
    outputfile = fopen(argv[2], "w");
    if(inputfile==NULL || outputfile==NULL) {
        fprintf(stderr, "Error: couldn't find file(s)\n");
        exit(EXIT_FAILURE);
    }

    /* init mutex */
    pthread_mutex_init(&accessVector, NULL);
    pthread_cond_init(&canInsert, NULL);
    pthread_cond_init(&canRemove, NULL);

    /* init filesystem */
    init_fs();

    pthread_t tid[numberThreads];

    /* get initial time */
    gettimeofday(&start, NULL);

    /* create the thread pool */
    for(i=0; i<numberThreads; i++) {
        if(pthread_create(&tid[i], NULL, applyCommands, NULL) != 0) {
            fprintf(stderr, "Error: error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }
    
    /* process input from file and close it */
    processInput();
    fclose(inputfile);

    /* wait for the thread pool */
    for(i=0; i<numberThreads; i++) {
        pthread_join(tid[i], NULL);
    }

    /* get final time and print total execution time */
    gettimeofday(&end, NULL);
    printf("TecnicoFS completed in %.4f seconds.\n", 
            (end.tv_sec - start.tv_sec) + 
            ((end.tv_usec - start.tv_usec)/1000000.0));

    /* print tree in the output file and close it */
    print_tecnicofs_tree(outputfile);
    fclose(outputfile);

    /* release allocated memory */
    destroy_fs();

    exit(EXIT_SUCCESS);
}
