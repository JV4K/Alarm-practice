#ifndef __ALRM_LOGIC_H__
#define __ALRM_LOGIC_H__

#include "main.h"
#include "alrm.h"

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
		ALARM_STATE state;
	
}alarm_logic_c;

#endif // _ALRM_LOGIC_H__