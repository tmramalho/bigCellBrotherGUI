#include "thresholdop.h"
#include "operationscontroller.h"

ThresholdOp::ThresholdOp(OperationsController *_controller)
{
	controller = _controller;
	updateParameters();
    thresholdTH = 255;
    threshold = 0;
    thresholdBG = 0;
    window = 27;
}

void ThresholdOp::updateParameters() {

	threshold = controller->getPM()->getNamedParameter("tto");
	window = controller->getPM()->getNamedParameter("ttw");
	thresholdTH = controller->getPM()->getNamedParameter("ttt");
	thresholdBG = controller->getPM()->getNamedParameter("ttb");
}

void ThresholdOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(2);
    /*cv::Mat thImage = ImageProcessor::adaptiveThreshold(prev, threshold, window, true);
	thImage = ImageProcessor::invertImage(thImage);
    thImage = ImageProcessor::erode(thImage, 7);*/

	cv::Mat bgImage = ImageProcessor::adaptiveThreshold(prev, thresholdBG, window, false);
    bgImage = ImageProcessor::applyMorphologyOp(bgImage, cv::MORPH_CLOSE, 3);
    //bgImage = ImageProcessor::applyMorphologyOp(bgImage, cv::MORPH_OPEN, 3);

	cv::Mat background = bgImage;

    if(thresholdTH > 0) {
        cv::Mat filledBG = ImageProcessor::threshold(prev, thresholdTH, false);
        filledBG = ImageProcessor::applyMorphologyOp(filledBG, cv::MORPH_CLOSE, 3);
		cv::add(background, filledBG, background);
    }

    //controller->setPipelineImage(22, thImage);
	controller->setPipelineImage(23, bgImage);
	controller->setPipelineImage(24, background);
}

void ThresholdOp::createPreview()
{
	std::vector<cv::Mat> channels;
	cv::Mat prev = controller->getPipelineImage(2);
	cv::Mat null(prev.size(), CV_8U, cv::Scalar::all(0));
    channels.push_back(null);
    channels.push_back(controller->getPipelineImage(24));
    channels.push_back(null);
	cv::Mat result(channels[0].size(), CV_8U);
	cv::merge(channels, result);
	cv::Mat orig;
	cv::cvtColor(prev, orig, CV_GRAY2BGR);
    controller->setPreview(0.2*result+0.8*orig);
}

void ThresholdOp::updateThreshold(int th)
{
	threshold = th;
	controller->getPM()->setNamedParameter("tto", threshold);
	perform();
}

void ThresholdOp::updateWindow(int wi)
{
	if(wi != 0 && wi % 2 == 0) wi +=1;
	std::cout << wi << std::endl;
	window = wi;
	controller->getPM()->setNamedParameter("ttw", window);
	perform();
}

void ThresholdOp::updateThresholdTH(int th)
{
	thresholdTH = th;
	controller->getPM()->setNamedParameter("ttt", thresholdTH);
	perform();
}

void ThresholdOp::updateThresholdBG(int th)
{
	thresholdBG = th;
	controller->getPM()->setNamedParameter("ttb", thresholdBG);
	perform();
}
