/*
 * =====================================================================================
 *
 *       Filename:  scaler.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2014 05:30:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#include <assert.h>

#include "scaler.h"
#include "log.h"

using namespace cxm::av;

namespace cxm {
namespace av {
    
Scaler::Scaler(int srcW, int srcH, PixelFormat srcFormat, int dstW, int dstH,
        PixelFormat dstFormat, int flags, SwsFilter *srcFilter,
        SwsFilter *dstFilter, const double *param)
{
    LOG(LOG_LEVEL_D, "create scaler with srcw %d, srch, %d, srcf %d, dstw %d, "\
            "dsth %d, dstf %d, flags %d, srcfilter %p, dstfilter %p, param %p",
            srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags,
            srcFilter, dstFilter, param);
    /*
    if (AV_PIX_FMT_NONE == srcFormat) {
        LOG(LOG_LEVEL_E, "Source Format unknown!");
        // srcFormat = AV_PIX_FMT_YUV420P;
    }
    */
    this->mpContext = sws_getContext(srcW, srcH, srcFormat, dstW, dstH,
            dstFormat, flags, srcFilter, dstFilter, param);
    assert(NULL != this->mpContext);
    mwidth = dstW;
    mheight = dstH;
}

Scaler::~Scaler()
{
    if (NULL != this->mpContext) {
        sws_freeContext(this->mpContext);
        this->mpContext = NULL;
    }
}

int Scaler::Scale(std::shared_ptr<MyAVFrame> frame,
        std::shared_ptr<MyAVPicture> pict)
{
    AVPicture &picture = pict->GetPicture();
    return Scale(frame->GetData(), frame->GetLinesize(), 0, mheight,
            picture.data, picture.linesize);
}

int Scaler::Scale(const uint8_t *const srcSlice[], const int srcStride[],
        int srcSliceY, int srcSliceH, uint8_t *dist[], const int distStride[])
{
    return sws_scale(this->mpContext, srcSlice, srcStride, srcSliceY,
            srcSliceH, dist, distStride);
}

}
}

