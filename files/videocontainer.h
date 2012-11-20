#ifndef VIDEOCONTAINER_H
#define VIDEOCONTAINER_H

#include "filecontainer.h"
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class VideoContainer : public FileContainer
{
public:
    VideoContainer();
	void openFile(std::string filename);
	cv::Mat grabFrameNumber(int frameNum);

private:
	cv::VideoCapture capture;
	cv::Size frameSize;
};

#endif // VIDEOCONTAINER_H
