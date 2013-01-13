#include "improveimageop.h"
#include "operationscontroller.h"

ImproveImageOp::ImproveImageOp(OperationsController *_controller)
{
	controller = _controller;
	updateParameters();
}

void ImproveImageOp::updateParameters() {

	blurWindow = controller->getPM()->getNamedParameter("iibw");
	stretchMinVal = controller->getPM()->getNamedParameter("iism");
	doubleRes = controller->getPM()->getNamedParameter("iidr");
}

void ImproveImageOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(1);
	cv::Mat improveImg = ImageProcessor::simplifyImage(prev, blurWindow, stretchMinVal);
	cv::Mat boostedImg = ImageProcessor::simplifyImage(prev, blurWindow, stretchMinVal, true);
	cv::Mat improveImgPump;
	cv::Mat boostedImgPump;

	if(doubleRes == 0) {
		improveImgPump = improveImg;
		boostedImgPump = boostedImg;
	} if(doubleRes == 1) {
		cv::resize(improveImg, improveImgPump, cv::Size(), 2, 2, cv::INTER_CUBIC);
		cv::resize(boostedImg, boostedImgPump, cv::Size(), 2, 2, cv::INTER_CUBIC);
	} else if(doubleRes == 2) {
		cv::resize(improveImg, improveImgPump, cv::Size(), 4, 4, cv::INTER_CUBIC);
		cv::resize(boostedImg, boostedImgPump, cv::Size(), 4, 4, cv::INTER_CUBIC);
	}

	controller->setPipelineImage(2, improveImgPump);
	controller->setPipelineImage(32, boostedImgPump);

	cv::Mat gradient = ImageProcessor::laplacian(improveImgPump, 27);
	gradient = ImageProcessor::invertImage(gradient);
	controller->setPipelineImage(33, gradient);
}

void ImproveImageOp::createPreview()
{
	controller->setPreview(controller->getPipelineImage(2));
}
void ImproveImageOp::updateSmoothing(int sm)
{
	if(sm != 0 && sm % 2 == 0) sm +=1;
	blurWindow = sm;
	controller->getPM()->setNamedParameter("iibw", blurWindow);
	perform();
}

void ImproveImageOp::updateContrast(int ct)
{
	stretchMinVal = ct;
	controller->getPM()->setNamedParameter("iism", stretchMinVal);
	perform();
}

void ImproveImageOp::updateDoubleRes(int dr)
{
	doubleRes = dr;
	controller->getPM()->setNamedParameter("iidr", doubleRes);
	perform();
}


cv::Mat ImproveImageOp::pumpImage(cv::Mat &image)
{
	cv::Mat imgPump;

	if(doubleRes == 0) {
		imgPump = image;
	} if(doubleRes == 1) {
		cv::resize(image, imgPump, cv::Size(), 2, 2, cv::INTER_CUBIC);
	} else if(doubleRes == 2) {
		cv::resize(image, imgPump, cv::Size(), 4, 4, cv::INTER_CUBIC);
	}

	return imgPump;
}
