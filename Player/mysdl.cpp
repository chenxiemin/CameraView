/*
 * =====================================================================================
 *
 *       Filename:  mysdl.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/25/2014 06:03:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#include <assert.h>

#include "mysdl.h"
#include "log.h"

using namespace std;
using namespace cxm::av;

namespace cxm {
namespace sdl {
	SDL::SDL(int width, int height, void *opaque)
	{
		mwidth = width;
		mheight = height;
		mopaque = opaque;
		bool res = OpenScreen();
		assert(res);
	}

	SDL::~SDL()
	{
		CloseScreen();
	}

	bool SDL::OpenScreen()
	{
		// create screen
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER /* | SDL_INIT_NOPARACHUTE */) < 0) {
			LOG(LOG_LEVEL_E, "SDL init error");
			return false;
		}
		if (NULL != mopaque) {
			mwindow = SDL_CreateWindowFrom(this->mopaque);
			int width = 0;
			int height = 0;
			SDL_GetWindowSize(mwindow, &width, &height);
			LOGD("Get exist window size: %d %d", width, height);
		} else {
			mwindow = SDL_CreateWindow("SDLWindow", SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED, mwidth, mheight, SDL_WINDOW_SHOWN);
			LOGD("Create window with size: %d %d", mwidth, mheight);
		}
		if (NULL == mwindow) {
			LOGE("Cannot create window from: %p", this->mopaque);
			return false;
		}
		mrender = SDL_CreateRenderer(mwindow, 0, SDL_RENDERER_ACCELERATED);
		if (NULL == mrender) {
			LOGE("Cannot create render");
			return false;
		}
		this->mtexture = SDL_CreateTexture(mrender, SDL_PIXELFORMAT_YV12,
			SDL_TEXTUREACCESS_STREAMING, mwidth, mheight);
		if (NULL == mtexture) {
			LOGE("Cannot create texture with size %d %d", mwidth, mheight);
			return false;
		}
		return true;
	}

	void SDL::CloseScreen()
	{
		if (NULL != mtexture) {
			SDL_DestroyTexture(mtexture);
			mtexture = NULL;
		}
		if (NULL != mrender) {
			SDL_DestroyRenderer(mrender);
			mrender = NULL;
		}
		if (NULL != mwindow) {
			SDL_DestroyWindow(mwindow);
			mwindow = NULL;
		}
		SDL_Quit();
	}

	void SDL::ShowFrame(shared_ptr<MyAVPicture> picture)
	{
		analysis.Analysis();

		const AVPicture &pPicture = picture->GetPicture();
		SDL_UpdateYUVTexture(mtexture, NULL, pPicture.data[0], pPicture.linesize[0],
			pPicture.data[1], pPicture.linesize[1], pPicture.data[2], pPicture.linesize[2]);

		SDL_Rect		dst;
		dst.x = 0;
		dst.y = 0;
		dst.w = mwidth;
		dst.h = mheight;
		SDL_QueryTexture(mtexture, NULL, NULL, &dst.w, &dst.h);

		//refresh screen
		SDL_RenderClear(mrender);
		SDL_RenderCopy(mrender, mtexture, NULL, &dst);
		SDL_RenderPresent(mrender);
	}

	void SDL::EventLoop()
	{
		SDL_Event event;

		while (true) {
			SDL_WaitEvent(&event); // wait event

			if (-1 == this->DispatchEvent(event))
				break;
		}
	}

	void SDL::PollEvent()
	{
		SDL_Event event;
		if (1 != SDL_PollEvent(&event))
			return;

		this->DispatchEvent(event);
	}

	void SDL::PushEvent(SDL_Event &event)
	{
		SDL_PushEvent(&event);
	}

	int SDL::DispatchEvent(const SDL_Event &event)
	{
		if (SDL_QUIT == event.type) {
			LOG(LOG_LEVEL_I, "Get SDL_QUIT event, exiting...");
			return -1;
		} else if (SDL_MOUSEBUTTONDOWN == event.type) {
			this->OnMouseDown(event);
		} else if (SDL_KEYDOWN == event.type) {
			this->OnKeyDown(event);
		} else if (CXM_SDL_TIMER_EVENT == event.type) {
			SDLTimerTag *tag = (SDLTimerTag *)event.user.data1;

			// find whether tag validate
			auto iter = mTimerTagList.begin();
			for (; iter != mTimerTagList.end(); iter++)
				if (tag == iter->get())
					break;
			if (iter == mTimerTagList.end()) {
				LOG(LOG_LEVEL_E, "Invalid timer tag find at %p, ignore", tag);
				return 0;
			}

			// hold on the shared_ptr then remove in list
			shared_ptr<SDLTimerTag> timerPtr = *iter;
			mTimerTagList.erase(iter);

			// notify
			tag->pcallback->OnTimer(tag->opaque);
		}

		return 0;
	}

	void SDL::AddTimer(int delayMils, ISDLTimer *pcallback, void *opaque)
	{
		if (NULL == pcallback) {
			LOGE("NULL timer callback");
			return;
		}
		SDLTimerTag *tag = new SDLTimerTag();
		tag->thiz = this;
		tag->pcallback = pcallback;
		tag->opaque = opaque;

		// add to list
		mTimerTagList.push_back(shared_ptr<SDLTimerTag>(tag));
		tag->id = SDL_AddTimer(delayMils, TimerHelper, tag);
	}

	void SDL::RemoveTimer(ISDLTimer *pcallback)
	{
		if (NULL == pcallback)
			return;

		auto copyList = mTimerTagList;
		for (auto iter = copyList.begin(); iter != copyList.end(); iter++) {
			if ((*iter)->pcallback == pcallback) {
				SDL_RemoveTimer((*iter)->id); // stop timer
				mTimerTagList.erase(iter - copyList.begin() + mTimerTagList.begin()); // erase
			}
		}
	}

	uint32_t SDL::TimerHelper(uint32_t interval, void *opaque)
	{
		assert(NULL != opaque);

		SDL_Event event;
		event.type = CXM_SDL_TIMER_EVENT;
		event.user.data1 = opaque;
		SDL_PushEvent(&event);

		return 0;
	}
}
}
