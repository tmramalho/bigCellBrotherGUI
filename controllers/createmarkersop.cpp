#include "createmarkersop.h"

CreateMarkersOp::CreateMarkersOp()
{
	width = 10;
	height = 80;
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
	perform();
}

void CreateMarkersOp::updateHeight(int h)
{
	height = h;
	perform();
}
