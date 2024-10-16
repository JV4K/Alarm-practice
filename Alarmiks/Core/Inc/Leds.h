#ifndef INC_LEDS_H_
#define INC_LEDS_H_

#include "main.h"

typedef enum {DIM, BLINKING}LED_STATE;

typedef struct {
	LED_STATE led_cur_state;
	LED_STATE led_prev_state;
	uint16_t timer_frequency;
	uint16_t led_frequency;
	uint16_t trigger_time;
	uint16_t blink_time;
	uint8_t is_on;
	uint8_t counter; // Счетчик до триггера, полупериода времени моргания светодиодом
	uint16_t counter_max; // Счетчик до конца работы светодиода
} LEDS;

void led_init(uint16_t, uint16_t, uint16_t);
void leds_calculation();
void leds_blink_start();
void leds_blink_stop();

extern LEDS* alarm_led;

#endif /* INC_LEDS_H_ */
