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

namespace cxm {
namespace av {

class Scaler;
class Player;

class MultiPlayer : public cxm::sdl::SDL,
	cxm::sdl::ISDLTimer, IPlayerProcdule, IStreamNotify
{
	private: struct OnePlayer
	{
		int mid;
		std::shared_ptr<Player> mplayer;
		std::shared_ptr<Scaler> mscaler;
		cxm::alg::SafeQueue<MyAVFrame> mqueue;
	};

	private: AVFormatContext *pFormatCtx;
	private: AVCodecContext *pCodecCtx;
	private: int videoStream = -1;
	private: std::list<std::shared_ptr<OnePlayer>> mplayerList;
	private: int mopenCount;
	private: ImageMerger mmerger;

	public: MultiPlayer(int width, int height, void *opaque) :
		SDL(width, height, opaque), mopenCount(0)
	{
	}

	public: virtual ~MultiPlayer()
	{
	}

	public: int Play(const std::string &url);
	public: void Close();

	private: virtual void OnTimer(void *opaque);
	private: int MultiPlayer::OnPlayerProcdule(Player &player, void *procduleTag,
		CXM_PLAYER_EVENT event, void *eventArgs);
	private: void OnFrame(Stream &stream, void *tag, AVPacket &packet,
		std::shared_ptr<MyAVFrame> frame);

	private: static void Split(const std::string &str,
		char spliter, std::vector<std::string> &container);
};

}
}
#endif
