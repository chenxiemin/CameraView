#include "image-merger.h"

using namespace std;

namespace cxm {
namespace av {

void ImageMerger::Merge(map<int, shared_ptr<MyAVFrame>> inFrames,
		shared_ptr<MyAVPicture> outPicture)
{
	AVPicture &pic = outPicture->GetPicture();
	int picHeight = outPicture->GetHeight();
	int picWidth = outPicture->GetWidth();

	memset(pic.data[0], 0, pic.linesize[0] * picHeight);
	memset(pic.data[1], 0x80, pic.linesize[1] * picHeight / 2);
	memset(pic.data[2], 0x80, pic.linesize[1] * picHeight / 2);

	int perWidth = picWidth / mrow; // per block size
	int perHeight = picHeight / mcolumn;
	// y data
	for (int j = 0; j < picHeight; j++) {
		for (int i = 0; i < picWidth; i++) {
			// get the row and column
			int colIndex = j / perHeight;
			int rowIndex = i / perWidth;
			// get associate frame
			std::shared_ptr<MyAVFrame> curFrame = inFrames[
                colIndex * mrow + rowIndex + mstartPage * mcolumn * mrow];
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

	// uv data
	perWidth /= 2;
	perHeight /= 2;
	picWidth /= 2;
	picHeight /= 2;
	for (int j = 0; j < picHeight; j++) {
		for (int i = 0; i < picWidth; i++) {
			// get the row and column
			int colIndex = j / perHeight;
			int rowIndex = i / perWidth;
			// get associate frame
			std::shared_ptr<MyAVFrame> curFrame = inFrames[
                colIndex * mrow + rowIndex + mstartPage * mcolumn * mrow];
			if (NULL == curFrame.get())
				continue;

			// map color
			int perj = j % perHeight; // per block index
			int peri = i % perWidth;

			int frameWidth = curFrame->GetWidth() / 2;
			int frameHeight = curFrame->GetHeight() / 2;
			int scalej = (int)(((double)frameHeight / perHeight) * perj);
			int scalei = (int)(((double)frameWidth / perWidth) * peri);

			pic.data[1][j * picWidth + i] = curFrame->GetData()[1][scalej * curFrame->GetLinesize()[1] + scalei];
			pic.data[2][j * picWidth + i] = curFrame->GetData()[2][scalej * curFrame->GetLinesize()[2] + scalei];
		}
	}

#if 0
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

}
}
