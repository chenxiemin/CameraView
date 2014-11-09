#ifndef _CXM_MULTI_PLAYER_H_
#define _CXM_MULTI_PLAYER_H_

#include <memory>

#include "mysdl.h"
#include "ffmpeg.h"
#include "player.h"
#include "stream.h"
#include "safe-queue.h"

namespace cxm {
namespace av {

class Scaler;
class Player;

class MultiPlayer : public cxm::sdl::SDL,
	cxm::sdl::ISDLTimer, IPlayerProcdule, IStreamNotify
{
	private: AVFormatContext *pFormatCtx;
	private: AVCodecContext *pCodecCtx;
	private: int videoStream = -1;
	private: std::shared_ptr<Scaler> scaler;
	private: std::shared_ptr<Player> mplayer;
	private: cxm::alg::SafeQueue<MyAVFrame> mqueue;

	public: MultiPlayer(int width, int height, void *opaque) :
		SDL(width, height, opaque)
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
};

}
}
#endif
