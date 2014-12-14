#include <chrono>
#include <map>

#include "multi-player.h"
#include "scaler.h"
#include "stream.h"
#include "recorder.h"
#include "string-util.h"

using namespace std;
using namespace std::chrono;
using namespace cxm::sdl;
using namespace cxm::util;

namespace cxm {
namespace av {

int MultiPlayer::Play(const string &url)
{
	vector<string> urls;
	StringUtil::Split(url, ' ', urls);

	for (auto iter = urls.begin(); iter != urls.end(); iter++) {
		shared_ptr<OnePlayer> onePlayer(new OnePlayer());
		
		shared_ptr<Player> player = shared_ptr<Player>(new Player());
		player->SetPlayerProcdule(this, onePlayer.get());
		player->Open(*iter, "");

		onePlayer->mplayer = player;
		onePlayer->mid = mopenCount++;
		onePlayer->mindex = onePlayer->mid;
		this->mplayerList.push_back(onePlayer);
	}

	return 0;
}

void MultiPlayer::Close()
{
	// close recording
	for (auto iter = mrecorderList.begin(); iter != mrecorderList.end(); iter++)
		(*iter)->Stop();
	mrecorderList.clear();
	// close player
	for (auto iter = this->mplayerList.begin();
		iter != mplayerList.end(); iter++)
			(*iter)->mplayer->Close();
	mplayerList.clear();
}

int MultiPlayer::Record(const std::string &fileName, int channel, int time)
{
	LOGD("Record with name %s channel %d time %d", fileName.c_str(), channel, time);
	if (fileName.length() == 0 || channel < 0 ||
		channel >= (int)mplayerList.size() || time < 0) {
		LOGE("Record: Illegal argument");
		return -1;
	}

	// find player
	shared_ptr<OnePlayer> onePlayer;
	for (auto iter = this->mplayerList.begin(); iter != mplayerList.end(); iter++)
		if ((*iter)->mid == channel)
			onePlayer = *iter;
	if (NULL == onePlayer.get()) {
		LOGE("Cannot find player by id: %d", channel);
		return -1;
	}

	// start record TODO fix hard code
	shared_ptr<Recorder> recorder(new Recorder(onePlayer->mplayer));
	mrecorderList.push_back(recorder);
	return recorder->Start(fileName + ".mp4", time);
}

void MultiPlayer::StopRecord(int playerId)
{
	for (auto oneIter = this->mplayerList.begin();
		oneIter != mplayerList.end(); oneIter++) {
		if (playerId != (*oneIter)->mid)
			continue;

		for (auto iter = this->mrecorderList.begin();
			iter != mrecorderList.end(); iter++) {
			if ((*oneIter)->mplayer.get() != (*iter)->GetPlayer().get())
				continue;

			(*iter)->Stop();
			mrecorderList.erase(iter);
			return;
		}
	}
}

bool MultiPlayer::IsRecord(int playerId)
{
	for (auto oneIter = this->mplayerList.begin();
		oneIter != mplayerList.end(); oneIter++) {
		if (playerId != (*oneIter)->mid)
			continue;

		for (auto iter = this->mrecorderList.begin();
			iter != mrecorderList.end(); iter++) {
			if ((*oneIter)->mplayer.get() != (*iter)->GetPlayer().get())
				continue;

			return true;
		}
	}

	return false;
}

void MultiPlayer::OnKeyDown(const SDL_Event &event)
{
	LOGD("On key down: %d", event.key.keysym.sym);
	static bool isInRecording = false;
	static int recordChannel = -1;

	switch (event.key.keysym.sym) {
	case SDLK_r: {
		if (isInRecording) {
			if (-1 != recordChannel) {
				 int res = Record("test", recordChannel, 10);
				 if (0 != res)
					 LOGE("Cannot record: %d", res);
			}
		}
		recordChannel = -1;
		isInRecording = !isInRecording;
		break;
	}
    case SDLK_0:
	case SDLK_1:
	case SDLK_2:
	case SDLK_3:
	case SDLK_4:
	case SDLK_5:
	case SDLK_6:
	case SDLK_7:
	case SDLK_8:
	case SDLK_9: {
		if (isInRecording) {
		    if (-1 == recordChannel)
		   	 recordChannel = event.key.keysym.sym - SDLK_0;
		    else
		   	 recordChannel = recordChannel * 10 +
		   		 event.key.keysym.sym - SDLK_0;
		}
		break;
	} case SDLK_d: {
		 this->ItermDisplayGrid();
		 break;
    } case SDLK_LEFT: {
		this->PageLeft();
		break;
    } case SDLK_RIGHT: {
		this->PageRight();
		break;
    }
	}
}

void MultiPlayer::OnMouseDown(const SDL_Event &event)
{
	switch (event.button.button)
	{
	case SDL_BUTTON_RIGHT: {
		// calculate index clicked
		int grid = this->mmerger.GetDisplayGrid();
		int hindex = event.button.x / (GetWidth() / grid);
		int vindex = event.button.y / (GetHeight() / grid);
		int index = vindex * grid + hindex + mmerger.GetStartPage() * grid;
		// show
		this->ShowSingle(index);
		LOGD("Mouse right button click at %d %d, go to index %d",
			event.button.x, event.button.y, index);
		break;
	} default:
		break;
	}
}

void MultiPlayer::OnTimer(void *opaque)
{
	this->AddTimer(0, this, NULL); // poll
	
	shared_ptr<MyAVPicture> picture(new MyAVPicture(PIXEL_FORMAT, GetWidth(), GetHeight()));
	map<int, shared_ptr<MyAVFrame>> framesMap;
	for (auto iter = mplayerList.begin(); iter != mplayerList.end(); iter++) {
		shared_ptr<OnePlayer> onePlayer = *iter;
		shared_ptr<MyAVFrame> frame = onePlayer->mqueue.Get(0, milliseconds(10));
		if (NULL == frame.get())
			frame = onePlayer->mcachedFrame;
		else
			onePlayer->mcachedFrame = frame;
		framesMap[onePlayer->mindex] = frame;
	}
	mmerger.Merge(framesMap, picture);

	this->ShowFrame(picture);
}

int MultiPlayer::OnPlayerProcdule(Player &player, void *procduleTag,
	CXM_PLAYER_EVENT event, shared_ptr<object> args)
{
	// LOGD("In function %s: with player event %d", __FUNCTION__, event);
	switch (event)
	{
	case CXM_PLAYER_EVENT_OPENED: {

		this->AddTimer(0, this, NULL);
		break;
	} case CXM_PLAYER_EVENT_STREAM_OPENED: {
		assert(NULL != args.get());
		// create scaler
		OnePlayer *onePlayer = (OnePlayer *)procduleTag;
		assert(NULL != onePlayer);

		Stream *pstream = (Stream *)args.get();
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

void MultiPlayer::ItermDisplayGrid()
{
	// change display grid with column and row
	int newGrid = (mmerger.GetDisplayGrid()) % mmerger.GetMaxGrid() + 1;
	mmerger.SetDisplayGrid(newGrid);
}

void MultiPlayer::PageLeft()
{
	// page left
	int currentPage = mmerger.GetStartPage();
	if (currentPage <= 0) {
		LOGE("Current Page reach at beginning: %d", currentPage);
		return;
	}
	currentPage--;
	mmerger.SetStartPage(currentPage);
	LOGD("Set start page to: %d", mmerger.GetStartPage());
}

void MultiPlayer::PageRight()
{
	// page right
	int currentPage = mmerger.GetStartPage() + 1;
	int displayItems = mmerger.GetDisplayNums();
	if (currentPage * displayItems >= (int)mplayerList.size()) {
		LOGE("Current Page reach at end: %d %d",
			currentPage * displayItems, (int)mplayerList.size());
		return;
	}
	mmerger.SetStartPage(mmerger.GetStartPage() + 1);
	LOGD("Set merger start page to: %d", mmerger.GetStartPage());
}

void MultiPlayer::ShowSingle(int index)
{
	if (index < 0 || index >= (int)mplayerList.size()) {
		LOGE("index out of range: %d", index);
		return;
	}

	mmerger.SetDisplayGrid(1);
	mmerger.SetStartPage(index);
}

}
}
