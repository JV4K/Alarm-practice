#ifndef __ALRM_H__
#define __ALRM_H__

#include "main.h"
#include "rtc.h"
#include "tim.h"



HAL_StatusTypeDef alrm_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
HAL_StatusTypeDef alrm_SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds);

extern uint16_t alarm_A_flag;
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;
extern RTC_AlarmTypeDef sAlarm;

#endif // _ALRM_H__