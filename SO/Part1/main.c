#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <pthread.h>
#include "fs/operations.h"

#define MAX_COMMANDS 150000
#define MAX_INPUT_SIZE 100

char inputfile_name[MAX_FILE_NAME];
FILE *inputfile;
char outputfile_name[MAX_FILE_NAME];
FILE *outputfile;
int numberThreads = 0;
char synchstrategy[MAX_FILE_NAME];

char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;
int headQueue = 0;

pthread_mutex_t accessVector;
pthread_mutex_t mutex;
pthread_rwlock_t rwl;


/* 
 * Lock functions: 
 * initialize the lock
 * lock depending on type
 * unlock
 */

void init_lock() {
    if (strcmp(synchstrategy, "mutex")==0)
        pthread_mutex_init(&mutex, NULL);
    else if (strcmp(synchstrategy, "rwlock")==0)
        pthread_rwlock_init(&rwl, NULL);
    else if (strcmp(synchstrategy, "nosync")==0)
        return;
    else {
        fprintf(stderr, "Error: invalid synchstrategy\n");
        exit(EXIT_FAILURE);
    }
}

void write_lock() {
    if (strcmp(synchstrategy, "mutex")==0)
        pthread_mutex_lock(&mutex);
    else if (strcmp(synchstrategy, "rwlock")==0)
        pthread_rwlock_wrlock(&rwl);
    else if (strcmp(synchstrategy, "nosync")==0)
        return;
}

void read_lock() {
    if (strcmp(synchstrategy, "mutex")==0)
        pthread_mutex_lock(&mutex);
    else if (strcmp(synchstrategy, "rwlock")==0)
        pthread_rwlock_rdlock(&rwl);
    else if (strcmp(synchstrategy, "nosync")==0)
        return;
}

void unlock_lock() {
    if (strcmp(synchstrategy, "mutex")==0)
        pthread_mutex_unlock(&mutex);
    else if (strcmp(synchstrategy, "rwlock")==0)
        pthread_rwlock_unlock(&rwl);
    else if (strcmp(synchstrategy, "nosync")==0)
        return;
}



void readCommandLine() {
    scanf("%s %s %d %s", 
        inputfile_name, outputfile_name, &numberThreads, synchstrategy);
   
    init_lock();

    if(strcmp(synchstrategy, "nosync") == 0 && numberThreads != 1) {
        fprintf(stderr, "Error: cannot have nosync as strategy when "
                        "there's only one thread\n");
        exit(EXIT_FAILURE);
    }
    
    /* open the input and output files */
    inputfile = fopen(inputfile_name, "r");
    outputfile = fopen(outputfile_name, "w");

    if(inputfile == NULL || outputfile == NULL) {
        fprintf(stderr, "Error: couldn't find file(s)\n");
        exit(EXIT_FAILURE);
    }
}

int insertCommand(char* data) {
    if(numberCommands != MAX_COMMANDS) {
        strcpy(inputCommands[numberCommands++], data);
        return 1;
    }
    return 0;
}

char* removeCommand() {
    if(numberCommands > 0){
        numberCommands--;
        return inputCommands[headQueue++];  
    }
    return NULL;
}

void errorParse(){
    fprintf(stderr, "Error: command invalid\n");
    exit(EXIT_FAILURE);
}

void processInput(){
    char line[MAX_INPUT_SIZE];

    /* break loop with ^Z or ^D */
    while (fgets(line, sizeof(line)/sizeof(char), inputfile)) {
        char token, type;
        char name[MAX_INPUT_SIZE];

        int numTokens = sscanf(line, "%c %s %c", &token, name, &type);

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
            
            case '#':
                break;
            
            default: { /* error */
                errorParse();
            }
        }
    }
}

void * applyCommands(){
    while (1){
        pthread_mutex_lock(&accessVector);
        const char* command = removeCommand();
        pthread_mutex_unlock(&accessVector);

        if (command == NULL){
            return NULL;
        }

        char token, type;
        char name[MAX_INPUT_SIZE];
        int numTokens = sscanf(command, "%c %s %c", &token, name, &type);
        if (numTokens < 2) {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        int searchResult;
        switch (token) {
            case 'c':
                switch (type) {
                    case 'f':
                        write_lock();
                        printf("Create file: %s\n", name);
                        create(name, T_FILE);
                        unlock_lock();
                        break;
                    case 'd':
                        write_lock();
                        printf("Create directory: %s\n", name);
                        create(name, T_DIRECTORY);
                        unlock_lock();
                        break;
                    default:
                        fprintf(stderr, "Error: invalid node type\n");
                        exit(EXIT_FAILURE);
                }
                break;
            case 'l': 
                read_lock();
                searchResult = lookup(name);
                if (searchResult >= 0)
                    printf("Search: %s found\n", name);
                else
                    printf("Search: %s not found\n", name);
                unlock_lock();
                break;
            case 'd':
                write_lock();
                printf("Delete: %s\n", name);
                delete(name);
                unlock_lock();
                break;
            default: { /* error */
                fprintf(stderr, "Error: command to apply\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return NULL;
}

void createThreadPool() {
    pthread_t tid[numberThreads];
    int i;

    for(i=0; i<numberThreads; i++) {
        if(pthread_create(&tid[i], NULL, applyCommands, NULL) != 0) {
            fprintf(stderr, "Error: error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }
    for(i=0; i<numberThreads; i++) {
        pthread_join(tid[i], NULL);
    }
}

int main(int argc, char* argv[]) {
    struct timeval start, end;

    /* init mutex */
    pthread_mutex_init(&accessVector, NULL);

    /* init filesystem */
    init_fs();

    /* inputfile outputfile numthreads synchstrategy */
    readCommandLine();

    /* process input from file and close it */
    processInput();
    fclose(inputfile);

    /* get initial time */
    gettimeofday(&start, NULL);

    /* create the thread pool */
    createThreadPool();

    /* get final time and print total execution time */
    gettimeofday(&end, NULL);
    printf("Tecnicofs completed in %.4f seconds.\n", 
            (end.tv_sec - start.tv_sec) + 
            ((end.tv_usec - start.tv_usec)/1000000.0));

    /* print tree in the output file and close it */
    print_tecnicofs_tree(outputfile);
    fclose(outputfile);

    /* release allocated memory */
    destroy_fs();

    exit(EXIT_SUCCESS);
}
