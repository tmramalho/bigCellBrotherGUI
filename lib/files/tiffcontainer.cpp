#include "tiffcontainer.h"

TiffContainer::TiffContainer(bool _rescale)
{
	loaded = false;
	tif = NULL;
	curFrame = -1;
	bpp = 0;
	height = 0;
	width = 0;
	spp = 0;
	photo = 0;
    vMin = 0;
    vMax = 65536;
    rescale = _rescale;
}

TiffContainer::~TiffContainer()
{
	if(tif != NULL) TIFFClose(tif);
}

void TiffContainer::openFile(std::string filename)
{
	this->closeFile();

	tif = TIFFOpen(filename.c_str(), "r");
	if (!tif) return;

	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
    //std::cout << height << ", " << width << ", " << bpp << ", "  << spp << ", " << photo << std::endl;
	frameSize.height = height;
	frameSize.width = width;

	int dircount = 0;
	do {
		dircount++;
	} while (TIFFReadDirectory(tif));

	numFrames = dircount;

	if(spp == 1 && photo == 1) loaded = true;
	else { // format not implemented
		TIFFClose(tif);
	}

    if(bpp == 16) isHDR = true;
    else isHDR = false;
    filepath = filename;
}

void TiffContainer::closeFile() {
	if(loaded) {
		TIFFClose(tif);
		tif = NULL;
		loaded = false;
	}
}

cv::Mat TiffContainer::grabFrameNumber(int frameNum)
{
    // if(frameNum == curFrame) return currentFrame;
	curFrame = frameNum;
	TIFFSetDirectory(tif, frameNum);
	cv::Mat readFrame(100, 100, CV_8U);

	if(spp == 1 && photo == 1) { // standard grayscale
		if(bpp == 16) readFrame.create(height, width, CV_16U);
		else if(bpp == 8) readFrame.create(height, width, CV_8U);
	}

	int nl = readFrame.rows;
	for (int j = 0; j < nl; j++) {
		if(bpp == 16) {
			uint16 *imageRow = readFrame.ptr<uint16>(j);
			TIFFReadScanline(tif, imageRow, j, 0);
		} else if(bpp == 8) {
			char *imageRow = readFrame.ptr<char>(j);
			TIFFReadScanline(tif, imageRow, j, 0);
		}
	}

    if(bpp == 16 && rescale) {
		double min, max;
		cv::minMaxLoc(readFrame, &min, &max);
        if(vMin > min) min = vMin;
        if(vMax < max) max = vMax;
		double scale = 255 / (max-min);
		double b = - min * scale;
        //std::cout << "Feeding rescaled frame " << min << " : " << max << std::endl;
		readFrame.convertTo(currentFrame, CV_8U, scale, b);
    } else {
        //std::cout << "Feeding raw frame" << std::endl;
		readFrame.copyTo(currentFrame);
	}

	return currentFrame;
}
