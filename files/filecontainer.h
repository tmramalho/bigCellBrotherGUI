#ifndef FILECONTAINER_H
#define FILECONTAINER_H

#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class FileContainer
{
public:
	FileContainer();
	virtual void openFile(std::string filename)=0;
	virtual cv::Mat grabFrameNumber(int frameNum)=0;

	bool isLoaded() { return loaded; }
	int getNumFrames() { return numFrames; }
	int getCurFrame() { return curFrame; }
	cv::Size getFrameSize() { return frameSize; }

protected:
	bool loaded;
	int rate;
	int fourcc;
	int numFrames;
	int curFrame;
	cv::Size frameSize;
};

#endif // FILECONTAINER_H
