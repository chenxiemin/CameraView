#include <string>

#include "stream.h"
#include "player.h"
#include "thread.h"

using namespace std;
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

		/*
		// open decode thread
		this->needContinue = true;
		this->mThread = shared_ptr<Thread>(new Thread(this));
		this->mThread->Start();
		*/
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

void Stream::Flush()
{

}

void Stream::Run()
{

}

}
}
