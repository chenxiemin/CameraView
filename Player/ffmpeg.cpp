/*
 * =====================================================================================
 *
 *       Filename:  ffmpeg.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/02/2014 05:39:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#include "ffmpeg.h"

using namespace std;

namespace cxm {
namespace av {

MyAVPacket::MyAVPacket(AVPacket *ppacket)
{
    mpktbuf = NULL;
    mpacket = (AVPacket *)av_malloc(sizeof(AVPacket));
    *mpacket = *ppacket;
}

MyAVPacket::MyAVPacket(uint8_t *ppktbuf, int pktlen)
{
    mpktbuf = new uint8_t[pktlen];
    mpacket = (AVPacket *)av_malloc(sizeof(AVPacket));
    av_new_packet(mpacket, pktlen);

    memcpy(mpktbuf, ppktbuf, pktlen);
    mpacket->data = mpktbuf;
    mpacket->size = pktlen;
}

MyAVPacket::~MyAVPacket()
{
    if (NULL != mpacket) {
        av_free_packet(mpacket);
        av_free(mpacket);
    }
    if (NULL != mpktbuf) {
        delete []mpktbuf;
        mpktbuf = NULL;
    }
}

MyAVFrame::MyAVFrame() : width(0), height(0), mpFrame(NULL)
{
    mpFrame = av_frame_alloc();
    assert(NULL != mpFrame);

    for (int i = 0; i < 3; i++) {
        data[i] = NULL;
        linesize[i] = 0;
    }
}

MyAVFrame::~MyAVFrame()
{
    if (NULL != mpFrame) {
        av_free(this->mpFrame);
        this->mpFrame = NULL;
    }
    for (int i = 0; i < 3; i++)
        if (NULL != this->data[i])
            delete [](this->data[i]);
}

void MyAVFrame::UpdateData()
{
    width = this->mpFrame->width;
    height = this->mpFrame->height;
    data[0] = new uint8_t[this->mpFrame->linesize[0] * this->mpFrame->height];
    data[1] = new uint8_t[this->mpFrame->linesize[1] * this->mpFrame->height / 2];
    data[2] = new uint8_t[this->mpFrame->linesize[2] * this->mpFrame->height / 2];

    memcpy(data[0], mpFrame->data[0], mpFrame->linesize[0] *
            this->mpFrame->height);
    memcpy(data[1], mpFrame->data[1], mpFrame->linesize[1] *
            this->mpFrame->height / 2);
    memcpy(data[2], mpFrame->data[2], mpFrame->linesize[2] *
            this->mpFrame->height / 2);
    linesize[0] = mpFrame->linesize[0];
    linesize[1] = mpFrame->linesize[1];
    linesize[2] = mpFrame->linesize[2];
}

}
}
