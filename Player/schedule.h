#ifndef _CXM_SCHEDULE_H_
#define _CXM_SCHEDULE_H_

#include <chrono>
#include <memory>

#include "common.h"
#include "multi-player.h"

namespace cxm {
namespace av {

class Schedule : public cxm::util::object
{
	private: bool misSchedule;
	protected: std::shared_ptr<MultiPlayer::OnePlayer> monePlayer;
	protected: std::string mscheduleString;

	public: Schedule(std::string schedule) : misSchedule(false),
		mscheduleString(schedule) { }

	// test whether self need to schedule at nowTime
	// return > 0 for still in schedule
	// return <= 0 for over time
	public: virtual int NeedSchedule(std::chrono::system_clock::time_point nowTime) = 0;
	// whether schedule need to be reschedule once a new day
	// > 0 for needing reschedule
	public: virtual int NeedRechedule(std::chrono::system_clock::time_point nowTime) = 0;

	// whether schedule running
	public: bool IsSchedule() { return misSchedule; }
	public: int DoSchedule();
	public: void UndoSchedule();

	protected: virtual int OnDoSchedule() = 0;
	protected: virtual void OnUndoSchedule() = 0;

	public: void SetPlayer(std::shared_ptr<MultiPlayer::OnePlayer> onePlayer) { monePlayer = onePlayer; }
	// public: bool operator<(const Schedule &rhs) { return mstartTime < rhs.mstartTime; }
	// public: bool operator>(const Schedule &rhs) { return mstartTime > rhs.mstartTime; }
};

// schedule in a week perioditcally
// schedule string as [startTime endTime monday tuesday...]
// startTime and endTime for 0(s) to 24 * 60 * 60(s)
class WeekPeriodSchedule : public Schedule
{
	private: bool mweek[7];
	private: int mstartTime; // from 0 to 24 * 60 * 60
	private: int mendTime;

	public: WeekPeriodSchedule(std::string schedule);

	public: virtual int NeedSchedule(std::chrono::system_clock::time_point nowTime);
	public: virtual int NeedRechedule(std::chrono::system_clock::time_point nowTime);

	protected: virtual int OnDoSchedule();
	protected: virtual void OnUndoSchedule();
};

class AlwaysSchedule : public Schedule
{
	public: AlwaysSchedule() : Schedule("") { }
	public: virtual int NeedSchedule(std::chrono::system_clock::time_point nowTime) { return 1; }
	public: virtual int NeedRechedule(std::chrono::system_clock::time_point nowTime) { return 1; }

	protected: virtual int OnDoSchedule();
	protected: virtual void OnUndoSchedule();
};

class ScheduleFactory 
{
	public: static std::shared_ptr<Schedule> GetSchedule(std::string type, std::string schedule);
};

}
}

#endif
