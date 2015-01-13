#ifndef _CXM_SCHEDULE_MANAGER_H_
#define _CXM_SCHEDULE_MANAGER_H_

#include <memory>
#include <vector>

#include "schedule.h"
#include "multi-player.h"

namespace cxm {
namespace av {

class ScheduleManager : public cxm::sdl::ISDLTimer
{
	private: static ScheduleManager *instance;

	// schedule list which already scheduled
	private: std::vector<std::shared_ptr<Schedule>> mscheduledList;
	// schedule list which is currently scheduling
	private: std::vector<std::shared_ptr<Schedule>> mschedulingList;
	// schedule list which wait for scheduling
	private: std::vector<std::shared_ptr<Schedule>> mscheduleList;
	private: std::shared_ptr<MultiPlayer> mplayer;

	private: ScheduleManager() { }

	public: void Start();
	public: void Stop();

	protected: virtual void OnTimer(void *opaque);
	// run schedule for every 1 min
	protected: void Scheduling(std::chrono::system_clock::time_point nowTime =
		std::chrono::system_clock::now());
	// add a schedule into plan
	// return >= 0 for its index
	// < 0 for fail
	public: int Add(std::shared_ptr<Schedule> schedule, std::string playerUrl);
	// remove a schedule by index
	public: void Remove(int index) { }
	public: void SetPlayer(std::shared_ptr<MultiPlayer> player) { mplayer = player; }

	// load schedule list from file
	private: int Load() { return -1; }
	// save schedule list into file
	private: int Save() { return -1; }

	public: static ScheduleManager *GetInstance()
	{
		if (NULL == instance)
			instance = new ScheduleManager();

		return instance;
	}

	public: static void Release()
	{
		if (NULL != instance) {
			delete instance;
			instance = NULL;
		}
	}
};

}
}
#endif
