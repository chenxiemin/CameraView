/*
 * =====================================================================================
 *
 *       Filename:  dysdl.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/25/2014 05:59:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#ifndef _CXM_SDL_H_
#define _CXM_SDL_H_

#include <memory>
#include <chrono>
#include <list>
#include <mutex>
#include <string>

#include "ffmpeg.h"
#include "analysiser.h"

namespace cxm {
namespace sdl {

class ISDLTimer {
	public: virtual ~ISDLTimer() { }

	public: virtual void OnTimer(void *opaque) = 0;
};

class SDL
{
	struct SDLTimerTag {
		SDL 			*thiz;
		ISDLTimer *pcallback;
		void 				*opaque;
	};
	private: static const int CXM_SDL_TIMER_EVENT = SDL_USEREVENT + 2;

	private: int mwidth;
	private: int mheight; 
	private: PixelFormat mformat;
	private: void *mopaque;
	private: SDL_Window *mwindow;
	private: SDL_Renderer *mrender;
	private: SDL_Texture *mtexture;
	private: std::list<std::shared_ptr<SDLTimerTag>> mTimerTagList;
	private: cxm::util::Analysiser analysis;

	public: SDL(int width, int height, void *opaque);
	public: virtual ~SDL();

	public: void EventLoop();
	public: void PollEvent();
	public: void ShowFrame(std::shared_ptr<cxm::av::MyAVPicture> picture);
	public: void AddTimer(int delayMils, ISDLTimer *pcallback, void *opaque);

	public: virtual void OnMouseDown(const SDL_Event &event) { } // do nothing
	public: virtual void OnKeyDown(const SDL_Event &event) { } // do nothing

    public: int GetWidth() { return mwidth; }
    public: int GetHeight() { return mheight; }

	private: bool OpenScreen();
	private: void CloseScreen();
	private: int DispatchEvent(const SDL_Event &event);
	private: static uint32_t TimerHelper(uint32_t interval, void *opaque);
};

}

}

#endif
