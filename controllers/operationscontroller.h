#ifndef OPERATIONSCONTROLLER_H
#define OPERATIONSCONTROLLER_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <QObject>
#include <QImage>
#include <QtWidgets/QDialog>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "lib/processors/ImageSegmentor.h"
#include "lib/files/CSVReader.h"
#include "parameterloader.h"

class Operation;

class OperationsController : public QObject
{
	Q_OBJECT

public:
	OperationsController();
	~OperationsController();

	void resetPipeline(cv::Mat initialFrame);
	void setupPipeline(cv::Mat initialFrame);
	cv::Mat runFullPipeline();
	void addOperation(std::string name, Operation *op);

	void setPreview(cv::Mat result);
	void showCurrentPreview();
	void showSelectedPreview(cv::Mat result);
	void updateSelectedOperationPreview(std::string op);
    void setOperationState(std::string op);
	cv::Mat getPreviewForOperation(std::string op);
	cv::Mat transformImage(cv::Mat &image);

	int getStepOrder(std::string st) { return stepsOrder[st]; }
	void setPipelineImage(int i, cv::Mat result) { pipelineImages[i] = result; }
	cv::Mat getPipelineImage(int i) { return pipelineImages[i]; }

	std::string getCurrentStep() { return currentStep; }

	ParameterLoader* getPM() { return pm; }
	void setPM(ParameterLoader *_pm) { pm = _pm; }

	ImageSegmentor is;
	bool pipelineReady;

signals:
	void operationDone(QImage imagePreview);
	void newBounds(int xi, int xf, int yi, int yf);

public slots:
	void parametersUpdated();

private:
	void runPipelineUntil(std::string op);
	std::string currentStep;
	std::map<int, cv::Mat> pipelineImages;
	std::map<std::string, Operation *> operationPipeline;
	std::map<std::string, cv::Mat> pipelineVisualization;
	std::map<std::string, int> stepsOrder;
	std::vector<std::string> steps;
	int nst;
	ParameterLoader *pm;
};

#endif // OPERATIONSCONTROLLER_H
