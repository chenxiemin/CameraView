#include "sdl-player.h"
#include "schedule-manager.h"
#include "schedule.h"

using namespace cxm::av;
using namespace std;

extern "C" EXPORT_HEAD(void) test()
{
	ScheduleManager *psm = ScheduleManager::GetInstance();
	shared_ptr<MultiPlayer> mp = shared_ptr<MultiPlayer>(new MultiPlayer(640, 360, NULL));
	mp->Play("rtsp://192.168.0.129/12");

	psm->SetPlayer(mp);
	psm->Add(ScheduleFactory::GetSchedule("WeekPeriodSchedule", "180 200 0 1 1 0 0 1 0"), "rtsp://192.168.0.129/12");
	psm->Start();
	mp->EventLoop();
	mp->Close();

	psm->Stop();
	ScheduleManager::Release();
}
