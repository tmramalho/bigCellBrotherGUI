#include "createmarkersop.h"
#include "operationscontroller.h"

CreateMarkersOp::CreateMarkersOp(OperationsController *_controller)
{
	controller = _controller;
	updateParameters();
}

void CreateMarkersOp::updateParameters() {
	width = controller->getPM()->getNamedParameter("ccw");
	height = controller->getPM()->getNamedParameter("cch");
}

void CreateMarkersOp::execute()
{
	cv::Mat backgroundMask = controller->getPipelineImage(24);
	controller->is.setBackgroundMask(backgroundMask);
	cv::Mat originalImage = controller->getPipelineImage(2);
	controller->is.setOriginalImage(originalImage);

	controller->is.createMarkersIterative(originalImage, height, width, 27);
	controller->setPipelineImage(4, controller->is.getMarkersPic());
}

void CreateMarkersOp::createPreview()
{
	cv::Mat original = controller->getPipelineImage(2);
	cv::Mat markers = controller->getPipelineImage(4);
	cv::Mat preview = PictureVis::drawMarkersOnPicture(original, markers);
	controller->setPreview(preview);
}

void CreateMarkersOp::updateWidth(int w)
{
	width = w;
	controller->getPM()->setNamedParameter("ccw", width);
	perform();
}

void CreateMarkersOp::updateHeight(int h)
{
	height = h;
	controller->getPM()->setNamedParameter("cch", height);
	perform();
}
