#include <stdio.h>
#include <stdboolh>
#include "sorter.h"

static int currentArraySize;
static int nextArraySize;
static int numberArraySorted;
static int *currentArray;

static bool running;
static bool requested;

// for now, sizeLock is not necessary. But, I put this because
// it is critical section
pthread_mutex_t arrayLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t arraySizeLock = PTHREAD_MUTEX_INITIALIZER;

// Condition for requesting hardcopy
pthread_cond_t arrayCopyRequestedCond = PTHREAD_COND_INITIALIZER;

// Set the size the next array to sort (don't change current array)
void Sorter_setArraySize (int newSize){
    pthread_mutex_lock(&arraySizeLock);
    nextArraySize = newSize;
    pthread_mutex_unlock(&arraySizeLock);
}

// Get the size of the array currently being sorted.
int Sorter_getArrayLength (void){
    pthread_mutex_lock(&arraySizeLock);
    int temp = currentArraySize;
    pthread_mutex_unlock(&arraySizeLock);
    return temp;
}

// Get a copy of the current (potentially partially sorted) array.
// Returns a newly allocated array and sets 'length' to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
int* Sorter_getArrayData (int *length){

    arrayCopyRequested = true;

    int temp_currentArrSize;
    pthread_mutex_lock(&sizeLock);
    {
        temp_currentArrSize = currentArraySize;
    }
    pthread_mutex_unlock(&sizeLock);

    *length = temp_currentArrSize;

    int *temp_arr = malloc(temp_currentArrSize*sizeof(int));

    pthread_mutex_lock(&arraySizeLock);
    {    
        for (int i = 0; i < length; i++)
            temp_arr[i] = currentArray[i];
    }
    pthread_mutex_unlock(&arraySizeLock); 
    arrayCopyRequested = false;
    pthread_cond_signal(&arrayCopyRequestedCond);
    return temp_arr;
} 
