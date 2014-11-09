/*
 * =====================================================================================
 *
 *       Filename:  ffmpeg_inc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2014 12:41:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#ifndef _FFMPEG_INC_H
#define _FFMPEG_INC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/avstring.h>
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_thread.h>
#include <SDL_render.h>

#ifdef __cplusplus
}
#endif

#endif
