#ifndef PRESS_DETECTION_H
#define PRESS_DETECTION_H

#include "stdint.h"
#include "button.h"
extern uint8_t But0_State, But1_State;
extern Button_Handle_t *hbut1, *hbut0;

// Define thresholds (in units of 100ms intervals)
#define LONG_PRESS_THRESHOLD 30  // 3 seconds for long press

// Enum to describe button press states
typedef enum {
    BUTTON_IDLE,
    BUTTON_SHORT_PRESS,
    BUTTON_LONG_PRESS
} ButtonPressState;

// Function to call in the interrupt handler
void PressDetection_Update(void);

// Functions to check for short/long press
ButtonPressState Get_Button0_PressState(void);
ButtonPressState Get_Button1_PressState(void);

extern ButtonPressState But0_press_state, But1_press_state;
extern ButtonPressState But0_prev_press_state, But1_prev_press_state;

#endif // PRESS_DETECTION_H
