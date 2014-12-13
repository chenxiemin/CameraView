#include <assert.h>

#include "sdl-player.h"
#include "mysdl.h"
#include "log.h"
#include "scaler.h"
#include "ffmpeg.h"
#include "player.h"
#include "stream.h"
#include "multi-player.h"

using namespace std;
using namespace cxm::av;
using namespace cxm::sdl;

shared_ptr<MultiPlayer> sdl;

extern "C" EXPORT_HEAD(int) SdlOpen(void *handler, int x, int y, const char *url)
{
	LOGD("Open sdl");
	sdl = shared_ptr<MultiPlayer>(new MultiPlayer(x, y, handler));
	sdl->Play(url);

    return 0;
}

extern "C" EXPORT_HEAD(void) SdlClose()
{
	sdl->Close();
	sdl.reset();
	LOGD("Exit sdl");
}

extern "C" EXPORT_HEAD(void) SdlTimer()
{
	sdl->PollEvent();
}

extern "C" EXPORT_HEAD(void) SdlMain(int width, int height, const char *url)
{
    assert(width > 0 && height > 0);

	MultiPlayer player(width, height, NULL);
	player.Play(url);

	player.EventLoop();
	player.Close();
}

