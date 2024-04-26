
#include "cmos.h"

int get_update_in_progress_flag()
{
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

uint8_t get_RTC_register(int reg)
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

time_t rtcGetTime()
{
    time_t time;
    time_t last;
    while (get_update_in_progress_flag())
        ;

    time.second = get_RTC_register(0x00);
    time.minute = get_RTC_register(0x02);
    time.hour = get_RTC_register(0x04);

    time.weekday = get_RTC_register(0x06);
    time.dayOfMonth = get_RTC_register(0x07);
    time.month = get_RTC_register(0x08);
    time.year = get_RTC_register(0x09);

    uint8_t regB = 0;

    do
    {
        last.second = time.second;
        last.minute = time.minute;
        last.hour = time.hour;
        last.weekday = time.weekday;
        last.dayOfMonth = time.dayOfMonth;
        last.month = time.month;
        last.year = time.year;

        while (get_update_in_progress_flag())
            ; // Make sure an update isn't in progress
        time.second = get_RTC_register(0x00);
        time.minute = get_RTC_register(0x02);
        time.hour = get_RTC_register(0x04);

        time.weekday = get_RTC_register(0x06);
        time.dayOfMonth = get_RTC_register(0x07);
        time.month = get_RTC_register(0x08);
        time.year = get_RTC_register(0x09);

    } while ((last.second != time.second) || (last.minute != time.minute) || (last.hour != time.hour) ||
             (last.weekday != time.weekday) || (last.dayOfMonth != time.dayOfMonth) ||
             (last.month != time.month) || (last.year != time.year));

    regB = get_RTC_register(0x0B);

    if (!(regB & 0x04))
    {
        time.second = (time.second & 0x0F) + ((time.second / 16) * 10);
        time.minute = (time.minute & 0x0F) + ((time.minute / 16) * 10);
        time.hour = ((time.hour & 0x0F) + (((time.hour & 0x70) / 16) * 10)) | (time.hour & 0x80);
        time.weekday = (time.weekday & 0x0F) + ((time.weekday / 16) * 10);
        time.dayOfMonth = (time.dayOfMonth & 0x0F) + ((time.dayOfMonth / 16) * 10);
        time.month = (time.month & 0x0F) + ((time.month / 16) * 10);
        time.year = (time.year & 0x0F) + ((time.year / 16) * 10);
    }

    if (!(regB & 0x02) && (time.hour & 0x80))
        time.hour = ((time.hour & 0x7F) + 12) % 24;
    
    return time;
}