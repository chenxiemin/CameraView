#ifndef _CXM_IMAGE_MERGER_H_
#define _CXM_IMAGE_MERGER_H_

#include <map>
#include <memory>

#include "ffmpeg.h"

namespace cxm {
namespace av {

class ImageMerger
{
	private: int mrow;
	private: int mcolumn;
	private: int mstartPage;

	public: ImageMerger() : mrow(2), mcolumn(2), mstartPage(0) { }

	public: void Merge(std::map<int, std::shared_ptr<MyAVFrame>> inFrames,
			std::shared_ptr<MyAVPicture> outPicture);

    public: void SetDisplayGrid(int grid) { mrow = mcolumn = grid; }
    public: int GetDisplayGrid() { return mrow; }

};

}
}
#endif

