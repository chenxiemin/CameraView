#include "multi-player.h"
#include "scaler.h"
#include "stream.h"

using namespace std;
using namespace cxm::sdl;

namespace cxm {
namespace av {

int MultiPlayer::Play(const string &url)
{
	if (NULL != mplayer.get())
		return -1;
	mplayer = shared_ptr<Player>(new Player());
	mplayer->SetPlayerProcdule(this, NULL);
	mplayer->Open(url, "");
	return 0;
}

void MultiPlayer::Close()
{
	mplayer->Close();
}

int MultiPlayer::OnShowFrame()
{
	AVPacket packet;
	int frameFinished = 0;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index != videoStream)
			continue;

		AVFrame *pFrame = av_frame_alloc();
		avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
		if (!frameFinished)
			continue;

		shared_ptr<MyAVFrame> frame(new MyAVFrame());
		frame->SetFrame(pFrame);
		shared_ptr<MyAVPicture> picture(new MyAVPicture(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height));
		scaler->Scale(frame, picture);
		this->ShowFrame(picture);

		break;
	}

	return 0;
}

void MultiPlayer::OnTimer(void *opaque)
{
	OnShowFrame();
	this->AddTimer(0, this, NULL);
}

int MultiPlayer::OnPlayerProcdule(Player &player, void *procduleTag,
	CXM_PLAYER_EVENT event, void *eventArgs)
{
	LOGD("In function %s: with player event %d", __FUNCTION__, event);

	switch (event)
	{
	case CXM_PLAYER_EVENT_OPENED: {
		shared_ptr<Stream> stream = mplayer->GetStream(0);
		pFormatCtx = mplayer->GetContext();
		pCodecCtx = stream->GetContext();
		videoStream = 0;
		LOGD("Get codec context: %p", pCodecCtx);

		// create scaler
		scaler = Scaler::CreateScaler(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
			pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
		this->AddTimer(0, this, NULL);
		break;
	} default:
		break;
	}
	return 0;
}

}
}
