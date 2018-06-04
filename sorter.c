#include <stdio.h>
#include <stdboolh>
#include <stdlib.h>
#include <time.h>
#include "sorter.h"

static int currentArraySize;
static int nextArraySize;
static int *currentArray;

static long long numSortedArray;

static bool running;
static bool requested;

static pthread_t sorterThread;

// for now, sizeLock is not necessary. But, I put this because
// it is critical section
pthread_mutex_t arrayLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t arraySizeLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numSortedArrayLock = PTHREAD_MUTEX_INITIALIZER;

// Condition for requesting hardcopy
pthread_cond_t arrayCopyRequestedCond = PTHREAD_COND_INITIALIZER;


// Begin/end the background thread which sorts random permutations.
void Sorter_startSorting (void){

}

static void sortLoop(){
    int *arr;
    while (running){
        pthread_mutex_lock (
        currentArray = createPermutation(currentArraySize);

}

static int* createPermutation (int length){
    int *arr = malloc(length*sizeof(int));
    for (int i = 0; i < length; i ++)
        arr[i] = i+1;

    int index;
    int temp;

    for (int i = 0; i < length; i ++){
        index = srand(time(0)) % i;
        temp = arr[index];
        arr[index] = arr[i];
        arr[i] = temp;
    }
    return arr;
}

void Sorter_stopSorting (void){

}

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
    pthread_mutex_lock(&arraySizeLock);
    {
        temp_currentArrSize = currentArraySize;
    }
    pthread_mutex_unlock(&arraySizeLock);

    *length = temp_currentArrSize;

    int *temp_arr = malloc(temp_currentArrSize*sizeof(int));

    pthread_mutex_lock(&arrayLock);
    {    
        // for the case when sorter_getArrayData gets called even before the first random permutation array arise
        if (currentArray == NULL){
            for (int i = 0 ; i < length; i++)
                temp_arr[i] = i;
        }
        else {
            for (int i = 0; i < length; i++)
                temp_arr[i] = currentArray[i];
        }
    }
    pthread_mutex_unlock(&arrayLock); 

    arrayCopyRequested = false;
    pthread_cond_signal(&arrayCopyRequestedCond);
    return temp_arr;
} 

// Get the number of arrays which have finished being sorted.
long long Sorter_getNumberArraysSorted (void){
    long long temp;
    pthread_mutex_lock(&numSortedArrayLock);
    {
        temp = numSortedArray;
    }
    pthread_mutex_unlock(&numSortedArrayLock);
    return temp;
}


