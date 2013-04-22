#include "videocontainer.h"

VideoContainer::VideoContainer()
{
	loaded = false;
}

void VideoContainer::openFile(std::string filename)
{

	if(loaded) {
		capture.release();
		loaded = false;
	}
	capture.open(filename);

	if (!capture.isOpened()) {
		return;
	}

	loaded = true;

	frameSize.height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	frameSize.width  = capture.get(CV_CAP_PROP_FRAME_WIDTH);

	rate = capture.get(CV_CAP_PROP_FPS);
	fourcc = capture.get(CV_CAP_PROP_FOURCC);
	numFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
	curFrame = 0;
}

cv::Mat VideoContainer::grabFrameNumber(int frameNum)
{
	curFrame = frameNum;
	cv::Mat image;

	capture.set(CV_CAP_PROP_POS_FRAMES, curFrame);
	capture.read(image);

	return image;
}
