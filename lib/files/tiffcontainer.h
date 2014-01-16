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
    TiffContainer(bool _rescale = true);
	~TiffContainer();
	void openFile(std::string filename);
	void closeFile();
    cv::Mat grabFrameNumber(int frameNum);

private:
    TIFF *tif;
    uint16 spp, bpp, photo;
    uint32 width, height;
	cv::Mat currentFrame;
    bool rescale;
};

#endif // TIFFCONTAINER_H
