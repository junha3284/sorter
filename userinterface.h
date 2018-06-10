// userinterface.h
// Module to support user interface such as
// showing the data which indicates
//      how many arrays sorted during previous 1 second (14 segement display)
// controlling the size of arrays which will be sorted with bubble sort (potentiometer)

// Begin the background thread which will
//      read the potentiometer and change the array size
//      display the number of sorted array during previous 1 second                              
#ifndef _USERINTERFACE_H_
#define _USERINTERFACE_H_

// return 0 for success
// return an error number for error
int UI_start (void);

// End the background thread
void UI_end (void);

#endif
