#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <pthread.h>
#include "fs/operations.h"
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/stat.h>

int sockfd;

int setSockAddrUn(char *path, struct sockaddr_un *addr) {
  if (addr == NULL)
    return 0;

  bzero((char *)addr, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;
  strcpy(addr->sun_path, path);

  return SUN_LEN(addr);
}

void * applyCommands() {
    while (1) {
        struct sockaddr_un client_addr;
        socklen_t addrlen;
        char in_buffer[MAX_INPUT_SIZE];
        int c;

        addrlen=sizeof(struct sockaddr_un);
        c = recvfrom(sockfd, in_buffer, sizeof(in_buffer)-1, 0,
            (struct sockaddr *)&client_addr, &addrlen);
        if (c <= 0) continue;
        in_buffer[c]='\0';

        char token;
        char name[MAX_INPUT_SIZE], name2[MAX_INPUT_SIZE];
        int numTokens = sscanf(in_buffer, "%c %s %s", &token, name, name2);
        char type = name2[0];
        FILE* outputFile;

        if (numTokens < 2) {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        int res;
        switch (token) {
            case 'c':
                switch (type) {
                    case 'f':
                        printf("Create file: %s\n", name);
                        res = create(name, T_FILE);
                        break;
                    case 'd':
                        printf("Create directory: %s\n", name);
                        res = create(name, T_DIRECTORY);
                        break;
                    default:
                        fprintf(stderr, "Error: invalid node type\n");
                        exit(EXIT_FAILURE);
                }
                break;
            case 'l':
                res = lookup_operation(name);
                if (res >= 0)
                    printf("Search: %s found\n", name);
                else
                    printf("Search: %s not found\n", name);
                break;
            case 'd':
                printf("Delete: %s\n", name);
                res = delete(name);
                break;
            case 'm':
                printf("Move: %s to %s\n", name, name2);
                res = move(name, name2);
                break;
            case 'p':
                printf("Print to: %s\n", name);
                outputFile = fopen(name, "w");
                if (outputFile == NULL) {
                    res = FAIL;
                    break;
                }
                res = print_tecnicofs_tree(outputFile);
                fclose(outputFile);
                break;
            default: { /* error */
                fprintf(stderr, "Error: command to apply\n");
                exit(EXIT_FAILURE);
            }
        }
        if (sendto(sockfd, &res, sizeof(res), 0, (struct sockaddr *)&client_addr, addrlen) < 0) {
            perror("server: sendto error");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char* argv[]) {
    int i;
    int numberThreads;
    char *path;
    struct sockaddr_un server_addr;
    socklen_t addrlen;

    if(argc != 3) {
        fprintf(stderr, "Error: wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }

    /* number of threads */
    numberThreads = *argv[1] - '0';
    if(numberThreads < 1) {
        fprintf(stderr, "Error: invalid number of threads\n");
        exit(EXIT_FAILURE);
    }

    /* create socket */
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("server: can't open socket");
        exit(EXIT_FAILURE);
    }
    path = argv[2];
    unlink(path);
    addrlen = setSockAddrUn (argv[2], &server_addr);
    if (bind(sockfd, (struct sockaddr *) &server_addr, addrlen) < 0) {
        perror("server: bind error");
        exit(EXIT_FAILURE);
    }

    /* init filesystem */
    init_fs();

    pthread_t tid[numberThreads];

    /* create the thread pool */
    for(i=0; i<numberThreads; i++) {
        if(pthread_create(&tid[i], NULL, applyCommands, NULL) != 0) {
            fprintf(stderr, "Error: error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    /* wait for the thread pool */
    for(i=0; i<numberThreads; i++) {
        pthread_join(tid[i], NULL);
    }

    /* close socket */
    close(sockfd);
    unlink(argv[2]);
    /* release allocated memory */
    destroy_fs();

    exit(EXIT_SUCCESS);
}
