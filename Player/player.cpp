#include "player.h"
#include "ffmpeg.h"
#include "log.h"
#include "stream.h"

using namespace std;
using namespace cxm::util;
using namespace std::chrono;

namespace cxm {
namespace av {

Player::Player()
{
	av_register_all();
}

Player::~Player()
{
}

int Player::Open(const string &url, const string &option)
{
	if (this->mThread.get() != NULL) {
		LOG(LOG_LEVEL_E, "Thread created\n");
		return -1;
	}

	this->mUrl = url;
	this->mOption = option;
	this->mThread = shared_ptr<Thread>(new Thread(this));
	this->mThread->Start();

	// put start event
	shared_ptr<PlayerEvent> openEvent(new PlayerEvent(PLAYER_EVENT_OPEN));
	meventQueue.Put(openEvent);

	return 0;
}

void Player::Close()
{
	// send close event
	shared_ptr<PlayerEvent> closeEvent(new PlayerEvent(PLAYER_EVENT_CLOSE));
	meventQueue.Put(closeEvent);

	// wait thread stop
	this->mThread->Join();
}

void Player::Run()
{
	LOG(LOG_LEVEL_D, "run player thread %s", mUrl.c_str());

	bool needContinue = true;
	// read frame
	while (needContinue)
	{
		shared_ptr<PlayerEvent> event = meventQueue.Get();

		switch (*event)
		{
		case PLAYER_EVENT_OPEN: {
			this->mStatus = CXM_PLAYER_PLAYING;
			int res = OnOpen();

			if (0 == res) {
				// set play status
				this->mStatus = CXM_PLAYER_PLAY;

				// starting get frame
				shared_ptr<PlayerEvent> aevent(
					new PlayerEvent(PLAYER_EVENT_READ_FRAME));
				meventQueue.Put(aevent);
			} else {
				mStatus = CXM_PLAYER_ERROR;

				// reopen
				shared_ptr<PlayerEvent> e(new PlayerEvent(PLAYER_EVENT_REOPEN));
				meventQueue.Put(e);
			}

			break;
		} case PLAYER_EVENT_CLOSE: {
			mStatus = CXM_PLAYER_STOPING;
			OnClose();
			mStatus = CXM_PLAYER_STOP;

			// stop thread
			needContinue = false;
			break;
		} case PLAYER_EVENT_REOPEN: {
			// close first
			OnClose();

			mStatus = CXM_PLAYER_PLAYING;

			// open
			int res = OnOpen();
			if (res == 0) {
				mStatus = CXM_PLAYER_PLAY;
				// put frame event
				shared_ptr<PlayerEvent> e(new PlayerEvent(
					PLAYER_EVENT_READ_FRAME));
				meventQueue.Put(e);
			} else {
				// continue open for a short time
				this_thread::sleep_for(milliseconds(1000));

				shared_ptr<PlayerEvent> e(new PlayerEvent(PLAYER_EVENT_REOPEN));
				meventQueue.Put(e);
			}
			break;
		} case PLAYER_EVENT_READ_FRAME: {
			assert(CXM_PLAYER_PLAY == mStatus);
			int res = OnFrame();
			if (0 == res) {
				// put again
				shared_ptr<PlayerEvent> e(new PlayerEvent(
					PLAYER_EVENT_READ_FRAME));
				meventQueue.Put(e);
			} else {
				shared_ptr<PlayerEvent> e(new PlayerEvent(PLAYER_EVENT_REOPEN));
				meventQueue.Put(e);
			}
			break;
		}
		}
	}
}

int Player::OnOpen()
{
	LOGE("In function %s with url %s", __FUNCTION__, mUrl.c_str());
    do
    {
		// TODO generate options by option
		// options
		AVDictionary *options = 0;
		av_dict_set(&options, "rtsp_transport", "tcp", 0);
		// av_dict_set(&options, "rtsp_transport", "udp", 0);
		av_dict_set(&options, "probesize", "10000", 0); // to fix video not sync problem
		// av_dict_set(&options, "fflags", "nobuffer", 0);
		// av_dict_set(&options, "analyzeduration", "50", 0);
		// av_dict_set(&options, "max_delay", "5", 0);
		av_dict_set(&options, "seekable", "0", 0);

		// context
		mpFormatContext = avformat_alloc_context();
		mpFormatContext->interrupt_callback.callback = InterruptCallback;
		mpFormatContext->interrupt_callback.opaque = this;
		mpFormatContext->flags |= AVFMT_FLAG_NONBLOCK;
		mpFormatContext->flags |= AVIO_FLAG_NONBLOCK;
		mInterruptAnlaysiser.Analysis(); // for interrupt open
		int res = avformat_open_input(&mpFormatContext, this->mUrl.c_str(), NULL, &options);
		if (0 != res) {
			LOGE("Cannot open file %s: %d\n", mUrl.c_str(), res);
			break;
		}
		if (NULL != options)
			av_dict_free(&options);
		// get stream info
		res = avformat_find_stream_info(mpFormatContext, NULL);
		if (0 != res) {
			LOG(LOG_LEVEL_E, "Cannot find stream info: %d\n", res);
			break;
		}
		// dump information
		av_dump_format(mpFormatContext, 0, mUrl.c_str(), 0);

		// open stream
        shared_ptr<Stream> pStream = OpenStream(AVMEDIA_TYPE_VIDEO, 0);
        if (NULL == pStream.get()) {
            LOGE("Cannot open stream");
            break;
        }
        // append stream
        this->mStreamList[0] = pStream;
        // fire stream notify
        if (NULL != this->mpPlayerProcdule)
            this->mpPlayerProcdule->OnPlayerProcdule(*this, this->mpProcduleTag,
                    CXM_PLAYER_EVENT_STREAM_OPENED, pStream.get());
        // flush cache
        pStream->Flush();

        // save status
		this->mStatus = CXM_PLAYER_PLAY;
        // fire opened
        if (NULL != this->mpPlayerProcdule)
            this->mpPlayerProcdule->OnPlayerProcdule(*this, this->mpProcduleTag,
                    CXM_PLAYER_EVENT_OPENED, NULL);

        LOG(LOG_LEVEL_D, "open url success: %s", this->mUrl.c_str());
        return 0;
    } while(0);

    OnClose();
    return -1;
}

void Player::OnClose()
{
	if (NULL == mpFormatContext)
		return;

    // close stream
    for (auto iter = this->mStreamList.begin();
            iter != this->mStreamList.end(); iter++)
        if (NULL != iter->second)
            iter->second->Close();

	// close format context
	avformat_close_input(&mpFormatContext);
	this->mpFormatContext = NULL;
}

int Player::OnFrame()
{
	this_thread::sleep_for(milliseconds(100));
	// LOGE("In function %s", __FUNCTION__);
#if 0
    // read frame
    Analysiser anal;
    shared_ptr<DYAVPacket> dyPacket = mplayerSource->ReadPacket();
    if (NULL == dyPacket.get()) {
        if (mretryCount > FRAME_RETRY_COUNT) {
            LOG(LOG_LEVEL_E, "Player %s retry %d times, reopen",
                    mUrl.c_str(), mretryCount);
            return -1;
        }

        // reopen
        LOG(LOG_LEVEL_W, "Retry for RTSP stream at %d time", mretryCount++);
        return 0;
    }

    // get stream
    AVPacket &packet = dyPacket->GetPacket();
    shared_ptr<Stream> pStream = this->mStreamList[packet.stream_index];
    if (NULL != pStream)
        // set packet
        if (0 != pStream->FillPacket(dyPacket))
            LOG(LOG_LEVEL_D, "Cannot fill packet");

    mretryCount = 0; // reset retry count
    // report
    int readFrameTime = anal.Analysis();
    mtotalReadCount++;
    mtotalReadTime += readFrameTime;
    if (readFrameTime > mmaxReadTime)
        mmaxReadTime = readFrameTime;
    if (readFrameTime > 40)
        mexceedReadCount++;
#endif

    return 0;
}

shared_ptr<Stream> Player::OpenStream(int streamType, int index)
{
	// get first stream
	int videoStream = -1;
	for (int i = 0; i < (int)mpFormatContext->nb_streams; i++)
	if (streamType == mpFormatContext->streams[i]->codec->codec_type) {
		// TODO ignore streamIndex for now
		videoStream = i;
		break;
	}
	if (-1 == videoStream) {
		LOG(LOG_LEVEL_E, "Cannot find video stream\n");
		return NULL;
	}

	// open
	shared_ptr<Stream> pStream(new Stream(*this, videoStream));
	int res = pStream->Open();
	if (0 != res) {
		LOG(LOG_LEVEL_E, "Cannot open stream at %d: %d", videoStream, res);
		pStream->Close();
		pStream.reset();
	}

	return pStream;
}

int Player::OnInterrupt()
{
	int delta = mInterruptAnlaysiser.Analysis();
	if (delta > INTERRUPT_TIME) {
		LOG(LOG_LEVEL_W, "Frame interrupt caused by time out: %d", delta);
		return 1;
	}
	mInterruptAnlaysiser.Revert(); // revert the time
	return 0;
}

int Player::InterruptCallback(void *ctx)
{
	assert(NULL != ctx);

	return ((Player *)ctx)->OnInterrupt();
}

}
}