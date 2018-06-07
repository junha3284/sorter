// network.h
// Module to support UDP network for accepting commands and replying it 
// It spawn a background thread which keep listening UDP message.
// It provides supports replying to the received message 
#ifndef _NETWORK_H_
#define _NETWORK_H_

typedef enum{
    NoCommand = 0,
    Count = 1,
    GetNum = 2,
    GetArray = 3,
    GetLength = 4,
    Stop = 5,
    Help = 6 
} CommandType; 

// Begin the background thread which listens user commands
// return 0 for success
// return an error number for error
int Network_start (void);

// End the background thread which listens to user commands
void Network_end (void);

// Get the command which is waiting for being processed
// return commandType
// 0 <=> no command in buffer
// 1 <=> 'count'
// 2 <=> 'get #'
// 3 <=> 'get array'
// 4 <=> 'get length'
// 5 <=> 'stop'
// set length to be the length of # for command 'get #' 
CommandType checkCommand (int* length);

// send a requested data to the client who gave command
// type: GetNum and GetArray 
//      If argument data is NULL, send the client the error message as following 
//           "the requested size is out of range (current size: 'current_array_length')"
//      otherwise, send the requested array to the client with one or multiple packet
//      depending on the length of the array 
// type: Count
//      send the count unless it is NULL. If it is null, send the error message as followingh 
//          "Inner error happend for command: count"
// type: Length
//      If argument data is NULL, send the client the error message as following
//          "Inner error happend for command: get length" 
//      otherwise, send data[0] as length of the current array
// Return the number of characters sent for success.
// Return -1 for sending error 
// Return -2 for unsupported command
int sendRequestedData (CommandType type, int *data, const long long *pCount);

#endif
