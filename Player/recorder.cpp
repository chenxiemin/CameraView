#include "recorder.h"
#include "log.h"
#include "string-util.h"
#include "player.h"
#include "stream.h"

using namespace std;
using namespace cxm::util;

namespace cxm {
namespace av {

Recorder::Recorder(shared_ptr<Player> player) : mplayer(player), mcontext(NULL)
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

		// get recording stream
		int streamSize = mplayer->GetStreamSize();
		if (streamSize < 1) {
			LOGE("Cannot get stream size: %d", streamSize);
			break;
		}
		AVStream *pstream = mplayer->GetStream(0)->GetStream();
		AVCodecContext *streamContext = mplayer->GetStream(0)->GetContext();
		// new stream
		AVStream *outStream = avformat_new_stream(mcontext, NULL);
		if (NULL == outStream) {
			LOGE("Cannot open stream");
			break;
		}
		res = avcodec_copy_context(outStream->codec, streamContext);
		if (0 != res) {
			LOGE("Cannot copy context: %d", res);
			break;
		}
		outStream->sample_aspect_ratio.num = streamContext->sample_aspect_ratio.num;
		outStream->sample_aspect_ratio.den = streamContext->sample_aspect_ratio.den;
		outStream->r_frame_rate = pstream->r_frame_rate;
		outStream->avg_frame_rate = outStream->r_frame_rate;
		outStream->time_base = av_inv_q(outStream->r_frame_rate);
		outStream->codec->time_base = outStream->time_base;
		res = avformat_write_header(mcontext, NULL);
		if (0 != res) {
			LOGE("Cannot write header: %d", res);
			break;
		}
		av_dump_format(mcontext, 0, fileName.c_str(), 1);

		// register notify
		mplayer->SetPlayerProcdule(this, NULL);
		// start thread

		return 0;
	} while (0);

	Stop();
	return -1;
}

void Recorder::Stop()
{
	if (NULL == mcontext)
		return;

	int res = av_write_trailer(mcontext);
	if (0 != res)
		LOGE("Cannot write trailer: %d", res);
	res = avio_close(mcontext->pb);
	if (0 != res)
		LOGE("Cannot close avio: %d", res);
	avformat_free_context(mcontext);
	mcontext = NULL;
}

void Recorder::Run()
{
}
	
int Recorder::OnPlayerProcdule(Player &player, void *procduleTag,
		CXM_PLAYER_EVENT event, shared_ptr<object> args)
{
	switch (event) {
	case CXM_PLAYER_EVENT_GET_PACKET:
		LOGD("Get packet %p", args.get());
		break;
	}
	return 0;
}

}
}
