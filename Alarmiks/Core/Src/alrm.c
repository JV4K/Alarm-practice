#include "alrm.h"

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
RTC_AlarmTypeDef sAlarm;

uint16_t alarm_A_flag;

HAL_StatusTypeDef alrm_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds){
	HAL_StatusTypeDef status_code;
	sTime.Hours = hours;
  sTime.Minutes = minutes;
  sTime.Seconds = seconds;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  status_code = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	
	return status_code;
}


HAL_StatusTypeDef alrm_SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds){
	HAL_StatusTypeDef status_code;
	
	sAlarm.AlarmTime.Hours = hours;
  sAlarm.AlarmTime.Minutes = minutes;
  sAlarm.AlarmTime.Seconds = seconds;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  status_code = HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
	
	return status_code;
};


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	alarm_A_flag = 1;
}


// Special increment func for BCD format
uint8_t increment_bcd(uint8_t bcd_val, uint8_t max_val) {
    // Increment the BCD value
    bcd_val++;
    
    // If ones place is greater than 9, convert to BCD tens place
    if ((bcd_val & 0x0F) > 0x09) {
        bcd_val += 0x06; // This skips invalid BCD values
    }
    
    // Handle overflow
    if (bcd_val > max_val) {
        bcd_val = 0x00; // Reset to 0 if the max BCD value is exceeded
    }

    return bcd_val;
}

