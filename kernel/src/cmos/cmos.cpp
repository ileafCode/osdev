
#include "cmos.h"
#include "../scheduling/pit/pit.h"

uint32_t secs_of_years(int years)
{
    uint32_t days = 0;
    years += 2000;
    while (years > 1969)
    {
        days += 365;
        if (years % 4 == 0)
        {
            if (years % 100 == 0)
            {
                if (years % 400 == 0)
                {
                    days++;
                }
            }
            else
            {
                days++;
            }
        }
        years--;
    }
    return days * 86400;
}

uint32_t secs_of_month(int months, int year)
{
    year += 2000;

    uint32_t days = 0;
    switch (months)
    {
    case 11:
        days += 30;
    case 10:
        days += 31;
    case 9:
        days += 30;
    case 8:
        days += 31;
    case 7:
        days += 31;
    case 6:
        days += 30;
    case 5:
        days += 31;
    case 4:
        days += 30;
    case 3:
        days += 31;
    case 2:
        days += 28;
        if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
        {
            days++;
        }
    case 1:
        days += 31;
    default:
        break;
    }
    return days * 86400;
}

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

const uint64_t daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

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

    const uint64_t yearsSinceEpoch = (2000 + time.year) - 1970; // Let's count the number of years passed since the Epoch Year: (1970)
    uint64_t leapYears = yearsSinceEpoch / 4;                      // We need to know how many leap years too...

    // if yearsSinceEpoch % 4 is greater/equal than 2 we have to add another leap year
    if ((yearsSinceEpoch % 4) > 1)
    {
        leapYears++;
    }

    uint64_t daysCurrentYear = 0;
    for (int i = 0; i < time.month - 1; i++)
    {
        daysCurrentYear += daysPerMonth[i];
    }

    daysCurrentYear = daysCurrentYear + (time.dayOfMonth);

    const uint64_t daysSinceEpoch = (yearsSinceEpoch * 365) - 1;
    const uint64_t unixTimeOfDay = ((((time.hour * 3600) + (time.minute * 60) + time.second) * 1000) - 600) + ((uint64_t)PIT::TimeSinceBoot % 1000);
    time.epoch = (((daysSinceEpoch * 86400) + (leapYears * 86400) + (daysCurrentYear * 86400) + unixTimeOfDay));

    return time;
}