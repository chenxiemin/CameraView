#include <string>
#include <chrono>

#include "stream.h"
#include "player.h"
#include "thread.h"

using namespace std;
using namespace std::chrono;
using namespace cxm::util;

namespace cxm {
namespace av {

Stream::Stream(Player &player, int streamIndex) : mPlayer(player),
	mStreamIndex(streamIndex), mpCodecContext(NULL), mpCodec(NULL),
	needContinue(false), mpktQueue(50)
{
	this->mpStreamNotify = NULL;
	this->mpStreamNotifyTag = NULL;

	string str = "AVFrame " + player.GetURL();
	mpktQueue.SetName(str.c_str());
}

Stream::~Stream()
{

}

int Stream::Open()
{
	if (NULL != this->mpCodecContext)
		return -1;

	do {
		const AVFormatContext *pFormatContext = mPlayer.GetContext();
		this->mpStream = pFormatContext->streams[mStreamIndex];
		this->mpCodecContext = pFormatContext->streams[mStreamIndex]->codec;

		// get decoder
		this->mpCodec = avcodec_find_decoder(this->mpCodecContext->codec_id);
		if (NULL == this->mpCodec) {
			LOG(LOG_LEVEL_E, "Cannot find decoder for id: %d\n", mpCodecContext->codec_id);
			break;
		}
		LOGD("find decoder width %d height %d format %d",
			mpCodecContext->width, mpCodecContext->height,
			mpCodecContext->pix_fmt);
		if (mpCodecContext->width < 0 || mpCodecContext->height < 0 ||
			mpCodecContext->pix_fmt < 0) {
			LOGE("find decoder failed");
			break;
		}
		// open codec
		int res = avcodec_open2(this->mpCodecContext, this->mpCodec, NULL);
		if (res < 0) {
			LOG(LOG_LEVEL_E, "Cannot open codec: %d\n", res);
			break;
		}

		// open decode thread
		this->needContinue = true;
		this->mThread = shared_ptr<Thread>(new Thread(this));
		this->mThread->Start();
		return 0;
	} while (0);

	Close();
	return -1;
}

void Stream::Close()
{
	// stop decode thread
	this->needContinue = false;
	mpktQueue.NotifyAll();
	if (NULL != mThread.get())
		this->mThread->Join();

	if (NULL != this->mpCodecContext)
		avcodec_close(this->mpCodecContext);
}

void Stream::Run()
{
	LOG(LOG_LEVEL_D, "On stream thread run");

	while (this->needContinue) {
		shared_ptr<MyAVPacket> packet = mpktQueue.Get();
		if (NULL == packet.get())
			continue;

		AVPacket *ppacket = &packet->GetPacket();

		shared_ptr<MyAVFrame> frame(new MyAVFrame());
		AVFrame *pframe = &frame->GetAVFrame();
		int frameFinished = 0;

		uint64_t beforeDecodeTick = Analysiser::GetTick();
		Analysiser anl;
		// decode
		avcodec_decode_video2(this->mpCodecContext, pframe,
			&frameFinished, ppacket);
		if (frameFinished <= 0)
			continue;

		// copy data
		frame->UpdateData();

		// TODO set play time
#if 0
		system_clock::time_point playTime = system_clock::now() +
			milliseconds(ppacket->rtsp_sync_clock - Analysiser::GetTick());
		frame->SetPlayTime(playTime);
#endif

		// notify
		if (NULL != this->mpStreamNotify)
			this->mpStreamNotify->OnFrame(*this, this->mpStreamNotifyTag,
			packet->GetPacket(), frame);
	}
}

void Stream::Flush()
{

}

int Stream::FillPacket(shared_ptr<MyAVPacket> packet) {
	mpktQueue.Put(packet);

	return 0;
}

}
}
