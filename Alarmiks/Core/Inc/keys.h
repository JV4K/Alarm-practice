#ifndef __KEYS_H__
#define __KEYS_H__

#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "button.h"



extern unsigned int But0_State, But1_State;
extern Button_Handle_t *hbut1, *hbut0;

#endif // __KEYS_H__