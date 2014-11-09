#ifndef _CXM_ANALYSISER_H
#define _CXM_ANALYSISER_H

#include <time.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <stdint.h>
#include "log.h"

namespace cxm {
namespace util {

class Analysiser {
	private: struct timeval localtime;
	private: struct timeval lasttime;
	public: Analysiser()
	{
		GetTime(&this->localtime);
        this->lasttime = localtime;
	}

	public: int Analysis()
	{
		struct timeval currentTime;
		GetTime(&currentTime);

		int mils = (currentTime.tv_sec - localtime.tv_sec) * 1000 +
			(currentTime.tv_usec - localtime.tv_usec) / 1000;

        lasttime = localtime;
		localtime = currentTime;
		return mils;
	}

    public: void Revert()
    {
        this->localtime = this->lasttime;
    }

    public: static uint64_t GetTick()
    {
        struct timeval time;
        GetTime(&time);

        return time.tv_sec * 1000 + time.tv_usec / 1000;
    }

	private: static void GetTime(struct timeval *ptime)
	{
#ifdef WIN32
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear - 1900;
		tm.tm_mon = wtm.wMonth - 1;
		tm.tm_mday = wtm.wDay;
		tm.tm_hour = wtm.wHour;
		tm.tm_min = wtm.wMinute;
		tm.tm_sec = wtm.wSecond;
		tm.tm_isdst = -1;
		clock = mktime(&tm);
		ptime->tv_sec = (long)clock;
		ptime->tv_usec = wtm.wMilliseconds * 1000;
#else
        gettimeofday(ptime, NULL);
#endif
	}
};
}
}

#endif
