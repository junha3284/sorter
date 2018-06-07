#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sorter.h"
#include "network.h"

int main(){
    int i = Sorter_startSorting();
    printf("test_sorter %d\n", i);
    sleep(1);
    int n;
    int *arr = Sorter_getArrayData(&n);
    for(int i=0; i < n; i ++)
        printf("%d ",arr[i]);
    printf("\n");
    printf("sorted num: %lld\n", Sorter_getNumberArraysSorted());
    Sorter_setArraySize(200);    
    printf("ArraySize for now: %d\n", Sorter_getArrayLength());  

    sleep(1);
    arr = NULL;
    free(arr);

    arr = Sorter_getArrayData(&n);
    for(int i=0; i < n; i ++)
        printf("%d ",arr[i]);
    printf("\n");
    printf("sorted num: %lld\n", Sorter_getNumberArraysSorted());
    printf("ArraySize for now: %d\n", Sorter_getArrayLength());  
    
    free(arr);
    arr = NULL;

    Sorter_stopSorting();
    return 0;
}
