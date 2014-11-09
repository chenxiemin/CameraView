#ifndef _CXM_PLAYER_H_
#define _CXM_PLAYER_H_

#include <map>
#include <memory>

#include "analysiser.h"
#include "thread.h"
#include "safe-queue.h"
#include "ffmpeg.h"

namespace cxm {
namespace av {

class Player;
class Stream;

enum CXM_PLAYER_STATUS {
	CXM_PLAYER_UNKNOWN,
	CXM_PLAYER_PLAYING,
	CXM_PLAYER_PLAY,
	CXM_PLAYER_STOPING,
	CXM_PLAYER_STOP,
	CXM_PLAYER_ERROR
};

enum CXM_PLAYER_EVENT {
	CXM_PLAYER_EVENT_OPENED,
	CXM_PLAYER_EVENT_STREAM_OPENED,
	CXM_PLAYER_EVENT_STREAM_ERROR,
	CXM_PLAYER_EVENT_STREAM_END
};

class IPlayerProcdule {
	public: virtual int OnPlayerProcdule(Player &player, void *procduleTag,
			CXM_PLAYER_EVENT event, void *eventArgs) = 0;

	public: virtual ~IPlayerProcdule() { }
};

class Player : public cxm::util::IRunnable
{
	private: enum PlayerEvent {
		 PLAYER_EVENT_OPEN,
		 PLAYER_EVENT_CLOSE,
		 PLAYER_EVENT_READ_FRAME,
		 PLAYER_EVENT_REOPEN
	};
	private: static const int INTERRUPT_TIME = 5000; // 5000ms for open
	private: static const int FRAME_RETRY_COUNT = 3; // retry times 

	private: std::string mUrl;
	private: std::string mOption;
	private: AVFormatContext *mpFormatContext;
	private: cxm::util::Analysiser mInterruptAnlaysiser;
	private: std::map<unsigned int, std::shared_ptr<Stream> > mStreamList;
	private: std::shared_ptr<cxm::util::Thread> mThread;
	private: volatile CXM_PLAYER_STATUS mStatus;
	private: cxm::alg::SafeQueue<PlayerEvent> meventQueue;
	private: int mretryCount; // count for frame failed

	private: IPlayerProcdule *mpPlayerProcdule;
	private: void *mpProcduleTag;

	public: Player();
	public: virtual ~Player();

	public: int Open(const std::string &url, const std::string &option);
	public: void Close();

	public: void SetPlayerProcdule(IPlayerProcdule *procdule, void *procTag)
	{
		this->mpPlayerProcdule = procdule;
		this->mpProcduleTag = procTag;
	}
	public: AVFormatContext *GetContext() { return mpFormatContext; }
	public: const std::string &GetURL() { return mUrl; }
	public: std::shared_ptr<Stream> GetStream(int index) { return mStreamList[index]; }

	private: std::shared_ptr<Stream> OpenStream(int type, int index);
	private: virtual void Run();
	protected: int OnOpen();
	protected: void OnClose();
	protected: int OnFrame();

	private: std::shared_ptr<MyAVPacket> ReadPacket();
	private: int OnInterrupt();
	private: static int InterruptCallback(void *arg);
};

}
}
#endif
