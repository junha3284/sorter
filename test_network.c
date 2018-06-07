#include <stdio.h>
#include "network.h"

int main(){
    printf("test is working\n");
    int i = Network_start();
    if ( i == 0)
        printf("success!\n");
    else
        printf("fail!\n");
    Network_end();
    return 0;
}
