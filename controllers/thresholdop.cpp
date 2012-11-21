#include "thresholdop.h"

ThresholdOp::ThresholdOp()
{
	threshold = 0;
	window = 9;
	smooth = 3;
	invert = true;
	thresholdTH = 20;
	windowTH = 9;
	invertTH = false;
	thresholdBG = 0;
	windowBG = 9;
	smoothBG = 3;
	invertBG = false;
}

void ThresholdOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(2);
	cv::Mat thImage = ImageProcessor::adaptiveThreshold(prev, threshold, window, invert);

	if(smooth > 1) {
		thImage = ImageProcessor::applyMorphologyOp(thImage, cv::MORPH_CLOSE, smooth);
		thImage = ImageProcessor::applyMorphologyOp(thImage, cv::MORPH_OPEN, smooth);
	}
	cv::Mat thatImage = ImageProcessor::applyMorphologyOp(prev, cv::MORPH_TOPHAT, windowTH);
			thatImage = ImageProcessor::threshold(thatImage, thresholdTH, invertTH);

	cv::Mat blobsImage;
	cv::subtract(thImage, thatImage, blobsImage);

	cv::Mat bgImage = ImageProcessor::adaptiveThreshold(prev, thresholdBG, windowBG, invertBG);
	if(smoothBG > 1) {
		bgImage = ImageProcessor::applyMorphologyOp(bgImage, cv::MORPH_CLOSE, smoothBG);
		bgImage = ImageProcessor::applyMorphologyOp(bgImage, cv::MORPH_OPEN, smoothBG);
	}

	cv::subtract(blobsImage, bgImage, blobsImage);

	/*if(window < 31) {
		cv::Mat lap = ImageProcessor::laplacian(prev, window);
		cv::imshow("lalap", lap);
	}*/

	controller->setPipelineImage(3, blobsImage);
	controller->setPipelineImage(22, thImage);
	controller->setPipelineImage(23, thatImage);
	controller->setPipelineImage(24, bgImage);
}

void ThresholdOp::createPreview()
{
	std::vector<cv::Mat> channels;
	channels.push_back(controller->getPipelineImage(24));
	channels.push_back(controller->getPipelineImage(23));
	channels.push_back(controller->getPipelineImage(22));
	cv::Mat result(channels[0].size(), CV_8U);
	cv::merge(channels, result);
	controller->setPreview(result);
}

void ThresholdOp::updateThreshold(int th)
{
	threshold = th;
	perform();
}

void ThresholdOp::updateWindow(int wi)
{
	if(wi != 0 && wi % 2 == 0) wi +=1;
	window = wi;
	perform();
}

void ThresholdOp::updateSmoothing(int sm)
{
	if(sm != 0 && sm % 2 == 0) sm +=1;
	smooth = sm;
	perform();
}

void ThresholdOp::updateInvert(bool in)
{
	invert = in;
	perform();
}

void ThresholdOp::updateThresholdTH(int th)
{
	thresholdTH = th;
	perform();
}

void ThresholdOp::updateWindowTH(int wi)
{
	if(wi != 0 && wi % 2 == 0) wi +=1;
	windowTH = wi;
	perform();
}

void ThresholdOp::updateInvertTH(bool in)
{
	invertTH = in;
	perform();
}

void ThresholdOp::updateThresholdBG(int th)
{
	thresholdBG = th;
	perform();
}

void ThresholdOp::updateWindowBG(int wi)
{
	if(wi != 0 && wi % 2 == 0) wi +=1;
	windowBG = wi;
	perform();
}

void ThresholdOp::updateSmoothingBG(int sm)
{
	if(sm != 0 && sm % 2 == 0) sm +=1;
	smoothBG = sm;
	perform();
}

void ThresholdOp::updateInvertBG(bool in)
{
	invertBG = in;
	perform();
}
