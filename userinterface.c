#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "userinterface.h"
#include "sorter.h"

#define POT_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

static pthread_t updatingThread;
static bool running;
static int totalNumSortedArray;

// Update the array size every 1 seconds depending on POT
//        the 14 segments displays to indicate how many arrays got sorted during last 1 second 
static void* update (void*);
static int getVoltage0Reading();

int UI_start (void)
{
    running = true;
    totalNumSortedArray = 0;
    int threadCreateResult = pthread_create(&updatingThread, NULL, update, NULL); 
    return threadCreateResult;
}

static void* update (void* empty)
{
    while (running){
        int voltageForNow = getVoltage0Reading(); 
        Sorter_setArraySize (voltageForNow);
        sleep(1);
    }
    return NULL;
}

void UI_end (void)
{
   running = false;
   pthread_join(updatingThread, NULL); 
}

static int getVoltage0Reading()
{
    // Open file
    FILE *f = fopen(POT_FILE_VOLTAGE0, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf("try:    echo BB-ADC > /sys/devices/platform/bone_capemgr/slots\n");
        return -1; 
    }

    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0){
        printf("ERROR: Unable to read values from voltage input file.\n");
        return -1;
    }

    // Close file
    fclose(f);

    return a2dReading;
}
    

