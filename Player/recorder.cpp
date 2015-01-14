#include "recorder.h"
#include "log.h"
#include "string-util.h"
#include "player.h"
#include "stream.h"

using namespace std;
using namespace cxm::util;

namespace cxm {
namespace av {

Recorder::Recorder(shared_ptr<Player> player) :
	mplayer(player), mcontext(NULL), misRun(false), mpframe(NULL), moutStream(NULL)
{
}

Recorder::~Recorder()
{
	if (NULL != mcontext)
		Stop();
}

int Recorder::Start(const string &fileName, int recordTime)
{
	if (NULL != mcontext) {
		LOGE("Already start");
		return -1;
	}
	LOGD("Start record player: %s %d", fileName.c_str(), recordTime);

	do {
		// check recording stream
		int streamSize = mplayer->GetStreamSize();
		if (streamSize < 1) {
			LOGE("Cannot get stream size: %d", streamSize);
			break;
		}
		
		// format
		vector<string> suffixes;
		StringUtil::Split(fileName, '.', suffixes);
		AVOutputFormat *format = av_guess_format(suffixes.back().c_str(), NULL, NULL);
		if (NULL == format) {
			LOGE("Cannot guess format by suffix: %s", suffixes.back().c_str());
			break;
		}

		// context
		mcontext = avformat_alloc_context();
		if (NULL == mcontext) {
			LOGE("Cannot alloc format context");
			break;
		}
		mcontext->oformat = format;
		int res = avio_open2(&mcontext->pb, fileName.c_str(), AVIO_FLAG_WRITE, NULL, NULL);
		if (res < 0) {
			LOGE("Cannot open avio: %d", res);
			break;
		}

		AVStream *pstream = mplayer->GetStream(0)->GetStream();
		AVCodecContext *streamContext = mplayer->GetStream(0)->GetContext();
		// new stream
		moutStream = avformat_new_stream(mcontext, NULL);
		if (NULL == moutStream) {
			LOGE("Cannot open stream");
			break;
		}
		res = avcodec_copy_context(moutStream->codec, streamContext);
		if (0 != res) {
			LOGE("Cannot copy context: %d", res);
			break;
		}
#if 0
		moutStream->sample_aspect_ratio.num = streamContext->sample_aspect_ratio.num;
		moutStream->sample_aspect_ratio.den = streamContext->sample_aspect_ratio.den;
		moutStream->r_frame_rate = pstream->r_frame_rate;
		moutStream->avg_frame_rate = moutStream->r_frame_rate;
		moutStream->time_base = av_inv_q(moutStream->r_frame_rate);
		moutStream->codec->time_base = moutStream->time_base;
#endif
		moutStream->time_base = pstream->time_base;
		moutStream->codec->time_base = pstream->time_base;
		res = avformat_write_header(mcontext, NULL);
		if (0 != res) {
			LOGE("Cannot write header: %d", res);
			break;
		}
		av_dump_format(mcontext, 0, fileName.c_str(), 1);

		// alloc frame for decode
		mpframe = av_frame_alloc();
		// register notify
		mplayer->SetPlayerProcdule(this, NULL);
		// start thread
		misRun = true;
		mthread = shared_ptr<Thread>(new Thread(this, "recorder"));
		mthread->Start();

		return 0;
	} while (0);

	Stop();
	return -1;
}

void Recorder::Stop()
{
	if (NULL == mcontext)
		return;

	LOGI("Stop recorder at: %s", mplayer->GetURL().c_str());
	// unregist notify
	mplayer->RemovePlayerProcdule(this);

	// stop thread
	if (NULL != mthread.get()) {
		misRun = false;
		msafeQueue.NotifyAll();
		mthread->Join();
		mthread.reset();
	}

	if (NULL != mcontext->pb) {
		if (NULL != moutStream) {
			int res = av_write_trailer(mcontext);
			if (0 != res)
				LOGE("Cannot write trailer: %d", res);
		}
		int res = avio_close(mcontext->pb);
		if (0 != res)
			LOGE("Cannot close avio: %d", res);
	}

	avformat_free_context(mcontext);
	mcontext = NULL;
	if (NULL != mpframe) {
		av_free(mpframe);
		mpframe = NULL;
	}
}

void Recorder::Run()
{
	int res = 0;
	bool isKey = false;
	int64_t startPts = 0;

	// decode to get the first key frame
	AVCodec *pcodec = avcodec_find_decoder(CODEC_ID_H264);
	AVCodecContext *pcodecContext = avcodec_alloc_context3(pcodec);
	avcodec_open2(pcodecContext, pcodec, 0);

	while (misRun) {
		shared_ptr<MyAVPacket> packet = msafeQueue.Get();
		if (!misRun)
			return; // double check cause safe queue blocked

		AVPacket &pkt = packet->GetPacket();

		if (!isKey) {
			int gotPicture = -1;
			res = avcodec_decode_video2(pcodecContext, mpframe, &gotPicture, &pkt);
			if (1 == gotPicture) {
				LOGD("Packet flag: %d, got picture: %d, res: %d", pkt.flags, gotPicture, res);
				isKey = true;
				startPts = pkt.pts;
			}
		}
		if (!isKey)
			continue;

		AVPacket pkt2;
		av_init_packet(&pkt2);
		pkt2.stream_index = 0;
		pkt2.data = pkt.data;
		pkt2.size = pkt.size;
		pkt2.pts = pkt2.dts = (pkt.pts - startPts);
		// LOGD("recording pkt2 pts: %lld", pkt2.pts);
		
		res = av_interleaved_write_frame(mcontext, &pkt2);
		if (0 != res)
			LOGE("Cannot write frame: %d", res);
	}
}
	
int Recorder::OnPlayerProcdule(Player &player, void *procduleTag,
		CXM_PLAYER_EVENT event, shared_ptr<object> args)
{
	switch (event) {
	case CXM_PLAYER_EVENT_GET_PACKET:
		shared_ptr<MyAVPacket> packet = dynamic_pointer_cast<MyAVPacket>(args);
		if (NULL == packet.get()) {
			LOGE("Cannot convert packet");
			break;
		}

		msafeQueue.Put(packet);
		break;
	}
	return 0;
}

}
}
