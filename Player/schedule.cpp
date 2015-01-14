#include <sstream>

#include "schedule.h"
#include "log.h"
#include "time-helper.h"
#include "schedule-manager.h"

using namespace std;
using namespace std::chrono;
using namespace cxm::util;

namespace cxm {
namespace av {

int Schedule::DoSchedule()
{
	if (NULL == monePlayer.get()) {
		LOGE("Unknown one player");
		return -1;
	}
	if (misSchedule) {
		LOGE("Already in scheduled");
		return -1;
	}

	int res = this->OnDoSchedule();
	if (0 != res) {
		LOGE("Cannot start schedule: " + res);
		return res;
	}

	this->misSchedule = true;
	return 0;
}

void Schedule::UndoSchedule()
{
	if (!misSchedule) {
		LOGW("Already stop schedule");
		return;
	}

	this->OnUndoSchedule();
	this->misSchedule = false;
}

WeekPeriodSchedule::WeekPeriodSchedule(string schedule) : Schedule(schedule)
{
	mstartTime = -1;
	mendTime = -1;
	memset(this->mweek, 0, sizeof(mweek));

	int startTime;
	int endTime;
	bool week[7];

	stringstream ss(schedule);
	ss >> startTime;
	if (!ss) {
		LOGE("string stream error when input startTime: %s", schedule.c_str());
		return;
	}
	if (startTime >= 24 * 60 * 60) {
		LOGE("Invalid start Time: %d", startTime);
		return;
	}
	ss >> endTime;
	if (!ss) {
		LOGE("stringstream error when input endTime: %s", schedule.c_str());
		return;
	}
	if (endTime >= 24 * 60 * 60) {
		LOGE("Invalid end time: %d", endTime);
		return;
	}
	if (endTime <= startTime) {
		LOGE("invalid time elspse: %d to %d", startTime, endTime);
		return;
	}
	for (int i = 0; i < 7; i++) {
		ss >> week[i];
		if (!ss) {
			LOGE("stringstream error when input week[%d]: %s", i, schedule.c_str());
			return;
		}
	}

	mstartTime = startTime;
	mendTime = endTime;
	memcpy(mweek, week, sizeof(week));
}

int WeekPeriodSchedule::NeedSchedule(system_clock::time_point nowTime)
{
	if (mstartTime < 0 || mendTime <= 0 || mstartTime >= 24 * 60 * 60 ||
		mendTime >= 24 * 60 * 60 || mstartTime >= mendTime) {
		LOGE("Invalid time elapse: %d to %d", mstartTime, mendTime);
		return -1;
	}

	// get week
	int week = TimerHelper::DayOfWeek(nowTime);
	if (!mweek[week])
		return -1;
	// get seconds duration
	int sec = TimerHelper::SecondsOfDay(nowTime);

	if (sec >= mstartTime && sec < mendTime)
		return mendTime - sec;
	else
		return -1;
}

int WeekPeriodSchedule::NeedRechedule(system_clock::time_point nowTime)
{
	return mweek[TimerHelper::DayOfWeek(nowTime)];
}

int WeekPeriodSchedule::OnDoSchedule()
{
	LOGD("In week period schedule doschedule");
	return 0;
}

void WeekPeriodSchedule::OnUndoSchedule()
{
	LOGD("In week period schedule undoschedule");
}

int AlwaysSchedule::OnDoSchedule()
{
	shared_ptr<MultiPlayer> multiPlayer = ScheduleManager::GetInstance()->GetMultiPlayer();
	if (multiPlayer->IsRecord(this->monePlayer)) {
		LOGE("Already in schedule for player: %d", this->monePlayer->mid);
		return -1;
	}

	return multiPlayer->Record(this->monePlayer);
}

void AlwaysSchedule::OnUndoSchedule()
{
	shared_ptr<MultiPlayer> multiPlayer = ScheduleManager::GetInstance()->GetMultiPlayer();
	if (!multiPlayer->IsRecord(this->monePlayer)) {
		LOGE("player not in recording: %d", this->monePlayer->mid);
		return;
	}
	multiPlayer->StopRecord(this->monePlayer);
}

shared_ptr<Schedule> ScheduleFactory::GetSchedule(string type, string schedule)
{
	if ("WeekPeriodSchedule" == type) {
		return shared_ptr<Schedule>(new WeekPeriodSchedule(schedule));
	} else if ("AlwaysSchedule" == type) {
		return shared_ptr<Schedule>(new AlwaysSchedule());
	} else {
		LOGE("Unknown schedule: %s", type.c_str());
		return NULL;
	}
}

}
}
