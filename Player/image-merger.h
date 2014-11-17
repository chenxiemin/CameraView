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

	public: ImageMerger() : mrow(3), mcolumn(3), mstartPage(0) { }

	public: void Merge(std::map<int, std::shared_ptr<MyAVFrame>> inFrames,
			std::shared_ptr<MyAVPicture> outPicture)
	{
		AVPicture &pic = outPicture->GetPicture();
		int picHeight = outPicture->GetHeight();
		int picWidth = outPicture->GetWidth();

		memset(pic.data[0], 100, pic.linesize[0] * picHeight);
		memset(pic.data[1], 0x80, pic.linesize[1] * picHeight / 2);
		memset(pic.data[2], 0x80, pic.linesize[1] * picHeight / 2);

		int perWidth = picWidth / mrow; // per block size
		int perHeight = picHeight / mcolumn;
#if 1
		for (int j = 0; j < picHeight; j++) {
			for (int i = 0; i < picWidth; i++) {
				// get the row and column
				int colIndex = j / perHeight;
				int rowIndex = i / perWidth;
				// get associate frame
				std::shared_ptr<MyAVFrame> curFrame = inFrames[colIndex * mrow + rowIndex];
				if (NULL == curFrame.get())
					continue;

				// map color
				int perj = j % perHeight; // per block index
				int peri = i % perWidth;

				int frameWidth = curFrame->GetWidth();
				int frameHeight = curFrame->GetHeight();
				int scalej = (int)(((double)frameHeight / perHeight) * perj);
				int scalei = (int)(((double)frameWidth / perWidth) * peri);
				pic.data[0][j * picWidth + i] = curFrame->GetData()[0][scalej * curFrame->GetLinesize()[0] + scalei];
			}
		}
#else
		std::shared_ptr<MyAVFrame> curFrame = inFrames[0];
		if (NULL == curFrame.get())
			return;

		uint8_t *inData0 = curFrame->GetData()[0];
		uint8_t *outData0 = pic.data[0];

		for (int j = 0; j < picHeight; j++) {
			for (int i = 0; i < picWidth; i++) {
				outData0[j * picWidth + i] = inData0[j * curFrame->GetLinesize()[0] + i];
			}
		}
#endif
	}
};

}
}
#endif
