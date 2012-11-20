#include "cropimageop.h"

CropImageOp::CropImageOp()
{
	xb = 0;
	xe = 100;
	yb = 0;
	ye = 100;
}

void CropImageOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(0);
	cv::Rect roi;
	roi.x = xb;
	roi.y = yb;
	roi.width = xe - xb;
	roi.height = ye - yb;
	cv::Mat next = prev(roi);
	cv::Mat croppedImage;
	next.copyTo(croppedImage);
	controller->setPipelineImage(1, croppedImage);
}

void CropImageOp::createPreview()
{
	controller->setPreview(controller->getPipelineImage(1));
}

void CropImageOp::updateXBegin(int pos)
{
	xb = pos;
	perform();
}

void CropImageOp::updateXEnd(int pos)
{
	xe = pos;
	perform();
}

void CropImageOp::updateYBegin(int pos)
{
	yb = pos;
	perform();
}

void CropImageOp::updateYEnd(int pos)
{
	ye = pos;
	perform();
}
