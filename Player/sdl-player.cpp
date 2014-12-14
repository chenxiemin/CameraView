#include <assert.h>

#include "sdl-player.h"
#include "mysdl.h"
#include "log.h"
#include "scaler.h"
#include "ffmpeg.h"
#include "player.h"
#include "stream.h"
#include "multi-player.h"
#include "string-util.h"

using namespace std;
using namespace cxm::av;
using namespace cxm::sdl;
using namespace cxm::util;

shared_ptr<MultiPlayer> sdl;

extern "C" EXPORT_HEAD(int) SdlOpen(void *handler, int x, int y, const char *url)
{
	assert(NULL != sdl.get());
	LOGD("Open sdl");
	sdl = shared_ptr<MultiPlayer>(new MultiPlayer(x, y, handler));
	sdl->Play(url);

    return 0;
}

extern "C" EXPORT_HEAD(void) SdlClose()
{
	assert(NULL != sdl.get());
	sdl->Close();
	sdl.reset();
	LOGD("Exit sdl");
}

extern "C" EXPORT_HEAD(void) SdlTimer()
{
	assert(NULL != sdl.get());
	sdl->PollEvent();
}

extern "C" EXPORT_HEAD(void) SdlMain(int width, int height, const char *url)
{
	assert(NULL != sdl.get());
    assert(width > 0 && height > 0);

	MultiPlayer player(width, height, NULL);
	player.Play(url);

	player.EventLoop();
	player.Close();
}

extern "C" EXPORT_HEAD(int) SdlGetPlayerCount()
{
	assert(NULL != sdl.get());
	return sdl->GetPlayerList().size();
}

extern "C" EXPORT_HEAD(int) SdlGetPlayer(SdlPlayer *pplayer, int len)
{
	assert(NULL != sdl.get());
	assert(NULL != pplayer && NULL != pplayer->name);

	auto playerList = sdl->GetPlayerList();
	if (len < (int)playerList.size())
		return 0;

	int i = 0;
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++) {
		pplayer[i].id = (*iter)->mid;
		StringUtil::SafeCopyCString(pplayer[i].name, SDL_PLAYER_NAME_MAX_LEN,
			(*iter)->mplayer->GetURL().c_str());
		i++;
	}

	return playerList.size();
}

extern "C" EXPORT_HEAD(void) SdlIterDisplayGrid()
{
	assert(NULL != sdl.get());

	sdl->ItermDisplayGrid();
}

extern "C" EXPORT_HEAD(void) SdlPageLeft()
{
	assert(NULL != sdl.get());

	sdl->PageLeft();
}

extern "C" EXPORT_HEAD(void) SdlPageRight()
{
	assert(NULL != sdl.get());

	sdl->PageRight();
}

extern "C" EXPORT_HEAD(int) SdlRecording(const char *name, int channel, int time)
{
	assert(NULL != sdl.get());

	return sdl->Record(name, channel, time);
}

extern "C" EXPORT_HEAD(void) SdlStopRecording(int playerId)
{
	assert(NULL != sdl.get());

	return sdl->StopRecord(playerId);
}

extern "C" EXPORT_HEAD(int) SdlIsRecording(int playerId)
{
	assert(NULL != sdl.get());

	return (int)sdl->IsRecord(playerId);
}