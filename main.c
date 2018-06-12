#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "network.h"
#include "userinterface.h"
#include "sorter.h"

int main(){

    bool flag = true;
    int i = Network_start () || Sorter_start () || UI_start ();
    if (i == 0) {
        printf ("success!\n");

        long seconds = 0;
        long nanoseconds = 500;
        struct timespec reqDelay = {seconds, nanoseconds};

        while (flag){
            CommandType currentType;
            int num;
            Network_checkCommand (&currentType, &num);
            switch (currentType) {
                case Stop:
                    flag = false;
                    break;
                case Count:
                {
                    long long i = Sorter_getNumberArraysSorted ();
                    Network_sendRequestedData (currentType, NULL, 0, &i);
                    break;
                }
                case GetNum:
                {
                    int length;
                    int *arr = Sorter_getArrayData (&length);
                    if (num <= 0 || num > length){
                        Network_sendRequestedData (currentType, NULL, length, NULL);
                        arr = NULL;
                        break;
                    }
                    Network_sendRequestedData (currentType, arr, length, NULL);
                    arr = NULL;
                    break;
                }
                case GetLength:
                {
                    int arr = Sorter_getArrayLength ();
                    Network_sendRequestedData (currentType, &arr, 0, NULL);
                    break;
                }
                case GetArray:
                {
                    int length;
                    int *arr = Sorter_getArrayData (&length);
                    Network_sendRequestedData (currentType, arr, length, NULL);
                    arr = NULL;
                    break;
                }
                default:
                    nanosleep (&reqDelay, (struct timespec *) NULL);
                    break;
            }
        }
    }
          
    else
        printf ("fail!\n");

    UI_end ();
    Sorter_end ();
    Network_end ();
    return 0;
}
