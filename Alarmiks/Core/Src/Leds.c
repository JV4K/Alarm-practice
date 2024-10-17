#include "Leds.h"

LEDS govno;
LEDS* alarm_led = &govno;

void led_init(uint16_t timer_frequency,
	uint16_t led_frequency, uint16_t blink_time) {
	alarm_led->led_cur_state = DIM;
	alarm_led->timer_frequency = timer_frequency;
	alarm_led->led_frequency = led_frequency;
	alarm_led->blink_time = blink_time;
	alarm_led->is_on = 1;
	alarm_led->counter = 0;
	alarm_led->counter_max = 0;
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
}

void leds_calculation() {
	
	if (alarm_led->led_cur_state == BLINKING){
		alarm_led->counter++;
		alarm_led->counter_max++;

		alarm_led->trigger_time = alarm_led->timer_frequency / (alarm_led->led_frequency * 2);

		if (alarm_led->counter_max >= (alarm_led->blink_time * alarm_led->timer_frequency)) {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
			alarm_led->counter_max = 0;
			alarm_led->led_cur_state = DIM;
		}
		else if (alarm_led->counter <= alarm_led->trigger_time) {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, !alarm_led->is_on);
			if(alarm_led->counter == alarm_led->trigger_time) {
				alarm_led->is_on = !alarm_led->is_on;
				alarm_led->counter = 0;
			}
		}
	} else{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
	}
}

void leds_blink_start(){
	alarm_led->counter_max = 0;
	alarm_led->counter = 0;
	alarm_led->is_on = 1;
	alarm_led->led_cur_state = BLINKING;
}

void leds_blink_stop(){
	alarm_led->counter_max = 0;
	alarm_led->counter = 0;
	alarm_led->is_on = 0;
	alarm_led->led_cur_state = DIM;
}