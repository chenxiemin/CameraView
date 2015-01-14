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
	public: struct OnePlayer
	{
		int mid; // unique id
		int mindex; // index is variable, from 0 ~ N contogous
		std::shared_ptr<Player> mplayer;
		std::shared_ptr<Scaler> mscaler;
		cxm::alg::SafeQueue<MyAVFrame> mqueue;
		std::shared_ptr<MyAVFrame> mcachedFrame;

		OnePlayer() : mqueue(10) { }
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
	public: int Record(std::shared_ptr<OnePlayer> onePlayer);
	public: void StopRecord(int playerId);
	public: void StopRecord(std::shared_ptr<OnePlayer> onePlayer);
	public: bool IsRecord(int playerId);
	public: bool IsRecord(std::shared_ptr<OnePlayer> onePlayer);

	/*
	public: int GetPlayerCount() { return mplayerList.size(); }
	public: std::shared_ptr<OnePlayer> GetPlayers(int index)
	{
		if (index >= (int)mplayerList.size())
			return NULL;

		auto iter = mplayerList.begin();
		std::advance(iter, index);
		return (*iter)->mplayer;
	}
	*/
	public: std::list<std::shared_ptr<OnePlayer>> GetPlayerList()
	{
		return mplayerList;
	}

	public: std::shared_ptr<OnePlayer> GetPlayerByUrl(std::string url)
	{
		for (auto iter = mplayerList.begin(); iter != mplayerList.end(); iter++)
			if ((*iter)->mplayer->GetURL() == url)
				return *iter;

		return NULL;
	}

	public: void ItermDisplayGrid();
	public: void PageLeft();
	public: void PageRight();
	// show [index] picture in 1x1 grid
	public: void ShowSingle(int index);

	private: virtual void OnTimer(void *opaque);
	private: virtual void OnKeyDown(const SDL_Event &event);
	public: virtual void OnMouseDown(const SDL_Event &event);
	private: virtual int OnPlayerProcdule(Player &player, void *procduleTag,
		CXM_PLAYER_EVENT event, std::shared_ptr<cxm::util::object> args);
	private: virtual void OnFrame(Stream &stream, void *tag, AVPacket &packet,
		std::shared_ptr<MyAVFrame> frame);
};

}
}
#endif
