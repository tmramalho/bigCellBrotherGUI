#include "createmarkersop.h"

CreateMarkersOp::CreateMarkersOp()
{
	width = 100;
	height = 100;
}

void CreateMarkersOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(3);
	cv::Mat backgroundMask = controller->getPipelineImage(24);
	controller->is.setBackgroundMask(backgroundMask);
	cv::Mat originalImage = controller->getPipelineImage(2);
	controller->is.setOriginalImage(originalImage);

	controller->is.createMarkers(prev, height, width);
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
	perform();
}

void CreateMarkersOp::updateHeight(int h)
{
	height = h;
	perform();
}
