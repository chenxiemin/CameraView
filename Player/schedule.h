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
	public: bool IsSchedule(std::chrono::system_clock::time_point nowTime);
	public: virtual int DoSchedule(std::shared_ptr<MultiPlayer::OnePlayer> onePlayer) = 0;
};

class RecordSchedule : public Schedule
{
};

}
}

#endif
