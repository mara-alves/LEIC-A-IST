#include "tecnicofs-client-api.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

char client_socket[MAX_FILE_NAME] = "/tmp/ClientSocket";
int servlen;
int sockfd;
struct sockaddr_un serv_addr;
struct sockaddr_un client_addr;
socklen_t clilen;

int setSockAddrUn(char *path, struct sockaddr_un *addr) {

  if (addr == NULL)
    return 0;

  bzero((char *)addr, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;
  strcpy(addr->sun_path, path);

  return SUN_LEN(addr);
}


int tfsCreate(char *filename, char nodeType) {
  int res;
  char string[MAX_INPUT_SIZE] = "c ";

  strcat(string, filename);
  strcat(string, " ");
  strcat(string, &nodeType);

  if (sendto(sockfd, string, strlen(string)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  }
  if (recvfrom(sockfd, &res, sizeof(res), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  }
  return res;
}


int tfsDelete(char *path) {
  int res;
  char string[MAX_INPUT_SIZE] = "d ";

  strcat(string, path);

  if (sendto(sockfd, string, strlen(string)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  }
  if (recvfrom(sockfd, &res, sizeof(&res), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  }
  return res;
}


int tfsMove(char *from, char *to) {
  int res;
  char string[MAX_INPUT_SIZE] = "m ";

  strcat(string, from);
  strcat(string, " ");
  strcat(string, to);

  if (sendto(sockfd, string, strlen(string)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  }
  if (recvfrom(sockfd, &res, sizeof(res), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  }
  return res;
}


int tfsLookup(char *path) {
  int res;
  char string[MAX_INPUT_SIZE] = "l ";

  strcat(string, path);

  if (sendto(sockfd, string, strlen(string)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  }
  if (recvfrom(sockfd, &res, sizeof(&res), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  }
  return res;
}


int tfsPrint(char *filename){
  int res;
  char string[MAX_INPUT_SIZE] = "p ";

  strcat(string, filename);

  if (sendto(sockfd, string, strlen(string)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  }
  if (recvfrom(sockfd, &res, sizeof(res), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  }
  return res;
}


int tfsMount(char * sockPath) {
  char *serverName;
  /* 
  Adding pid to name of socket to make sure
  every client has a different socket
  */
  char pid[12];
  sprintf(pid, "%d", getpid());
  strcat(client_socket, pid);

  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0) ) < 0) {
    perror("client: can't open socket");
    exit(EXIT_FAILURE);
  }

  unlink(client_socket);
  clilen = setSockAddrUn (client_socket, &client_addr);
  if (bind(sockfd, (struct sockaddr *) &client_addr, clilen) < 0) {
    perror("client: bind error");
    exit(EXIT_FAILURE);
  }

  serverName = sockPath;
  servlen = setSockAddrUn(serverName, &serv_addr);

  return 0;
}


int tfsUnmount() {
  close(sockfd);
  unlink(client_socket);
  return 0;
}