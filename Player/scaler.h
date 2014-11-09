/*
 * =====================================================================================
 *
 *       Filename:  scaler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2014 05:30:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xieminchen (mtk), hs_xieminchen@mediatek.com
 *        Company:  Mediatek
 *
 * =====================================================================================
 */

#ifndef _CXM_SCALER_H
#define _CXM_SCALER_H
#include <memory>

#include "ffmpeg.h"

namespace cxm {
namespace av {

class Scaler {
    private: struct SwsContext *mpContext;
    private: int mwidth;
    private: int mheight;

    private: Scaler(int srcW, int srcH, PixelFormat srcFormat, int dstW,
                    int dstH, PixelFormat dstFormat, int flags,
                    SwsFilter *srcFilter, SwsFilter *dstFilter,
                    const double *param);
    public: ~Scaler();

	public: int Scale(std::shared_ptr<cxm::av::MyAVFrame> frame,
                    std::shared_ptr<cxm::av::MyAVPicture> pict);

    public: int Scale(const uint8_t *const srcSlice[], const int srcStride[],
                    int srcSliceY, int srcSliceH, uint8_t *dist[],
                    const int distStride[]);
    public: static std::shared_ptr<Scaler> CreateScaler(int srcW, int srcH,
                    PixelFormat srcFormat, int dstW,
                    int dstH, PixelFormat dstFormat, int flags,
                    SwsFilter *srcFilter, SwsFilter *dstFilter,
                    const double *param)
    {
        std::shared_ptr<Scaler> ptr(new Scaler(srcW, srcH, srcFormat,
                    dstW, dstH, dstFormat, flags, srcFilter, dstFilter, param));
        return ptr;
    }
};
}
}
#endif

