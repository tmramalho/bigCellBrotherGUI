#include "improveimageop.h"
#include "operationscontroller.h"

ImproveImageOp::ImproveImageOp(OperationsController *_controller)
{
	controller = _controller;
    denoisingStrength = 3;
	updateParameters();
}

void ImproveImageOp::updateParameters() {

	blurWindow = controller->getPM()->getNamedParameter("iibw");
	stretchMinVal = controller->getPM()->getNamedParameter("iism");
	doubleRes = controller->getPM()->getNamedParameter("iidr");
    denoisingStrength = controller->getPM()->getNamedParameter("dns");
}

void ImproveImageOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(1);
    cv::Mat improveImg;
    cv::Mat boostedImg;
    cv::Mat improveImgPump;
    cv::Mat boostedImgPump;

    if(stretchMinVal > -1) {
        improveImg = ImageProcessor::simplifyImage(prev, 0, stretchMinVal);
        boostedImg = ImageProcessor::simplifyImage(prev, 0, stretchMinVal, true);
    } else {
        prev.copyTo(improveImg);
        boostedImg = ImageProcessor::simplifyImage(prev, 0, 0, true);
    }

    if(denoisingStrength > 0) {
        cv::fastNlMeansDenoising(improveImg, improveImg, denoisingStrength);
    }

    if(blurWindow > 0) {
        cv::Mat tmp;
        cv::GaussianBlur(improveImg, tmp, cv::Size(blurWindow,blurWindow), blurWindow);
        cv::addWeighted(improveImg, 1.5, tmp, -0.5, 0, improveImg);
        cv::GaussianBlur(boostedImg, tmp, cv::Size(blurWindow,blurWindow), blurWindow);
        cv::addWeighted(boostedImg, 1.5, tmp, -0.5, 0, boostedImg);
    }

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

void ImproveImageOp::updateNoise(int ds)
{
    denoisingStrength = ds;
    controller->getPM()->setNamedParameter("dns", denoisingStrength);
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
