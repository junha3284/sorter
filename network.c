#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "network.h"

#define RECEIVING_MSG_MAX_LEN 64 
#define REPLYING_MSG_MAX_LEN 1024
#define MYPORT 12345
#define MAPPING_LENGTH 5

typedef struct {
    struct sockaddr_storage senderAddress;
    unsigned int addressSize;
    CommandType type;
    int requestedNum;
} Command;

const static struct {
    const char *str;
    CommandType type;
} mapping_string_command[] = {
    {"count", Count},
    {"get", Get},
    {"stop", Stop},
    {"help", Help}
};

static struct sockaddr_in my_addr; 
static int sockfd;

static Command currentCommand;

static void* recvLoop(void*);
static CommandType stringToCommandMap(const char *string);
static int replyToSender(char *reply);
//static int checkUserInput (char *input, int len);

static pthread_t recvThread;
static pthread_mutex_t currentCommandLock = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t processingCommandCond = PTHREAD_COND_INITIALIZER;

static bool running;


int Network_start (void)
{
    currentCommand.type = NoCommand;
    currentCommand.requestedNum = 0;
    currentCommand.addressSize = sizeof(currentCommand.senderAddress);
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
        char Message[RECEIVING_MSG_MAX_LEN];
        printf("startReceiving\n");
        int bytesRx;
        pthread_mutex_lock (&currentCommandLock);
        {
            bytesRx = recvfrom(sockfd,
                    Message,                    // Buffer for message input
                    RECEIVING_MSG_MAX_LEN,                // Size of Message Buffer
                    0,                          // Flags
                    (struct sockaddr*) &(currentCommand.senderAddress), // struct sockaddr* from
                    &(currentCommand.addressSize));// fromlen
        }
        pthread_mutex_unlock (&currentCommandLock);

        // to prevent out-of-bounds access
        Message[RECEIVING_MSG_MAX_LEN-1] = '\0';

        // to cut out 'enter' character from user input
        Message[bytesRx-1] = '\0';

        char *token = strtok(Message, " ");

        if (bytesRx > 0){
            switch (stringToCommandMap(token)) {
                case Stop :
                    running = false;
                    replyToSender("the program got stopped\n");
                    break;

                case Get :
                    token = strtok(NULL, " ");
                    if (token == NULL){
                        replyToSender("there is no enough argument for Get command\n");
                        break;
                    }
                    // Deal with get length command
                    if (strcmp(token,"length") == 0){
                        pthread_mutex_lock (&currentCommandLock);
                        {
                            currentCommand.type = GetLength; 
                            pthread_cond_wait (&processingCommandCond, &currentCommandLock);
                        }
                        pthread_mutex_unlock (&currentCommandLock);
                    }
                    // Deal with get array command
                    else if (strcmp(token, "array") == 0){
                        pthread_mutex_lock (&currentCommandLock);
                        {
                           currentCommand.type = GetArray; 
                           pthread_cond_wait (&processingCommandCond, &currentCommandLock);
                        }
                        pthread_mutex_unlock (&currentCommandLock);
                    }
                    // Deal with get # command
                    else {
                        char *endptr = NULL;
                        int requestedNum = strtol(token, &endptr, 10);

                        // check whether second argument includes non-numeric value or not
                        if (endptr != NULL){
                            // second argument include non-numeric value
                            if (*endptr != '\0'){
                                replyToSender("Invalid argument for Get command (only numeric value for second argument)\n");
                                break;
                            }
                            // second argument does not include non-numeric value
                            pthread_mutex_lock (&currentCommandLock);
                            {
                                currentCommand.requestedNum = requestedNum; 
                                currentCommand.type = GetNum;
                                pthread_cond_wait (&processingCommandCond, &currentCommandLock);
                            }
                            pthread_mutex_unlock (&currentCommandLock);
                            break;
                        }
                        printf("unknown error while verifying second argument for Get command\n");
                    } 
                    break;

                case Help:
                    replyToSender("Accepted command examples:\n"
                            "\tcount -- display number arrays sorted.\n"
                            "\tget length -- display length of array currently being sorted.\n"
                            "\tget array -- display the full array being sorted.\n"
                            "\tget 10 -- display the tenth element of array currently being sorted.\n"
                            "\tstop -- cause the server program to end.\n");
                    break;
                case Count:
                    pthread_mutex_lock (&currentCommandLock);
                    {
                       currentCommand.type = Count; 
                       pthread_cond_wait (&processingCommandCond, &currentCommandLock);
                    }
                    pthread_mutex_unlock (&currentCommandLock);

                case Invalid:
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

// Get the command which is waiting for being processed
// set num to be # for command 'get #' if current command is get #
// @commandType type possible returned value
//     NoCommand
//     Count
//     GetNum
//     GetArray 
//     GetLength
//     Stop
void checkCommand (CommandType *type, int *num)
{
    pthread_mutex_lock (&currentCommandLock);
    {
        if (currentCommand.type == GetNum)
            *num = currentCommand.requestedNum;
        *type = currentCommand.type;
    }
    pthread_mutex_unlock (&currentCommandLock);
}

int sendRequestedData (CommandType type, int *data, int dataLength, const long long *pCount)
{
    int requestedNum;

    pthread_mutex_lock (&currentCommandLock);
    {
        if (type != currentCommand.type)
            return -2;
        requestedNum = currentCommand.requestedNum;
        currentCommand.type = NoCommand;
    }
    pthread_mutex_unlock (&currentCommandLock);

    char Message[REPLYING_MSG_MAX_LEN];

    switch (type){
        case GetArray:
           if (data == NULL){
               replyToSender("Inner error happend for command: get array\n"); 
               pthread_cond_signal(&processingCommandCond);
               return -1; 
           }
           return replyToSender("now it is being implemented\n");
           break;

        case GetNum:
           if (data == NULL){
               pthread_cond_signal(&processingCommandCond);
               return replyToSender("the requested size is out of range\n"); 
           }
           snprintf(Message, REPLYING_MSG_MAX_LEN, "Value %d: %d", requestedNum, data[requestedNum-1]);
           break;

        case Count:
           if (pCount == NULL){
               replyToSender("Inner error happend for command: count\n");
               pthread_cond_signal(&processingCommandCond);
               return -1;
           }
           snprintf(Message, REPLYING_MSG_MAX_LEN, "Number of arrays sorted = %lld\n", *pCount);
           break;

        case GetLength:
           if (data == NULL){
               replyToSender("Inner error happend for command: get length\n"); 
               pthread_cond_signal(&processingCommandCond);
               return -1; 
           }
           snprintf(Message, REPLYING_MSG_MAX_LEN, "Current array length = %d", data[0]);
           break;

        default:
           printf("Unkown type error happend while sendingRequestedData\n");
           return -1;
    }
    pthread_cond_signal(&processingCommandCond);
    return replyToSender(Message);
}

static int replyToSender (char *reply)
{
    int bytesRx = sendto(sockfd,
            reply,                          // message the program want to send
            strlen(reply)+1,                  // size of message
            0,                              // Flags
            (struct sockaddr*) &(currentCommand.senderAddress),   // struct sockaddr* from
            currentCommand.addressSize);    // fromlen

    if (bytesRx <= 0){
        running = true;
        printf("error happend while replying\n");
    }
    return bytesRx;
}

static CommandType stringToCommandMap (const char *string)
{
    for (int i = 0; i < MAPPING_LENGTH; i++){
        if (strcmp(mapping_string_command[i].str, string) == 0)
            return mapping_string_command[i].type;
    }
    return Invalid;
}
