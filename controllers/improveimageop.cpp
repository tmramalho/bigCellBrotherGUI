#include "improveimageop.h"

ImproveImageOp::ImproveImageOp()
{
	blurWindow = 0;
	stretchMinVal = 0;
	doubleRes = false;
}

void ImproveImageOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(1);
	cv::Mat improveImg = ImageProcessor::simplifyImage(prev, blurWindow, stretchMinVal);
	cv::Mat boostedImg = ImageProcessor::simplifyImage(prev, blurWindow, stretchMinVal, true);
	if(doubleRes) {
		cv::Mat improveImgPump;
		cv::Mat boostedImgPump;
		cv::resize(improveImg, improveImgPump, cv::Size(), 2, 2, cv::INTER_CUBIC);
		cv::resize(boostedImg, boostedImgPump, cv::Size(), 2, 2, cv::INTER_CUBIC);
		controller->setPipelineImage(2, improveImgPump);
		controller->setPipelineImage(32, boostedImgPump);
	} else {
		controller->setPipelineImage(2, improveImg);
		controller->setPipelineImage(32, boostedImg);
	}
}

void ImproveImageOp::createPreview()
{
	controller->setPreview(controller->getPipelineImage(2));
}
void ImproveImageOp::updateSmoothing(int sm)
{
	if(sm != 0 && sm % 2 == 0) sm +=1;
	blurWindow = sm;
	perform();
}

void ImproveImageOp::updateContrast(int ct)
{
	stretchMinVal = ct;
	perform();
}

void ImproveImageOp::updateDoubleRes(bool dr)
{
	doubleRes = dr;
	perform();
}
