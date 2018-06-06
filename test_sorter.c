#include <stdio.h>
#include <unistd.h>
#include "sorter.h"

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
    sleep(1);
    Sorter_setArraySize(200);    
    sleep(1);
    arr = Sorter_getArrayData(&n);
    for(int i=0; i < n; i ++)
        printf("%d ",arr[i]);
    printf("\n");
    printf("sorted num: %lld\n", Sorter_getNumberArraysSorted());

    Sorter_stopSorting();
    return 0;
}
