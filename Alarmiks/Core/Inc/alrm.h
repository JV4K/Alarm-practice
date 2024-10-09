#ifndef __ALRM_H__
#define __ALRM_H__

#include "main.h"
#include "rtc.h"
#include "tim.h"


HAL_StatusTypeDef alrm_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
HAL_StatusTypeDef alrm_SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds);

#ifndef __ARLM_C__
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;
extern RTC_AlarmTypeDef sAlarm;
#endif

#endif // _ALRM_H__