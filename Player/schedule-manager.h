#ifndef _CXM_SCHEDULE_MANAGER_H_
#define _CXM_SCHEDULE_MANAGER_H_

#include <memory>
#include <vector>

#include "mysdl.h"
#include "schedule.h"

namespace cxm {
namespace av {

class ScheduleManager
{
	private: static ScheduleManager *instance;

	private: std::shared_ptr<cxm::sdl::SDL> msdl;
	private: std::vector<std::shared_ptr<Schedule>> mscheduleList;

	private: ScheduleManager();

	public: void SetWindow(std::shared_ptr<cxm::sdl::SDL> sdl) { msdl = sdl; }

	public: int Schedule();
	public: int Add();
	public: int Remove();
	private: int Load();
	private: int Save();

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
