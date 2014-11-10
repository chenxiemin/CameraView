#include <chrono>
#include <map>

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
	vector<string> urls;
	Split(url, ' ', urls);

	for (auto iter = urls.begin(); iter != urls.end(); iter++) {
		shared_ptr<OnePlayer> onePlayer(new OnePlayer());
		
		shared_ptr<Player> player = shared_ptr<Player>(new Player());
		player->SetPlayerProcdule(this, onePlayer.get());
		player->Open(*iter, "");

		onePlayer->mplayer = player;
		onePlayer->mid = mopenCount++;
		this->mplayerList.push_back(onePlayer);
	}

	return 0;
}

void MultiPlayer::Close()
{
	for (auto iter = this->mplayerList.begin();
		iter != mplayerList.end(); iter++)
			(*iter)->mplayer->Close();
}

void MultiPlayer::OnTimer(void *opaque)
{
	this->AddTimer(0, this, NULL); // poll
	
	shared_ptr<OnePlayer> onePlayer = this->mplayerList.back();

	// boocked get frame within 100ms
	shared_ptr<MyAVFrame> frame = onePlayer->mqueue.Get(0, milliseconds(100));
	if (NULL == frame.get())
		return;
	shared_ptr<MyAVPicture> picture(new MyAVPicture(PIXEL_FORMAT, GetWidth(), GetHeight()));
#if 0
	onePlayer->mscaler->Scale(frame, picture);
#else
	map<int, shared_ptr<MyAVFrame>> framesMap;
	framesMap[0] = frame;
	mmerger.Merge(framesMap, picture);
#endif

	this->ShowFrame(picture);
}

int MultiPlayer::OnPlayerProcdule(Player &player, void *procduleTag,
	CXM_PLAYER_EVENT event, void *eventArgs)
{
	LOGD("In function %s: with player event %d", __FUNCTION__, event);

	switch (event)
	{
	case CXM_PLAYER_EVENT_OPENED: {

		this->AddTimer(0, this, NULL);
		break;
	} case CXM_PLAYER_EVENT_STREAM_OPENED: {
		assert(NULL != eventArgs);
		// create scaler
		OnePlayer *onePlayer = (OnePlayer *)procduleTag;
		assert(NULL != onePlayer);

		Stream *pstream = (Stream *)eventArgs;
		onePlayer->mscaler = Scaler::CreateScaler(pstream, GetWidth(), GetHeight());
		pstream->SetStreamNotify(this, onePlayer);
		break;
	} default:
		break;
	}
	return 0;
}

void MultiPlayer::OnFrame(Stream &stream, void *tag, AVPacket &packet,
                    shared_ptr<MyAVFrame> frame)
{
	assert(NULL != tag);

	OnePlayer *onePlayer = (OnePlayer *)tag;
	onePlayer->mqueue.Put(frame);
}

void MultiPlayer::Split(const string &str, char spliter,
	vector<string> &container)
{
	container.clear();

	size_t offsetStart = 0;
	size_t offsetEnd = 0;
	while (string::npos != (offsetEnd = str.find(spliter, offsetStart))) {
		string sub = str.substr(offsetStart, offsetEnd);
		if (sub.length() > 0)
			container.push_back(sub);
		offsetStart = offsetEnd + 1;
	}
	string sub = str.substr(offsetStart, offsetEnd);
	if (sub.length() > 0)
		container.push_back(sub);
}

}
}
