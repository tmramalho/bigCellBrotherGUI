#include "watershedop.h"
#include "operationscontroller.h"

WatershedOp::WatershedOp(OperationsController *_controller)
{
	controller = _controller;
	updateParameters();
}

void WatershedOp::updateParameters() {

	size = controller->getPM()->getNamedParameter("wsz");
	smoothing = controller->getPM()->getNamedParameter("wsm");
	nStep = controller->getPM()->getNamedParameter("wsst");
	rmBorder = controller->getPM()->getNamedParameter("wsrm");
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
	if(rmBorder) controller->is.removeBorderMarkers();

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
	controller->getPM()->setNamedParameter("wsm", smoothing);
	perform();
}

void WatershedOp::updateSize(int sz)
{
	size = sz;
	controller->getPM()->setNamedParameter("wsz", size);
	perform();
}

void WatershedOp::updateStep(int sp)
{
	nStep = sp;
	controller->getPM()->setNamedParameter("wsst", nStep);
	perform();
}

void WatershedOp::removeBorder(bool rm)
{
	rmBorder = rm;
	controller->getPM()->setNamedParameter("wsrm", rmBorder);
	perform();
}
