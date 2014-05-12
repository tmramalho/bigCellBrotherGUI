#include "cropimageop.h"
#include "operationscontroller.h"

CropImageOp::CropImageOp(OperationsController *_controller)
{
	controller = _controller;
	updateParameters();
}

void CropImageOp::updateParameters() {

	xb = controller->getPM()->getNamedParameter("xb");
	xe = controller->getPM()->getNamedParameter("xe");
	yb = controller->getPM()->getNamedParameter("yb");
	ye = controller->getPM()->getNamedParameter("ye");
    angle = controller->getPM()->getNamedParameter("ang");
}

void CropImageOp::execute()
{
	cv::Mat prev = controller->getPipelineImage(0);
    cv::Mat next(prev.size(), prev.type());

    //rotate
    int len = std::max(prev.cols, prev.rows);
    cv::Point2f pt(len/2., len/2.);
    cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
    cv::warpAffine(prev, next, r, cv::Size(len, len));

    //crop
	cv::Rect roi;
	roi.x = xb;
	roi.y = yb;
	roi.width = xe - xb;
	roi.height = ye - yb;
    cv::Mat final = next(roi);
	cv::Mat croppedImage;
    final.copyTo(croppedImage);
	controller->setPipelineImage(1, croppedImage);
}

void CropImageOp::createPreview()
{
	controller->setPreview(controller->getPipelineImage(1));
}

void CropImageOp::updateXBegin(int pos)
{
    if(pos >= xe) return;
	xb = pos;
	controller->getPM()->setNamedParameter("xb", xb);
	perform();
}

void CropImageOp::updateXEnd(int pos)
{
    if(pos <= xb) return;
	xe = pos;
	controller->getPM()->setNamedParameter("xe", xe);
	perform();
}

void CropImageOp::updateYBegin(int pos)
{
    if(pos >= ye) return;
	yb = pos;
	controller->getPM()->setNamedParameter("yb", yb);
	perform();
}

void CropImageOp::updateYEnd(int pos)
{
    if(pos <= yb) return;
	ye = pos;
	controller->getPM()->setNamedParameter("ye", ye);
    perform();
}

void CropImageOp::updateAngle(int an)
{
   angle = an;
   controller->getPM()->setNamedParameter("ang", angle);
   perform();
}


cv::Mat CropImageOp::cropExternalImage(cv::Mat &image)
{
    cv::Mat next(image.size(), image.type());
    int len = std::max(image.cols, image.rows);
    cv::Point2f pt(len/2., len/2.);
    cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
    cv::warpAffine(image, next, r, cv::Size(len, len));

	cv::Rect roi;
	roi.x = xb;
	roi.y = yb;
	roi.width = xe - xb;
	roi.height = ye - yb;
    cv::Mat final = next(roi);
	cv::Mat result;
    final.copyTo(result);
    return result;
}

void CropImageOp::resetParameters(int xi, int xf, int yi, int yf)
{
    xb = xi;
    xe = xf;
    yb = yi;
    ye = yf;
    controller->getPM()->setNamedParameter("xb", xb);
    controller->getPM()->setNamedParameter("xe", xe);
    controller->getPM()->setNamedParameter("yb", yb);
    controller->getPM()->setNamedParameter("ye", ye);
}
