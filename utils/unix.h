#pragma once
#ifndef UNIX_H
#define UNIX_H
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <ctime>

std::string dateToUnixTimestamp(const std::string& strDate) // date = "dd/mm/yyyy hh/mm/ss" estrictamente
{
    int day = std::stoi(strDate.substr(0, 2));
    int month = std::stoi(strDate.substr(3, 2));
    int year = std::stoi(strDate.substr(6, 4));
    int hour = std::stoi(strDate.substr(11, 2));
    int min = std::stoi(strDate.substr(14, 2));
    int sec = std::stoi(strDate.substr(17, 2));

	time_t rawtime;
	struct tm * timeinfo;
	/* get current timeinfo: */
	time (&rawtime); //or: rawtime = time(0);
	/* convert to struct: */
	timeinfo = localtime( &rawtime );
	/* now modify the timeinfo to the given date: */
	timeinfo->tm_year   = year - 1900;
	timeinfo->tm_mon    = month - 1;    //months since January - [0,11]
	timeinfo->tm_mday   = day;          //day of the month - [1,31]
	timeinfo->tm_hour   = hour;         //hours since midnight - [0,23]
	timeinfo->tm_min    = min;          //minutes after the hour - [0,59]
	timeinfo->tm_sec    = sec;          //seconds after the minute - [0,59]
	/* call mktime: create unix time stamp from timeinfo struct */
	auto date = timegm( timeinfo );
	return std::to_string(date);
}

std::string unixTimeToHumanReadable(const long int& seconds)
{
    std::string ans = "";
    int daysOfMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    long int currYear, daysTillNow, extraTime, extraDays, index, date, month, hours, minutes, secondss, flag = 0;

    daysTillNow = seconds / (24 * 60 * 60);
	extraTime = seconds % (24 * 60 * 60);
	currYear = 1970;

    while (true)
    {
		if (currYear % 400 == 0 || (currYear % 4 == 0 && currYear % 100 != 0))
        {
			if (daysTillNow < 366) { break; }
			daysTillNow -= 366;
		}
		else
        {
            if (daysTillNow < 365) { break; }
			daysTillNow -= 365;
		}
		currYear += 1;
	}

    extraDays = daysTillNow + 1;

    if (currYear % 400 == 0 || (currYear % 4 == 0 && currYear % 100 != 0)) { flag = 1; }

    month = 0, index = 0;
    if (flag == 1)
    {
		while (true)
        {
			if (index == 1)
            {
				if (extraDays - 29 < 0) { break; }
				month += 1;
				extraDays -= 29;
			}
			else
            {
				if (extraDays - daysOfMonth[index] < 0) { break; }
				month += 1;
				extraDays -= daysOfMonth[index];
			}
			index += 1;
		}
	}
	else
    {
		while (true)
        {
			if (extraDays - daysOfMonth[index] < 0) { break; }
			month += 1;
			extraDays -= daysOfMonth[index];
			index += 1;
		}
	}

    if (extraDays > 0)
    {
		month += 1;
		date = extraDays;
	}
	else
    {
		if (month == 2 && flag == 1) { date = 29; }
		else { date = daysOfMonth[month - 1]; }
	}

    hours = extraTime / 3600;
	minutes = (extraTime % 3600) / 60;
	secondss = (extraTime % 3600) % 60;

	ans += std::to_string(date);
	ans += "/";
	ans += std::to_string(month);
	ans += "/";
	ans += std::to_string(currYear);
	ans += " ";
	ans += std::to_string(hours);
	ans += ":";
	ans += std::to_string(minutes);
	ans += ":";
	ans += std::to_string(secondss);

    return ans;
}

#endif