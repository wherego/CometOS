#include <kernel/portio.h>

#define TIME_SEC 0x00 //seconds
#define TIME_MIN 0x02 //minutes
#define TIME_HOUR 0x04 //hours
#define TIME_WEEK 0x06 //weekdays
#define TIME_DOM 0x07 //day of month
#define TIME_MONTH 0x08 //month
#define TIME_YEAR 0x09 //year

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define CURRENT_YEAR 2014                            // Change this each year!
 
int century_register = 0x00;                                // Set by ACPI table parsing code if possible
 
unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;
 
unsigned int get_update_in_progress_flag()
{
      outport8(CMOS_ADDRESS, 0x0A);
      return inport8(CMOS_DATA) & 0x80;
}
 
unsigned char get_RTC_register(int reg)
{
      outport8(CMOS_ADDRESS, reg);
      return inport8(CMOS_DATA);
}
 
void read_rtc()
{
      unsigned char century;
      unsigned char last_second;
      unsigned char last_minute;
      unsigned char last_hour;
      unsigned char last_day;
      unsigned char last_month;
      unsigned char last_year;
      unsigned char last_century;
      unsigned char registerB;
 
      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates
 
      while (get_update_in_progress_flag());                // Make sure an update isn't in progress
      second = get_RTC_register(TIME_SEC);
      minute = get_RTC_register(TIME_MIN);
      hour = get_RTC_register(TIME_HOUR);
      day = get_RTC_register(TIME_DOM);
      month = get_RTC_register(TIME_MONTH);
      year = get_RTC_register(TIME_YEAR);
      if(century_register != 0)
      {
            century = get_RTC_register(century_register);
      }
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;
 
            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            second = get_RTC_register(TIME_SEC);
            minute = get_RTC_register(TIME_MIN);
            hour = get_RTC_register(TIME_HOUR);
            day = get_RTC_register(TIME_DOM);
            month = get_RTC_register(TIME_MONTH);
            year = get_RTC_register(TIME_YEAR);
            if(century_register != 0) {
                  century = get_RTC_register(century_register);
              }

                   while((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century))
                   	{
                   		registerB = get_RTC_register(0x0B);
                   	}
 
      // Convert BCD to binary values if necessary
 
      if (!(registerB & TIME_HOUR))
      {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0)
            {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }
 
      // Convert 12 hour clock to 24 hour clock if necessary
 
      if (!(registerB & TIME_MIN) && (hour & 0x80))
      {
            hour = ((hour & 0x7F) + 12) % 24;
      }
 
      // Calculate the full (4-digit) year
 
      if(century_register != 0)
      {
            year += century * 100;
      }
      else
      {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) year += 100;
      }
}

unsigned int time_get(int type)
{
	read_rtc();

	switch(type)
	{
		case 0:
			return second;
		break;

		case 1:
			return minute;
		break;

		case 2:
			return hour;
		break;

		case 3:
			return day;
		break;

		case 4:
			return month;
		break;

		case 5:
			return year;
		break;
	}
}
