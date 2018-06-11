#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "sorter.h"

#define DEFAULT_ARRAY_SIZE 100

static int currentArraySize;
static int nextArraySize;
static int *currentArray;

static long long numSortedArray;

static bool running;
static bool arrayCopyRequested;

static pthread_t sorterThread;

// for now, sizeLock is not necessary. But, I put this because
// it is critical section
static pthread_mutex_t currentArrayLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t arraySizeLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t numSortedArrayLock = PTHREAD_MUTEX_INITIALIZER;

// Condition for requesting hardcopy
static pthread_cond_t arrayCopyRequestedCond = PTHREAD_COND_INITIALIZER;

// Function Declarations
static void* sortLoop(void*);
static int* createPermutation (int length);

// Begin/end the background thread which sorts random permutations.
// return 0 for success
// return an error number for error
int Sorter_start (void){
    nextArraySize = DEFAULT_ARRAY_SIZE;
    numSortedArray = 0;
    running = true;
    arrayCopyRequested = false;
    int threadCreateResult = pthread_create( &sorterThread, NULL, sortLoop, NULL);
    return threadCreateResult;
}

static void* sortLoop(void* empty){
   
    int temp; 
    while (running){

        pthread_mutex_lock (&arraySizeLock);
        {
            currentArraySize = nextArraySize;
        }
        pthread_mutex_unlock (&arraySizeLock);

        pthread_mutex_lock (&currentArrayLock);
        {
            currentArray = createPermutation(currentArraySize);
        }
        pthread_mutex_unlock (&currentArrayLock);
        
        for (int i = 0; i < currentArraySize-1; i ++){
            for (int j=0; j < currentArraySize - (i+1); j++){
                pthread_mutex_lock (&currentArrayLock);
                {
                    if (arrayCopyRequested)
                        pthread_cond_wait (&arrayCopyRequestedCond, &currentArrayLock);
         
                    if (currentArray[j] > currentArray[j+1]){
                        temp = currentArray[j];
                        currentArray[j] = currentArray[j+1];
                        currentArray[j+1] = temp;
                    }
                }
                pthread_mutex_unlock (&currentArrayLock);
            }     
        }

        pthread_mutex_lock (&numSortedArrayLock);
        {
            numSortedArray++;
        }
        pthread_mutex_unlock (&numSortedArrayLock);
        free (currentArray);
        currentArray = NULL;
    }
    return NULL;
}

static int* createPermutation (int length){
    int *arr = malloc(length*sizeof(int));
    for (int i = 0; i < length; i ++)
        arr[i] = i+1;

    int index;
    int temp;

    srand(time(NULL));
    for (int i = 0; i < length; i ++){
        index = rand() % (i+1);
        temp = arr[index];
        arr[index] = arr[i];
        arr[i] = temp;
    }
    return arr;
}

void Sorter_end (void){
    running = false;
    pthread_join(sorterThread, NULL); 
    free(currentArray);
    currentArray = NULL;
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

    pthread_mutex_lock(&currentArrayLock);
    {    
        // for the case when sorter_getArrayData gets called even before the first random permutation array arise
        if (currentArray == NULL){
            for (int i = 0 ; i < temp_currentArrSize; i++)
                temp_arr[i] = i;
        }
        else {
            for (int i = 0; i < temp_currentArrSize; i++)
                temp_arr[i] = currentArray[i];
        }
    }
    pthread_mutex_unlock(&currentArrayLock); 

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


