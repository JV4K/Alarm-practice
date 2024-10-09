/*
 * button.h
 *
 *  Created on: Oct 23, 2023
 *      Author: JV4K
 */

#include "button.h"
#include "stdlib.h"

Button_Handle_t* buttonInit(float debounce_time_s, float sample_freq_hz) {
	Button_Handle_t *button = (Button_Handle_t*)malloc(sizeof(Button_Handle_t));
	button->maximum = (unsigned int) (debounce_time_s * sample_freq_hz);
	return button;
}

unsigned int getDebouncedButton(Button_Handle_t *button, unsigned char input) {
	/* Step 1: Update the integrator based on the input signal.  Note that the
	 integrator follows the input, decreasing or increasing towards the limits as
	 determined by the input state (0 or 1). */

	if (input == 0) {
		if (button->integrator > 0)
			button->integrator--;
	} else if (button->integrator < button->maximum)
		button->integrator++;

	/* Step 2: Update the output state based on the integrator.  Note that the
	 output will only change states if the integrator has reached a limit, either
	 0 or MAXIMUM. */

	if (button->integrator == 0)
		button->output = 0;
	else if (button->integrator >= button->maximum) {
		button->output = 1;
		button->integrator = button->maximum; /* defensive code if integrator got corrupted */
	}
	return button->output;
}
