#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "network.h"

#define MSG_MAX_LEN 64 
#define MYPORT 12345

static pthread_t recvThread;
//static pthread_mutex_t currentCommandLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    struct sockaddr *senderAddress;
    unsigned int addressSize;
    CommandType type;
} Command;

const static struct {
    const char *str;
    CommandType type;
} mapping_string_Command[] = {
    {"get


static struct sockaddr_in my_addr; 
static int sockfd;

static Command currentCommand;

static void* recvLoop(void*);
static int checkUserInput (char *input, int len);

int Network_start (void)
{
    currentCommand.type = NoCommand;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));

    bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    int threadCreateResult = pthread_create(&recvThread, NULL, recvLoop, NULL);
    return threadCreateResult;
}

static void* recvLoop (void* empty)
{
    while(1){
        char Message[MSG_MAX_LEN];
        printf("startReceiving\n");
        int bytesRx = recvfrom(sockfd,
                Message,                    // Buffer for message input
                MSG_MAX_LEN,                // Size of Message Buffer
                0,                          // Flags
                currentCommand.senderAddress, // struct sockaddr* from
                &currentCommand.addressSize);// fromlen

        // to prevent out-of-bounds access
        Message[MSG_MAX_LEN-1] = '\0';
        // to cut out 'enter' character from user input
        Message[bytesRx-1] = '\0';

        if (bytesRx > 0){
            switch (Message) {
                case "get num" :
                    printf("get num is received!\n");
                    break;
                default :
                    printf("unknown command is received!\n");
            }
        }
        else
            printf("error for recvfrom\n");
    }
    return NULL;
}

void Network_end()
{
    pthread_join(recvThread, NULL);
}
