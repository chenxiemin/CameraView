#include <Windows.h>

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

const char *url = "rtsp://192.168.0.130/12";
// const char *url = "rtsp://192.168.0.10/live";


shared_ptr<MultiPlayer> sdl;


int SDL_main(HWND handle, const RECT &rc)
{
	sdl = shared_ptr<MultiPlayer>(new MultiPlayer(rc.right, rc.bottom, handle));
	sdl->Play(url);

	return 0;
}

extern "C" __declspec(dllexport) int __stdcall SdlOpen(int handler, int x, int y)
{
	RECT rect;
	rect.right = x;
	rect.bottom = y;

	LOGD("Open sdl");
	return SDL_main((HWND)handler, rect);
}

extern "C" __declspec(dllexport) void __stdcall SdlClose()
{
	sdl->Close();
	sdl.reset();
	LOGD("Exit sdl");
}

extern "C" __declspec(dllexport) void __stdcall SdlTimer()
{
	sdl->PollEvent();
}

extern "C" __declspec(dllexport) void __stdcall SdlMain()
{
	MultiPlayer player(640, 360, NULL);
	player.Play(url);

	player.EventLoop();
	player.Close();
}
