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

    int window = controller->getPM()->getNamedParameter("ttw");
    cv::Mat topHat = ImageProcessor::applyMorphologyOp(originalImage, cv::MORPH_TOPHAT, window);

    cv::Mat blobs = ImageProcessor::adaptiveThreshold(originalImage, 0, window, true);
    blobs = blobs - topHat - backgroundMask;

    int laplaceWindow = (window < 32) ? window : 31;
    cv::Mat laplace = ImageProcessor::laplacian(originalImage, laplaceWindow);
    cv::Mat distTrans = ImageProcessor::distanceTransform(blobs);
    cv::Mat landscape = distTrans*0.7 + laplace*0.3 - topHat - backgroundMask;

    controller->is.createMarkersIterative(originalImage, landscape, height, width);
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
