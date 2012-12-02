#ifndef TIFFCONTAINER_H
#define TIFFCONTAINER_H

#include "filecontainer.h"
#include "tiffio.h"
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class TiffContainer : public FileContainer
{
public:
	TiffContainer();
	~TiffContainer();
	void openFile(std::string filename);
	cv::Mat grabFrameNumber(int frameNum);

private:
	TIFF *tif;
	uint16 spp, bpp, photo;
	uint32 width, height;
	char *buf;
	cv::Mat currentFrame;
};

#endif // TIFFCONTAINER_H
