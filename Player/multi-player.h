#ifndef _CXM_MULTI_PLAYER_H_
#define _CXM_MULTI_PLAYER_H_

#include <memory>
#include <list>
#include <vector>

#include "mysdl.h"
#include "ffmpeg.h"
#include "player.h"
#include "stream.h"
#include "safe-queue.h"
#include "image-merger.h"
#include "common.h"

namespace cxm {
namespace av {

class Scaler;
class Player;
class Recorder;

class MultiPlayer : public cxm::sdl::SDL,
	cxm::sdl::ISDLTimer, IPlayerProcdule, IStreamNotify
{
	private: struct OnePlayer
	{
		int mid;
		std::shared_ptr<Player> mplayer;
		std::shared_ptr<Scaler> mscaler;
		cxm::alg::SafeQueue<MyAVFrame> mqueue;
		std::shared_ptr<MyAVFrame> mcachedFrame;
	};

	private: AVFormatContext *pFormatCtx;
	private: AVCodecContext *pCodecCtx;
	private: int videoStream = -1;
	private: std::list<std::shared_ptr<OnePlayer>> mplayerList;
	private: int mopenCount;
	private: ImageMerger mmerger;
	private: std::list<std::shared_ptr<Recorder>> mrecorderList;

	public: MultiPlayer(int width, int height, void *opaque) :
		SDL(width, height, opaque), mopenCount(0)
	{
	}

	public: virtual ~MultiPlayer()
	{
	}

	public: int Play(const std::string &url);
	public: void Close();

	public: int Record(const std::string &fileName, int channel, int time);

	private: virtual void OnTimer(void *opaque);
	private: virtual void OnKeyDown(const SDL_Event &event);
	private: virtual int OnPlayerProcdule(Player &player, void *procduleTag,
		CXM_PLAYER_EVENT event, std::shared_ptr<cxm::util::object> args);
	private: virtual void OnFrame(Stream &stream, void *tag, AVPacket &packet,
		std::shared_ptr<MyAVFrame> frame);
};

}
}
#endif
