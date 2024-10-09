/*
 * button.h
 *
 *  Created on: Oct 23, 2023
 *      Author: JV4K
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

typedef struct {
	unsigned int maximum;

	unsigned int integrator; /* Will range from 0 to the specified maximum */
	unsigned int output;

} Button_Handle_t;

Button_Handle_t* buttonInit(float debounce_time_s, float sample_freq_hz);
unsigned int getDebouncedButton(Button_Handle_t* button, unsigned char input);

#endif /* INC_BUTTON_H_ */
