#include "operationscontroller.h"
#include "operation.h"
#include "cropimageop.h"
#include "improveimageop.h"

OperationsController::OperationsController()
{
	currentStep = "Load Image";
	pipelineReady = false;
	decider = new NaiveBayes();
	nst = 0;
	stepsOrder[currentStep] = this->nst++;
	steps.push_back(currentStep);
}

OperationsController::~OperationsController()
{
	operationPipeline.clear();
	pipelineVisualization.clear();
	pipelineImages.clear();
	delete decider;
}

void OperationsController::resetPipeline(cv::Mat initialFrame)
{
	pipelineReady = false;
	currentStep = "Load Image";
	pipelineVisualization.clear();
	pipelineVisualization[currentStep] = initialFrame;
	this->setPipelineImage(0, initialFrame);

	pipelineReady = true;
}

void OperationsController::setupPipeline(cv::Mat initialFrame)
{
	cv::Size bounds = initialFrame.size();
	emit newBounds(0, bounds.width, 0, bounds.height);
}

cv::Mat OperationsController::runFullPipeline()
{
	runPipelineUntil("Watershed");
	return pipelineImages[5];
}

void OperationsController::addOperation(std::string name, Operation *op)
{
	stepsOrder[name] = this->nst++;
	operationPipeline[name] = op;
	steps.push_back(name);
}

void OperationsController::setPreview(cv::Mat result)
{
	pipelineVisualization[currentStep] = result;
}

void OperationsController::showCurrentPreview()
{
	cv::Mat result = pipelineVisualization[currentStep];
	showSelectedPreview(result);
}

void OperationsController::showSelectedPreview(cv::Mat result)
{
	cv::Mat image;
	if(result.type() == CV_8U)
		cv::cvtColor(result, image, CV_GRAY2RGB);
	else
		cv::cvtColor(result, image, CV_BGR2RGB);

	QImage img = QImage((const unsigned char*)(image.data),
			 image.cols, image.rows, image.step, QImage::Format_RGB888);
	emit operationDone(img);
}

void OperationsController::updateSelectedOperationPreview(std::string op)
{
	if(!pipelineReady) return;
	runPipelineUntil(op);
	currentStep = op;
	showCurrentPreview();
}

cv::Mat OperationsController::getPreviewForOperation(std::string op)
{
	return pipelineVisualization.at(op);
}

cv::Mat OperationsController::cropImage(cv::Mat &image)
{
	CropImageOp* cropOp = (CropImageOp*)(operationPipeline["Crop Image"]);
	cv::Mat croppedImage = cropOp->cropExternalImage(image);
	ImproveImageOp *iiOp = (ImproveImageOp*)(operationPipeline["Improve Image"]);
	croppedImage = iiOp->pumpImage(croppedImage);
	return croppedImage;
}

void OperationsController::runPipelineUntil(std::string op)
{
	if(stepsOrder[op] > stepsOrder[currentStep]) {
		int pos = stepsOrder[currentStep];
		while(currentStep != op) {
			++pos;
			currentStep = steps[pos];
			operationPipeline[currentStep]->execute();
			operationPipeline[currentStep]->createPreview();
		}
	}
}


void OperationsController::parametersUpdated()
{
	for(std::map<std::string, Operation*>::iterator it=operationPipeline.begin();
		it!=operationPipeline.end(); ++it) {
		it->second->updateParameters();
   }
}
