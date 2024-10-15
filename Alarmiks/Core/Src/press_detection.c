#include "press_detection.h"

// Debounced button states
uint8_t But0_State, But1_State;

// Button handlers (button.h)
Button_Handle_t *hbut1, *hbut0;

// Button press state (idle/short/long)
ButtonPressState But0_press_state, But1_press_state;

// Internal states to track timing and conditions
static uint8_t But0_PressCounter = 0;
static uint8_t But1_PressCounter = 0;
static ButtonPressState But0_PressState = BUTTON_IDLE;
static ButtonPressState But1_PressState = BUTTON_IDLE;

// Function to call in interrupt handler, 100 Hz
void PressDetection_Update(void) {
    // Button 0
    if (But0_State == 1) {  // Button is pressed
        But0_PressCounter++;
        if (But0_PressCounter >= LONG_PRESS_THRESHOLD) {
            But0_PressState = BUTTON_LONG_PRESS;
        }
    } else {  // Button is released
        if (But0_PressCounter > 0 && But0_PressCounter < LONG_PRESS_THRESHOLD) {
            But0_PressState = BUTTON_SHORT_PRESS;
        }
        But0_PressCounter = 0;  // Reset counter after release
    }

    // Button 1
    if (But1_State == 1) {  // Button is pressed
        But1_PressCounter++;
        if (But1_PressCounter >= LONG_PRESS_THRESHOLD) {
            But1_PressState = BUTTON_LONG_PRESS;
        }
    } else {  // Button is released
        if (But1_PressCounter > 0 && But1_PressCounter < LONG_PRESS_THRESHOLD) {
            But1_PressState = BUTTON_SHORT_PRESS;
        }
        But1_PressCounter = 0;  // Reset counter after release
    }
}

// Function to check the state of Button 0
ButtonPressState Get_Button0_PressState(void) {
    ButtonPressState state = But0_PressState;
    // Reset the state after it's read
    But0_PressState = BUTTON_IDLE;
    return state;
}

// Function to check the state of Button 1
ButtonPressState Get_Button1_PressState(void) {
    ButtonPressState state = But1_PressState;
    // Reset the state after it's read
    But1_PressState = BUTTON_IDLE;
    return state;
}