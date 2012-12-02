#include "operationscontroller.h"

OperationsController::OperationsController()
{
	currentStep = 0;
	pipelineReady = false;
	decider = NULL;
}

OperationsController::~OperationsController()
{
	operationPipeline.clear();
	operationDialogs.clear();
	pipelineVisualization.clear();
	pipelineImages.clear();
}

void OperationsController::resetPipeline(cv::Mat initialFrame)
{
	pipelineReady = false;
	pipelineVisualization.clear();
	pipelineVisualization.push_back(initialFrame);
	this->setPipelineImage(0, initialFrame);

	currentStep = 0;
	pipelineReady = true;
}

void OperationsController::setupPipeline(cv::Mat initialFrame)
{
	cv::Size bounds = initialFrame.size();
	emit newBounds(0, bounds.width, 0, bounds.height);
}

cv::Mat OperationsController::runFullPipeline()
{
	runPipelineUntil(operationDialogs.size()-1);
	return pipelineImages[5];
}

void OperationsController::editOperation(int op)
{
	if(pipelineReady) {
		runPipelineUntil(op);
		std::cout << "Editing operation " << op << std::endl;
		currentStep = op + 1; //current step incremented before executing
		operationDialogs[op]->show();
	}
}

void OperationsController::setPreview(cv::Mat result)
{
	if(pipelineVisualization.size() > (uint) currentStep) {
		pipelineVisualization[currentStep] = result;
	} else {
		pipelineVisualization.push_back(result);
	}
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

void OperationsController::updateSelectedOperationPreview(int op)
{
	if(!pipelineReady) return;
	runPipelineUntil(op);
	currentStep = op + 1;
	showCurrentPreview();
}

cv::Mat OperationsController::getPreviewForOperation(int op)
{
	int pos = op + 1;
	if(op < -2) pos = operationDialogs.size() - 1;
	return pipelineVisualization[pos];
}

void OperationsController::runPipelineUntil(int op)
{
	if(currentStep > op) return; //already past it
	for(int i = currentStep; i < op + 1; i++) {
		currentStep++; //current step incremented before executing
		operationPipeline[i]->execute();
		operationPipeline[i]->createPreview();
	}
}
