#include <chrono>

#include "multi-player.h"
#include "scaler.h"
#include "stream.h"

using namespace std;
using namespace std::chrono;
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

void MultiPlayer::OnTimer(void *opaque)
{
	this->AddTimer(0, this, NULL); // poll
	
	// boocked get frame within 100ms
	shared_ptr<MyAVFrame> frame = this->mqueue.Get(0, milliseconds(100));
	if (NULL == frame.get())
		return;
	shared_ptr<MyAVPicture> picture(new MyAVPicture(PIXEL_FORMAT, GetWidth(), GetHeight()));
	scaler->Scale(frame, picture);
	this->ShowFrame(picture);
}

int MultiPlayer::OnPlayerProcdule(Player &player, void *procduleTag,
	CXM_PLAYER_EVENT event, void *eventArgs)
{
	LOGD("In function %s: with player event %d", __FUNCTION__, event);

	switch (event)
	{
	case CXM_PLAYER_EVENT_OPENED: {
		// create scaler
		shared_ptr<Stream> stream = mplayer->GetStream(0);
		scaler = Scaler::CreateScaler(stream, GetWidth(), GetHeight());
		this->AddTimer(0, this, NULL);
		break;
	} case CXM_PLAYER_EVENT_STREAM_OPENED: {
		assert(NULL != eventArgs);
		Stream *pstream = (Stream *)eventArgs;
		pstream->SetStreamNotify(this, NULL);
		break;
	} default:
		break;
	}
	return 0;
}

void MultiPlayer::OnFrame(Stream &stream, void *tag, AVPacket &packet,
                    shared_ptr<MyAVFrame> frame)
{
	this->mqueue.Put(frame);
}

}
}
