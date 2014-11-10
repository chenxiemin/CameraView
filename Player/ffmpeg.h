/*
 * =====================================================================================
 *
 *       Filename:  ffmpeg.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/03/2014 09:45:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#ifndef _FFMPEG_H_
#define _FFMPEG_H_

#include <assert.h>
#include <memory>
#include <mutex>

#include "ffmpeg-inc.h"
#include "log.h"

namespace cxm {
namespace av {

class MyAVObject
{
    private: unsigned int mid;
    private: std::mutex mmutex;

    public: MyAVObject()
    {
        static unsigned int count = 0;

        // new id
        std::lock_guard<std::mutex> lock(mmutex);
        count++;
        mid = count;
    }

    public: virtual ~MyAVObject() { }

    public: unsigned int GetId()
    {
        return mid;
    }
    public: void SetId(unsigned int id)
    {
        mid = id;
    }
};

class MyAVPicture : MyAVObject
{
	private: AVPicture mpicture;
    private: uint8_t *mpbuffer;
	private: int mwidth;
	private: int mheight;
	private: AVPixelFormat mformat;
	private: int mbufferSize;

    public: MyAVPicture(enum AVPixelFormat pix_fmt, int width, int height)
    {
		mbufferSize = avpicture_get_size(pix_fmt, width, height);
        mpbuffer = (uint8_t *)av_malloc(mbufferSize);
        assert(NULL != mpbuffer);

        avpicture_fill(&mpicture, mpbuffer, pix_fmt, width, height);
		mformat = pix_fmt;
		mwidth = width;
		mheight = height;
    }
    public: ~MyAVPicture()
    {
		av_free(mpbuffer);
    }

    public: AVPicture &GetPicture() { return mpicture; }
	public: int GetWidth() { return mwidth; }
	public: int GetHeight() { return mheight; }
	public: int GetBufferSize() { return mbufferSize; }
};

class MyAVPacket : public MyAVObject
{
    private: AVPacket *mpacket;
    private: uint8_t *mpktbuf;

    public: MyAVPacket(uint8_t *ppktbuf, int pktlen);
    public: MyAVPacket(AVPacket *ppacket);

    public: ~MyAVPacket();

    public: AVPacket &GetPacket()
    {
        return *mpacket;
    }

    public: bool operator> (const MyAVPacket &rhs)
    {
        return true; // do not need to compare, so return directly
    }
};

class MyAVFrame : public MyAVObject
{
    private: std::chrono::system_clock::time_point mPlayTime;
    private: AVFrame *mpFrame;
    private: int width;
    private: int height;
    private: uint8_t *data[3];
    private: int linesize[3];

    public: MyAVFrame();
    public: MyAVFrame(AVPixelFormat fmt, int w, int h);
    public: ~MyAVFrame();

    public: void SetFrame(AVFrame *frame)
    {
        if (NULL != mpFrame)
            av_free(mpFrame);
        mpFrame = frame;
    }

    public: void UpdateData();

    public: void SetPlayTime(const std::chrono::system_clock::time_point &time)
    {
        this->mPlayTime = time;
    }

    public: std::chrono::system_clock::time_point GetPlayTime()
    {
        return this->mPlayTime;
    }

    public: std::chrono::system_clock::time_point GetSyncTime()
    {
        return mPlayTime;
    }

    public: AVFrame &GetAVFrame() { return *(this->mpFrame); }
    public: int GetWidth() { return width; }
    public: int GetHeight() { return height; }
			
    public: uint8_t **GetData()
    {
        if (NULL != data[0])
            return data;

        return mpFrame->data;
    }
    public: int *GetLinesize()
    {
        if (0 != linesize[0])
            return linesize;
        return mpFrame->linesize;
    }

	/*
    public: std::shared_ptr<MyAVFrame> Clone()
    {
        MyAVFrame *pFrame = new MyAVFrame();
        pFrame->width = this->width;
        pFrame->height = this->height;
        pFrame->data[0] = new uint8_t[this->linesize[0] * this->height];
        pFrame->data[1] = new uint8_t[this->linesize[1] * this->height / 2];
        pFrame->data[2] = new uint8_t[this->linesize[2] * this->height / 2];
        memcpy(pFrame->data[0], this->data[0], this->linesize[0] *
                this->height);
        memcpy(pFrame->data[1], this->data[1], this->linesize[1] *
                this->height / 2);
        memcpy(pFrame->data[2], this->data[2], this->linesize[2] *
                this->height / 2);
        pFrame->linesize[0] = this->linesize[0];
        pFrame->linesize[1] = this->linesize[1];
        pFrame->linesize[2] = this->linesize[2];

        std::shared_ptr<MyAVFrame> cloneFrame(pFrame);
        return cloneFrame;
    }

	void Resize(int rw, int rh)
	{
		width = rw;
		height = rh;

		linesize[0] = rw + 32;
		linesize[1] = rw / 2 + 16;
		linesize[2] = rw / 2+ 16;
		if (NULL != data[0])
			delete []data[0];
		if (NULL != data[1])
			delete []data[1];
		if (NULL != data[2])
			delete []data[2];
		data[0] = new uint8_t[linesize[0] * height];
		data[1] = new uint8_t[linesize[1] * height / 2];
		data[2] = new uint8_t[linesize[2] * height / 2];
		memset(data[0], 0, linesize[0] * height);
		memset(data[1], 0, linesize[1] * height / 2);
		memset(data[2], 0, linesize[2] * height / 2);
	}

    public: bool operator> (const MyAVFrame &rhs)
    {
        return mPlayTime > rhs.mPlayTime;
    }
	*/
};

}
}

#endif

