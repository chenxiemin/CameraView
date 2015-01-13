#include "schedule-manager.h"
#include "log.h"
#include "multi-player.h"
#include "time-helper.h"

using namespace std;
using namespace std::chrono;
using namespace cxm::util;

namespace cxm {
namespace av {

ScheduleManager *ScheduleManager::instance = NULL;

void ScheduleManager::Start()
{
	assert(NULL != mplayer.get());

	mplayer->AddTimer(0, this, NULL);
}

void ScheduleManager::Stop()
{
	mplayer->RemoveTimer(this);
}

void ScheduleManager::OnTimer(void *opaque)
{
	this->Scheduling(system_clock::now());

	this->mplayer->AddTimer(3 * 1000, this, NULL);
}

void ScheduleManager::Scheduling(std::chrono::system_clock::time_point nowTime)
{
	static int lastSec = 0;

	// reverse schedule list if necessary to add scheduled list to schedule list
	int sec = TimerHelper::SecondsOfDay(nowTime);
	if (sec < lastSec) {
		// start of one day, check scheduled list
		auto copyList = mscheduledList;
		for (auto iter = copyList.begin(); iter != copyList.end(); iter++) {
			if ((*iter)->NeedRechedule(nowTime) > 0) {
				mscheduledList.erase(iter - copyList.begin() + mscheduledList.begin());
				mscheduleList.push_back(*iter);

				// confirm status
				if ((*iter)->IsSchedule()) {
					LOGE("Schedule unwanted scheduled status");
					(*iter)->UndoSchedule();
				}
			}
		}
	}
	lastSec = sec;

	// check schedule list to add unscheduled list to scheduling list
	auto copyList = mscheduleList;
	for (auto iter = copyList.begin(); iter != copyList.end(); iter++) {
		if ((*iter)->NeedSchedule(nowTime) > 0) {
			mscheduleList.erase(iter - copyList.begin() + mscheduleList.begin());
			mschedulingList.push_back(*iter);

			// schedule
			if (!(*iter)->IsSchedule())
				(*iter)->DoSchedule();
			else
				LOGE("Unwanted schedule status");
		}
	}

	// check scheduling list to add scheduling list to scheduled list
	auto copySchedulingList = mschedulingList;
	for (auto iter = copySchedulingList.begin(); iter != copySchedulingList.end(); iter++) {
		if ((*iter)->NeedSchedule(nowTime) <= 0) {
			mschedulingList.erase(iter - copySchedulingList.begin() + mschedulingList.begin());
			mscheduledList.push_back(*iter);

			if ((*iter)->IsSchedule())
				(*iter)->UndoSchedule();
			else
				LOGE("Unwanted unscheduled status");
		}
	}
}

int ScheduleManager::Add(shared_ptr<Schedule> schedule, string playerUrl)
{
	if (NULL == schedule.get() || playerUrl.length() <= 0) {
		LOGE("Illegal argument");
		return -1;
	}

	// get player
	shared_ptr<MultiPlayer::OnePlayer> onePlayer = mplayer->GetPlayerByUrl(playerUrl);
	if (NULL == onePlayer.get()) {
		LOGE("Cannot find player by url: %s", playerUrl.c_str());
		return -1;
	}
	schedule->SetPlayer(onePlayer);
	// add to schedule list
	mscheduleList.push_back(schedule);

	return 0;
}

}
}
