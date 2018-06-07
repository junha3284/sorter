#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "network.h"

#define COMMAND_BUFFER_SIZE 10
#define MSG_MAX_LEN 64 

typedef struct {
    struct sockaddr_sotrage,
    int addressSize,
    commandType type
} command

struct addrinfo hints, *res;
int sockfd;

int Network_start (void){
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddinfo(NULL, "12345", &hints, &res);
    
}
