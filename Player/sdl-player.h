/*
 * =====================================================================================
 *
 *       Filename:  sdl-window.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/2014 10:14:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#ifndef _CXM_SDL_WINDOW_H_
#define _CXM_SDL_WINDOW_H_

#ifdef _WIN32
#include <Windows.h>
#include <vld.h>
#endif

#ifdef _WIN32
#define EXPORT_HEAD(type) __declspec(dllexport) type __stdcall
#else
#define EXPORT_HEAD(type) type
#endif

extern "C" EXPORT_HEAD(int) SdlOpen(void *handler, int x, int y, const char *url);
extern "C" EXPORT_HEAD(void) SdlClose();
extern "C" EXPORT_HEAD(void) SdlTimer();
extern "C" EXPORT_HEAD(void) SdlMain(int width, int height, const char *url);

#endif
