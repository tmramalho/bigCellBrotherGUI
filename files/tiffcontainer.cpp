#include "tiffcontainer.h"

TiffContainer::TiffContainer()
{
	loaded = false;
	tif = NULL;
	curFrame = -1;
}

TiffContainer::~TiffContainer()
{
	if(tif != NULL) TIFFClose(tif);
}

void TiffContainer::openFile(std::string filename)
{

	if(loaded) {
		TIFFClose(tif);
		tif = NULL;
		loaded = false;
	}

	tif = TIFFOpen(filename.c_str(), "r");
	if (!tif) return;

	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
	std::cout << height << ", " << width << ", " << bpp << ", "  << spp << ", " << photo << std::endl;
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
}

cv::Mat TiffContainer::grabFrameNumber(int frameNum)
{
	if(frameNum == curFrame) return currentFrame;
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

	if(bpp == 16) {
		double min, max;
		cv::minMaxLoc(readFrame, &min, &max);
		double scale = 255 / (max-min);
		double b = - min * scale;
		readFrame.convertTo(currentFrame, CV_8U, scale, b);
	} else if(bpp == 8) {
		readFrame.copyTo(currentFrame);
	}

	return currentFrame;
}
