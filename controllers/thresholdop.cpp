#include "thresholdop.h"

ThresholdOp::ThresholdOp()
{
	threshold = 0;
	window = 9;
	smooth = 7;
	invert = true;
	thresholdTH = 2;
	invertTH = false;
	thresholdBG = 0;
	smoothBG = 3;
	invertBG = false;
}

void ThresholdOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(2);
	cv::Mat thImage = ImageProcessor::adaptiveThreshold(prev, threshold, window, invert);
	//thImage = ImageProcessor::applyMorphologyOp(thImage, cv::MORPH_CLOSE, smooth);
	thImage = ImageProcessor::invertImage(thImage);
	thImage = ImageProcessor::erode(thImage, smooth);
	//thImage = ImageProcessor::applyMorphologyOp(thImage, cv::MORPH_CLOSE, smooth);

	cv::Mat bgImage = ImageProcessor::adaptiveThreshold(prev, thresholdBG, window, invertBG);
	bgImage = ImageProcessor::applyMorphologyOp(bgImage, cv::MORPH_CLOSE, 3);
	if(smoothBG > 1) {
		thImage = ImageProcessor::applyMorphologyOp(thImage, cv::MORPH_CLOSE, smoothBG);
		thImage = ImageProcessor::applyMorphologyOp(thImage, cv::MORPH_OPEN, smoothBG);
	}

	cv::Mat background;
	cv::add(thImage, bgImage, background);

	int px = -1, py = -1;
	for( int i = 0; i < background.rows; i++ )
		for( int j = 0; j < background.cols; j++ ) {
			if(background.at<uchar>(i,j)) {
				py = i;
				px = j;
				break;
			}
		}
	if(px >= 0) {
		cv::Mat filledBG = ImageProcessor::floodBackground(prev, px, py, thresholdTH);
		filledBG = ImageProcessor::applyMorphologyOp(filledBG, cv::MORPH_CLOSE, 3);
		cv::add(background, filledBG, background);
	}


	controller->setPipelineImage(22, thImage);
	controller->setPipelineImage(23, bgImage);
	controller->setPipelineImage(24, background);
}

void ThresholdOp::createPreview()
{
	std::vector<cv::Mat> channels;
	cv::Mat prev = controller->getPipelineImage(2);
	cv::Mat null(prev.size(), CV_8U, cv::Scalar::all(0));
	channels.push_back(controller->getPipelineImage(22));
	channels.push_back(controller->getPipelineImage(23));
	channels.push_back(controller->getPipelineImage(24));
	cv::Mat result(channels[0].size(), CV_8U);
	cv::merge(channels, result);
	cv::Mat orig;
	cv::cvtColor(prev, orig, CV_GRAY2BGR);
	controller->setPreview(0.5*result+0.5*orig);
}

void ThresholdOp::updateThreshold(int th)
{
	threshold = th;
	perform();
}

void ThresholdOp::updateWindow(int wi)
{
	if(wi != 0 && wi % 2 == 0) wi +=1;
	std::cout << wi << std::endl;
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
