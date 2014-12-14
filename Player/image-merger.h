#ifndef _CXM_IMAGE_MERGER_H_
#define _CXM_IMAGE_MERGER_H_

#include <map>
#include <memory>

#include "ffmpeg.h"
#include "log.h"

namespace cxm {
namespace av {

class ImageMerger
{
	private: int mrow;
	private: int mcolumn;
	private: int mstartPage;
	private: int mmaxGrid;

	public: ImageMerger() : mrow(2), mcolumn(2), 
		mstartPage(0), mmaxGrid(5) { }

	public: void Merge(std::map<int, std::shared_ptr<MyAVFrame>> inFrames,
			std::shared_ptr<MyAVPicture> outPicture);

    public: void SetDisplayGrid(int grid)
    {
		if (grid < 1 || grid > mmaxGrid) {
			LOGE("grid out of range: %d", grid);
			return;
		}
        mrow = mcolumn = grid;
        mstartPage = 0; // need to reset the start page
    }

    public: void SetStartPage(int page)
	{
		if (page < 0) {
			LOGE("page index out of range: %d", page);
			return;
		}
		mstartPage = page;
	}

	public: int GetMaxGrid() { return mmaxGrid; }
	public: int GetDisplayGrid() { return mrow; }
    public: int GetStartPage() { return mstartPage; }
    public: int GetDisplayNums() { return mrow * mcolumn; }
};

}
}
#endif

