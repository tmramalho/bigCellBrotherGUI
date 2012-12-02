#include "watershedop.h"

WatershedOp::WatershedOp()
{
	size = 3;
	smoothing = 1;
	nStep = 50;
}

void WatershedOp::execute()
{
	cv::Mat image = controller->getPipelineImage(2);
	cv::Mat boosted = controller->getPipelineImage(33);
	cv::Mat backgroundMask = controller->getPipelineImage(24);
	cv::Mat markers;
	controller->getPipelineImage(4).copyTo(markers);
	controller->is.setOriginalImage(image);
	controller->is.setBoostedImage(boosted);
	controller->is.setMarkersPic(markers);
	controller->is.setBackgroundMask(backgroundMask);
	controller->is.watershed();

	for (int n = 3; n < size; n += nStep) {
		controller->is.removeSmallMarkers(n);
		controller->is.watershed();
	}

	if(smoothing > 1) controller->is.smoothLabels(smoothing);

	controller->setPipelineImage(5, controller->is.getMarkersPic());
}

void WatershedOp::createPreview()
{
	cv::Mat original = controller->getPipelineImage(2);
	cv::Mat markers = controller->getPipelineImage(5);
	cv::Mat preview = PictureVis::drawMarkersOnPicture(original, markers);
	controller->setPreview(preview);
}

void WatershedOp::updateSmoothing(int sm)
{
	if(sm % 2 == 0) sm +=1;
	smoothing = sm;
	perform();
}

void WatershedOp::updateSize(int sz)
{
	size = sz;
	perform();
}

void WatershedOp::updateStep(int sp)
{
	nStep = sp;
	perform();
}
