#include "alrm_logic.h"

alarm_t* alarm_p; // Pointer to alarm data structure

void alarm_init_components(){
	hbut0 = buttonInit(DEBOUNCE_TIME, HANDLE_FREQ);
	hbut1 = buttonInit(DEBOUNCE_TIME, HANDLE_FREQ);
	led_init(HANDLE_FREQ, LED_BLINK_FREQ, LED_BLINK_TIME);
}

void alarm_periph_handler(){
	// Polling and debouncing of buttons (integrator + trigger)
		But0_State = getDebouncedButton(hbut0, !HAL_GPIO_ReadPin(BUT0_GPIO_Port, BUT0_Pin));
		But1_State = getDebouncedButton(hbut1, !HAL_GPIO_ReadPin(BUT1_GPIO_Port, BUT1_Pin));
		
		// Process keystrokes (long/short)
		PressDetection_Update();
	
		But0_prev_press_state = But0_press_state;
		But1_prev_press_state = But1_press_state;
		But0_press_state = Get_Button0_PressState();
		But1_press_state = Get_Button1_PressState();
		
		// Update time from HW RTC
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD); // RTC_FORMAT_BIN , RTC_FORMAT_BCD
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

void alarm_state_machine(){
	switch (alarm_p->alarm_state){
		case IDLE:{
			if (alarm_A_flag){
				alarm_A_flag = 0;
				alarm_p->alarm_state = RING;
			} else {
					if(But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state != BUTTON_LONG_PRESS 
					&& But1_press_state != BUTTON_LONG_PRESS){
					alarm_p->alarm_state = SET_TIME;
					} else if (But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state != BUTTON_LONG_PRESS){
						alarm_p->alarm_state = SET_ALARM;
					}
			}
			break;
		}
		
		case RING:{
			if(!alarm_p->ring_flag){
				alarm_p->ring_flag = 1;
				leds_blink_start();
			} else{}

			if(But0_press_state == BUTTON_SHORT_PRESS || But1_press_state == BUTTON_SHORT_PRESS || 
					(But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state != BUTTON_LONG_PRESS) || 
					(But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state != BUTTON_LONG_PRESS)){
				alarm_p->alarm_state = IDLE;
				leds_blink_stop();
			} else if (alarm_led->led_cur_state == DIM){
				alarm_p->alarm_state = IDLE;
			}
			break;
		}
		
		case SET_TIME:{
			
			break;
		}
		
		case SET_ALARM:{
			
			break;
		}
			
	}

}