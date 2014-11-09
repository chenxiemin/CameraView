#ifndef _CXM_STREAM_H_
#define _CXM_STREAM_H_

#include "ffmpeg.h"
#include "safe-queue.h"
#include "thread.h"

namespace cxm {
namespace av {

class Player;
class Stream;

class IStreamNotify
{
	public: virtual ~IStreamNotify() { }
	public: virtual void OnFrame(Stream &stream, void *tag, AVPacket &packet,
		std::shared_ptr<MyAVFrame> frame) = 0;
};

class Stream : public cxm::util::IRunnable
{
	private: int mStreamIndex; // current stream relate
	protected: Player &mPlayer;
	protected: AVStream *mpStream;
	protected: AVCodecContext *mpCodecContext;
	protected: AVCodec *mpCodec;
	protected: IStreamNotify *mpStreamNotify;
	protected: void *mpStreamNotifyTag;

	private: std::shared_ptr<cxm::util::Thread> mThread;
	private: volatile bool needContinue;
	private: cxm::alg::SafeQueue<cxm::av::MyAVPacket> mpktQueue;

	public: Stream(Player &player, int streamIndex);
	public: virtual ~Stream();

	public: int Open();
	public: void Close();
	public: void Flush();
	public: int FillPacket(std::shared_ptr<MyAVPacket> packet);

    public: void SetStreamNotify(IStreamNotify *notify, void *notifyTag)
    {
        this->mpStreamNotify = notify;
        this->mpStreamNotifyTag = notifyTag;
    }
	public: AVCodecContext *GetContext() { return mpCodecContext; }

	private: virtual void Run();
};

}
}
#endif
