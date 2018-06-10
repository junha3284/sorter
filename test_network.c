#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "network.h"

int main(){
    printf("test is working\n");
    bool flag = true;
    int i = Network_start();
    if (i == 0){
        printf("success!\n");
        while(flag){
            CommandType currentType;
            int num;
            Network_checkCommand(&currentType, &num);
            switch(currentType){
                case Stop:
                    flag = false;
                    break;
                case Count:
                {
                    long long i = 123;
                    Network_sendRequestedData (currentType, NULL, 0, &i);
                    break;
                }
                case GetNum:
                {
                    // int arr[3] = 123;
                    // Network_sendRequestedData (currentType, arr, 3, NULL);
                    Network_sendRequestedData (currentType, NULL, 3, NULL);
                }
                case GetLength:
                {
                    int arr[3] = {1,2,3};
                    Network_sendRequestedData (currentType, arr, 3, NULL);
                    //Network_sendRequestedData (currentType, NULL, 3, NULL);
                }
                case GetArray:
                {
                    int arr[3] = {1,2,3};
                    Network_sendRequestedData (currentType, arr, 3, NULL);
                }
                default:
                {
                    long seconds = 0;
                    long nanoseconds = 500;
                    struct timespec reqDelay = {seconds, nanoseconds};
                    nanosleep(&reqDelay, (struct timespec *) NULL);
                    break;
                }
            }
        }
    }
          
    else
        printf("fail!\n");
    Network_end();
    return 0;
}
