#ifndef __ALRM_LOGIC_H__
#define __ALRM_LOGIC_H__

#include "main.h"

#include "alrm.h"
#include "press_detection.h"
#include "Leds.h"

#define HANDLE_FREQ 100 // Frequency of periodical interrupt handle
#define DEBOUNCE_TIME 0.01 // Debounce time in seconds
#define LED_BLINK_FREQ 5 // Frequency of led blinking
#define LED_BLINK_TIME 10 // Time during which the led will blink (seconds)



// States of alarm (refer to state machine)
/*
IDLE - time is ticking, nothing happens
RING - alarm is ringing, time is still ticking
SET_TIME - time set mode, use keyboard
ALARM_TIME - same, but setting alarm time
*/
typedef enum {IDLE, RING, SET_TIME, SET_ALARM}ALARM_STATE;

// Struct to store
typedef struct{
		ALARM_STATE alarm_state;
		ALARM_STATE prev_alarm_state;
		uint8_t ring_flag;
		uint8_t time_set_flag;
		uint8_t alarm_set_flag;
	
		uint8_t time_set_hours; 
		uint8_t time_set_minutes;
		uint8_t alarm_set_hours; 
		uint8_t alarm_set_minutes;
	
		uint8_t hours_scroll_flag;
		uint8_t minutes_scroll_flag;
}alarm_t;


void alarm_init_components();
void alarm_periph_handler();
void alarm_state_machine();


extern alarm_t* alarm_p; // Pointer to alarm data structure


#endif // _ALRM_LOGIC_H__