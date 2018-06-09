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
#define MAPPING_LENGTH 5

typedef struct {
    struct sockaddr_storage senderAddress;
    unsigned int addressSize;
    CommandType type;
} Command;

const static struct {
    const char *str;
    CommandType type;
} mapping_string_command[] = {
    {"Count", Count},
    {"Get", Get},
    {"GetLength", GetLength},
    {"Stop", Stop},
    {"Help", Help}
};

static struct sockaddr_in my_addr; 
static int sockfd;

static Command currentCommand;

static void* recvLoop(void*);
static CommandType stringToCommandMap(const char *string);
static int replyToSender(char *reply);
//static int checkUserInput (char *input, int len);

static pthread_t recvThread;
//static pthread_mutex_t currentCommandLock = PTHREAD_MUTEX_INITIALIZER;

static bool running;


int Network_start (void)
{
    currentCommand.type = NoCommand;
    running = true;
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));

    bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    int threadCreateResult = pthread_create(&recvThread, NULL, recvLoop, NULL);
    return threadCreateResult;
}

void Network_end()
{
    pthread_join(recvThread, NULL);
}

static void* recvLoop (void* empty)
{
    while(running){
        char Message[MSG_MAX_LEN];
        printf("startReceiving\n");
        int bytesRx = recvfrom(sockfd,
                Message,                    // Buffer for message input
                MSG_MAX_LEN,                // Size of Message Buffer
                0,                          // Flags
                (struct sockaddr*) &(currentCommand.senderAddress), // struct sockaddr* from
                &(currentCommand.addressSize));// fromlen

        // to prevent out-of-bounds access
        Message[MSG_MAX_LEN-1] = '\0';

        // to cut out 'enter' character from user input
        Message[bytesRx-1] = '\0';

        char *token = strtok(Message, " ");

        if (bytesRx > 0){
            switch (stringToCommandMap(token)) {
                case Stop :
                    running = false;
                    int i = replyToSender("the program got stopped");
                    if( i <=0 ){
                        running = true;
                        printf("error happend while replying, %d, %d\n", i, currentCommand.addressSize);
                    }
                    break;

                case Invalid :
                    printf("Invalid command is received!\n");
                    break;

                default :
                    printf("unknown error while mapping command!\n");
            }
        }
        else
            printf("error for recvfrom\n");
    }
    return NULL;
}

static int replyToSender (char *reply)
{
    int bytesRx = sendto(sockfd,
            reply,                          // message the program want to send
            strlen(reply)+1,                  // size of message
            0,                              // Flags
            (struct sockaddr*) &(currentCommand.senderAddress),   // struct sockaddr* from
            currentCommand.addressSize);    // fromlen
    return bytesRx;
}

static CommandType stringToCommandMap(const char *string)
{
    for (int i = 0; i < MAPPING_LENGTH; i++){
        if (strcmp(mapping_string_command[i].str, string) == 0)
            return mapping_string_command[i].type;
    }
    return Invalid;
}
