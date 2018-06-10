// network.h
// Module to support UDP network for accepting commands and replying it 
// It spawn a background thread which keep listening UDP message.
// It provides supports replying to the received message 

#ifndef _NETWORK_H_
#define _NETWORK_H_

typedef enum{
    NoCommand = 0,
    Count = 1,
    Get = 2,
    GetNum = 3,
    GetArray = 4,
    GetLength = 5,
    Stop = 6,
    Help = 7,
    Invalid = 8,
} CommandType; 

// Begin the background thread which listens user commands
// return 0 for success
// return an error number for error
int Network_start (void);

// End the background thread which listens to user commands
void Network_end (void);

// Get the command which is waiting for being processed
// set num to be # for command 'get #' if current command is 'get #'
// @commandType type possible returned value
//     NoCommand
//     Count
//     GetNum
//     GetArray 
//     GetLength
//     Stop
void Network_checkCommand (CommandType *type, int *num);

// send a requested data to the client who gave command
// type: GetNum
//      If argument data set NULL, send the client the error message as following 
//           "the requested size is out of range (current size: 'current_array_length')"
//      otherwise, send the requested #_th element to the client
// type: GetArray
//      If argument data is NULL, send the client the error message as following
//          "Inner error happend for command: get array" 
//      otherwise, send the current array with one or multiple packets
// type: Count
//      If argument pCount is NULL, send the client the error message as following
//          "Inner error happend for command: count"
//      otherwise, send the number of arrays which has been sorted until now 
// type: GetLength
//      If argument data is NULL, send the client the error message as following
//          "Inner error happend for command: get length" 
//      otherwise, send data[0] as length of the current array
// Return the number of characters sent for success.
// Return -1 for any error 
int Network_sendRequestedData (CommandType type, int *data, int dataLength, const long long *pCount);

#endif
