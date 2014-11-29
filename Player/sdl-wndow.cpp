#ifdef _WIN32
#include <Windows.h>
#include <vld.h>
#endif

#include "mysdl.h"
#include "log.h"
#include "scaler.h"
#include "ffmpeg.h"
#include "player.h"
#include "stream.h"
#include "multi-player.h"

#ifdef _WIN32
#define EXPORT_HEAD __declspec(dllexport) __stdcall
#else
#define EXPORT_HEAD
#endif

using namespace std;
using namespace cxm::av;
using namespace cxm::sdl;

// const char *url = "rtsp://192.168.0.129/12 rtsp://192.168.0.128/12 rtsp://192.168.0.130/12 rtsp://192.168.0.131/12";
// const char *url = "rtsp://127.0.0.1/test.264 rtsp://127.0.0.1/test10.264";
const char *url = "rtsp://192.168.0.11/live-high";
shared_ptr<MultiPlayer> sdl;

extern "C" EXPORT_HEAD int SdlOpen(void *handler, int x, int y)
{
	LOGD("Open sdl");
	sdl = shared_ptr<MultiPlayer>(new MultiPlayer(x, y, handler));
	sdl->Play(url);

    return 0;
}

extern "C" EXPORT_HEAD void SdlClose()
{
	sdl->Close();
	sdl.reset();
	LOGD("Exit sdl");
}

extern "C" EXPORT_HEAD void SdlTimer()
{
	sdl->PollEvent();
}

extern "C" EXPORT_HEAD void SdlMain()
{
	MultiPlayer player(640, 360, NULL);
	player.Play(url);

	player.EventLoop();
	player.Close();
}
