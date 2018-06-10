#include <stdio.h>
#include <fcntl.h> 
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "userinterface.h"
#include "sorter.h"

#define POT_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_PWL_INTERVAL 500

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_LEFT_DIGIT_NUM 64
#define GPIO_RIGHT_DIGIT_NUM 44
#define GPIO_61_DIRECTION "/sys/class/gpio/gpio61/direction"
#define GPIO_44_DIRECTION "/sys/class/gpio/gpio44/direction"
#define GPIO_61_VALUE "/sys/class/gpio/gpio61/value"
#define GPIO_44_VALUE "/sys/class/gpio/gpio44/value"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

#define REG_0_OUTA 0xA1
#define REG_0_OUTB 0x86
#define REG_1_OUTA 0x04
#define REG_1_OUTB 0x20
#define REG_2_OUTA 0x31
#define REG_2_OUTB 0x0E
#define REG_3_OUTA 0xB0
#define REG_3_OUTB 0x0E
#define REG_4_OUTA 0x14
#define REG_4_OUTB 0xA8
#define REG_5_OUTA 0xB0
#define REG_5_OUTB 0x8C
#define REG_6_OUTA 0xB1
#define REG_6_OUTB 0x8C
#define REG_7_OUTA 0x80
#define REG_7_OUTB 0x06
#define REG_8_OUTA 0xB1
#define REG_8_OUTB 0x8E
#define REG_9_OUTA 0x90
#define REG_9_OUTB 0x8E


static pthread_t updatingThread;
static bool running;
static int totalNumSortedArray;
static int i2cFileDesc;

// (index of array * 500) is corresponding A2D Reading
// the value in that index is array size
static const int dataPoints[10] = {1, 20, 60, 120, 250, 300, 500, 800, 1200, 5700};

// Update the array size every 1 seconds depending on POT
//        the 14 segments displays to indicate how many arrays got sorted during last 1 second 
static void* update (void*);

static int getVoltage0Reading(void);
static int GPIO_export(int gpio_num);
static int initI2cBus(char* bus, int address);
static int writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);

int UI_start (void)
{
    if ( GPIO_export(GPIO_LEFT_DIGIT_NUM) || GPIO_export(GPIO_RIGHT_DIGIT_NUM) ) 
        return 1;

    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    if (i2cFileDesc == -1)
        return 1;

    if ( writeI2cReg(i2cFileDesc, REG_DIRA, 0x00) || writeI2cReg(i2cFileDesc, REG_DIRB, 0x00) )
        return 1;

    running = true;
    totalNumSortedArray = 0;
    int threadCreateResult = pthread_create(&updatingThread, NULL, update, NULL); 
    return threadCreateResult;
}
static void* update (void* empty)
{
    while (running){
        float voltageForNow = ((float) getVoltage0Reading()) / A2D_PWL_INTERVAL; 
        
        int index = voltageForNow;
        float index_float_point = voltageForNow - index;

        int size = dataPoints[index]
            +  (index_float_point * (dataPoints[index+1] - dataPoints[index]));

        Sorter_setArraySize (size);
        sleep(1);
    }
    return NULL;
}

void UI_end (void)
{
   running = false;
   pthread_join(updatingThread, NULL); 
}

static int GPIO_export(int gpio_num)
{
    FILE *export = fopen(GPIO_EXPORT, "w");

    if (export == NULL)
        return 1; 

    int charWritten = fprintf(export, "%d", 44);
    if (charWritten <= 0)
        return 1;

    fclose(export);
    return 0; 
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
    
static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		return -1;
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		return -1;
	}
	return i2cFileDesc;
}

static int writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		return 1;
	}
    return 0;
}
