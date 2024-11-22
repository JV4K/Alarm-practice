#include "alrm_logic.h"
alarm_t halarm;
alarm_t* alarm_p = &halarm; // Pointer to alarm data structure
// uint8_t div10hz;

char time_str[6];

void create_time_string(uint8_t hours_bcd, uint8_t minutes_bcd, char *time_str) {
  // Convert BCD to decimal
  uint8_t hours = ((hours_bcd >> 4) * 10) + (hours_bcd & 0x0F);
  uint8_t minutes = ((minutes_bcd >> 4) * 10) + (minutes_bcd & 0x0F);

  // Format the string
  snprintf(time_str, 6, "%02d:%02d", hours, minutes);
}

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
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD); // Necessary for proper time update
	
		leds_calculation();
}

void alarm_state_machine(){
	switch (alarm_p->alarm_state){
		case IDLE:{
			create_time_string(sTime.Hours, sTime.Minutes, time_str);
			Displ_WString(20, 52, time_str, Font24, 1, GREEN, BLACK);

			if (alarm_A_flag){
				alarm_A_flag = 0;
				alarm_p->alarm_state = RING;
				Displ_CLS(BLACK);
			} else {
					if(But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state != BUTTON_LONG_PRESS 
					&& But1_press_state != BUTTON_LONG_PRESS){
					alarm_p->alarm_state = SET_TIME;
					Displ_CLS(BLACK);
					} else if (But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state != BUTTON_LONG_PRESS){
						alarm_p->alarm_state = SET_ALARM;
						Displ_CLS(BLACK);
					}
			}
			break;
		}
		
		case RING:{
			Displ_WString(15, 30, "ALARM!", Font24, 1, RED, BLACK);
			create_time_string(sTime.Hours, sTime.Minutes, time_str);
			Displ_WString(20, 52, time_str, Font24, 1, GREEN, BLACK);

			if(!alarm_p->ring_flag){
				alarm_p->ring_flag = 1;
				leds_blink_start();                                
			} else{}

			if(But0_press_state == BUTTON_SHORT_PRESS || But1_press_state == BUTTON_SHORT_PRESS || 
					(But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state != BUTTON_LONG_PRESS) || 
					(But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state != BUTTON_LONG_PRESS)){
				alarm_p->alarm_state = IDLE;
				Displ_CLS(BLACK);
				alarm_p->ring_flag = 0;
				leds_blink_stop();
			} else if (alarm_led->led_cur_state == DIM){
				alarm_p->ring_flag = 0;
				alarm_p->alarm_state = IDLE;
				Displ_CLS(BLACK);
			}
			break;
		}
		
		case SET_TIME:{
			if(!alarm_p->time_set_flag){
				alarm_p->time_set_hours = sTime.Hours;
				alarm_p->time_set_minutes = sTime.Minutes;
				alarm_p->time_set_flag = 1;
			}
			
			// First check if we are exiting time set mode
			if (But0_press_state == BUTTON_LONG_PRESS && But1_press_state == BUTTON_LONG_PRESS){
				alarm_p->alarm_state = IDLE;
				Displ_CLS(BLACK);
				alrm_SetTime(alarm_p->time_set_hours, alarm_p->time_set_minutes, 0); // Set time to rtc
				alarm_p->time_set_flag = 0;
				alarm_p->hours_scroll_flag = 0;
				alarm_p->minutes_scroll_flag = 0;
				alarm_p->hours_scroll_timeout_cnt = 0;
				alarm_p->minutes_scroll_timeout_cnt = 0;
				alarm_p->hours_scroll_divider_cnt = 0;
				alarm_p->minutes_scroll_divider_cnt = 0;
				return; // Skip the rest
			}

			Displ_WString(10, 10, "Set time:", Font16, 1, YELLOW, BLACK);
			create_time_string(alarm_p->time_set_hours, alarm_p->time_set_minutes, time_str);
			Displ_WString(20, 52, time_str, Font24, 1, GREEN, BLACK);
			
			// Increment hours on short press of button0
			if (But0_press_state == BUTTON_SHORT_PRESS){
				alarm_p->time_set_hours = increment_bcd(alarm_p->time_set_hours, 0x23);
			}
			
			// Increment minutes on short press of button1
			if (But1_press_state == BUTTON_SHORT_PRESS){
				alarm_p->time_set_minutes = increment_bcd(alarm_p->time_set_minutes, 0x59);
			}
			
			// Increment hours quickly on long press of button0
			if(But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state != BUTTON_LONG_PRESS){
				alarm_p->hours_scroll_flag = 1;
			} else if (But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state == BUTTON_LONG_PRESS && 
									alarm_p->hours_scroll_flag){
				
				if (alarm_p->hours_scroll_timeout_cnt < BUTTON_SCROLL_TIMEOUT){
					alarm_p->hours_scroll_timeout_cnt++;
				} else {
					alarm_p->hours_scroll_divider_cnt++;
					if (alarm_p->hours_scroll_divider_cnt >= BUTTON_SCROLL_FREQ_DIV){
						alarm_p->time_set_hours = increment_bcd(alarm_p->time_set_hours, 0x23);
						alarm_p->hours_scroll_divider_cnt = 0;
					}
				}
				
			} else if (But0_press_state == BUTTON_IDLE){
				alarm_p->hours_scroll_flag = 0;
				alarm_p->hours_scroll_timeout_cnt = 0;
				alarm_p->hours_scroll_divider_cnt = 0;
			}
			
			// Increment minutes quickly on long press of button1
			if(But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state != BUTTON_LONG_PRESS){
				alarm_p->minutes_scroll_flag = 1;
			} else if (But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state == BUTTON_LONG_PRESS && 
									alarm_p->minutes_scroll_flag){
										
				if (alarm_p->minutes_scroll_timeout_cnt < BUTTON_SCROLL_TIMEOUT){
					alarm_p->minutes_scroll_timeout_cnt++;
				} else {
					alarm_p->minutes_scroll_divider_cnt++;
					if (alarm_p->minutes_scroll_divider_cnt >= BUTTON_SCROLL_FREQ_DIV){
						alarm_p->time_set_minutes = increment_bcd(alarm_p->time_set_minutes, 0x59);
						alarm_p->minutes_scroll_divider_cnt = 0;
					}
				}
				
			} else if (But1_press_state == BUTTON_IDLE){
				alarm_p->minutes_scroll_flag = 0;
				alarm_p->minutes_scroll_timeout_cnt = 0;
				alarm_p->minutes_scroll_divider_cnt = 0;
			}
			
			break;
		}
		
		case SET_ALARM:{
			
//				if(!alarm_p->alarm_set_flag){
//				alarm_p->alarm_set_hours = sTime.Hours;
//				alarm_p->alarm_set_minutes = sTime.Minutes;
//				alarm_p->alarm_set_flag = 1;
//			}
			
			// First check if we are exiting alarm set mode
			if (But0_press_state == BUTTON_LONG_PRESS && But1_press_state == BUTTON_LONG_PRESS){
				alarm_p->alarm_state = IDLE;
				Displ_CLS(BLACK);
				alrm_SetAlarm(alarm_p->alarm_set_hours, alarm_p->alarm_set_minutes, 0); // Set time to rtc
//				alarm_p->alarm_set_flag = 0;
				alarm_p->hours_scroll_flag = 0;
				alarm_p->minutes_scroll_flag = 0;
				alarm_p->hours_scroll_timeout_cnt = 0;
				alarm_p->minutes_scroll_timeout_cnt = 0;
				alarm_p->hours_scroll_divider_cnt = 0;
				alarm_p->minutes_scroll_divider_cnt = 0;
				return; // Skip the rest
			}

			Displ_WString(10, 10, "Set alarm:", Font16, 1, YELLOW, BLACK);
			create_time_string(alarm_p->alarm_set_hours, alarm_p->alarm_set_minutes, time_str);
			Displ_WString(20, 52, time_str, Font24, 1, GREEN, BLACK);
			
			// Increment hours on short press of button0
			if (But0_press_state == BUTTON_SHORT_PRESS){
				alarm_p->alarm_set_hours = increment_bcd(alarm_p->alarm_set_hours, 0x23);
			}
			
			// Increment minutes on short press of button1
			if (But1_press_state == BUTTON_SHORT_PRESS){
				alarm_p->alarm_set_minutes = increment_bcd(alarm_p->alarm_set_minutes, 0x59);
			}
			
			// Increment hours quickly on long press of button0
			if(But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state != BUTTON_LONG_PRESS){
				alarm_p->hours_scroll_flag = 1;
			} else if (But0_press_state == BUTTON_LONG_PRESS && But0_prev_press_state == BUTTON_LONG_PRESS && 
									alarm_p->hours_scroll_flag){
				
				if (alarm_p->hours_scroll_timeout_cnt < BUTTON_SCROLL_TIMEOUT){
					alarm_p->hours_scroll_timeout_cnt++;
				} else {
					alarm_p->hours_scroll_divider_cnt++;
					if (alarm_p->hours_scroll_divider_cnt >= BUTTON_SCROLL_FREQ_DIV){
						alarm_p->alarm_set_hours = increment_bcd(alarm_p->alarm_set_hours, 0x23);
						alarm_p->hours_scroll_divider_cnt = 0;
					}
				}
				
			} else if (But0_press_state == BUTTON_IDLE){
				alarm_p->hours_scroll_flag = 0;
				alarm_p->hours_scroll_timeout_cnt = 0;
				alarm_p->hours_scroll_divider_cnt = 0;
			}
			
			// Increment minutes quickly on long press of button1
			if(But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state != BUTTON_LONG_PRESS){
				alarm_p->minutes_scroll_flag = 1;
			} else if (But1_press_state == BUTTON_LONG_PRESS && But1_prev_press_state == BUTTON_LONG_PRESS && 
									alarm_p->minutes_scroll_flag){
										
				if (alarm_p->minutes_scroll_timeout_cnt < BUTTON_SCROLL_TIMEOUT){
					alarm_p->minutes_scroll_timeout_cnt++;
				} else {
					
					alarm_p->minutes_scroll_divider_cnt++;
					if (alarm_p->minutes_scroll_divider_cnt >= BUTTON_SCROLL_FREQ_DIV){
						alarm_p->alarm_set_minutes = increment_bcd(alarm_p->alarm_set_minutes, 0x59);
						alarm_p->minutes_scroll_divider_cnt = 0;
					}
				}
				
			} else if (But1_press_state == BUTTON_IDLE){
				alarm_p->minutes_scroll_flag = 0;
				alarm_p->minutes_scroll_timeout_cnt = 0;
				alarm_p->minutes_scroll_divider_cnt = 0;
			}
			
			
			break;
		}
			
	}

}