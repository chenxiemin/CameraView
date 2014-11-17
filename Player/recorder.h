#ifndef _CXM_RECORDER_H_
#define _CXM_RECORDER_H_

#include <string>
#include <memory>

#include "ffmpeg.h"
#include "thread.h"
#include "player.h"
#include "common.h"

namespace cxm {
namespace av {

class Player;

class Recorder : public cxm::util::IRunnable, IPlayerProcdule {
	private: std::shared_ptr<Player> mplayer;
	private: AVFormatContext *mcontext;
	private: std::shared_ptr<cxm::util::Thread> mthread;

	public: Recorder(std::shared_ptr<Player> player);
	public: ~Recorder();

	public: int Start(const std::string &fileName, int recordTime);
	public: void Stop();

	private: virtual void Run();
	private: virtual int OnPlayerProcdule(Player &player, void *procduleTag,
		CXM_PLAYER_EVENT event, std::shared_ptr<cxm::util::object> args);
};

}
}
#endif
