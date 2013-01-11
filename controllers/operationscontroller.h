#ifndef OPERATIONSCONTROLLER_H
#define OPERATIONSCONTROLLER_H

#include <vector>
#include <map>
#include <iostream>
#include <QObject>
#include <QImage>
#include <QDialog>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageSegmentor.h"
#include "files/CSVReader.h"
#include "classifier/NaiveBayes.h"

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

	void setPreview(cv::Mat result);
	void showCurrentPreview();
	void showSelectedPreview(cv::Mat result);
	void updateSelectedOperationPreview(int op);
	cv::Mat getPreviewForOperation(int op);
	cv::Mat cropImage(cv::Mat &image);

	void setPipelineImage(int i, cv::Mat result) { pipelineImages[i] = result; }
	cv::Mat getPipelineImage(int i) { return pipelineImages[i]; }

	int getCurrentStep() { return currentStep; }

	std::vector<Operation *> operationPipeline;
	std::vector<QDialog *> operationDialogs;
	ImageSegmentor is;
	bool pipelineReady;
	NaiveBayes *decider;

signals:
	void operationDone(QImage imagePreview);
	void newBounds(int xi, int xf, int yi, int yf);

private:
	void runPipelineUntil(int op);
	int currentStep;
	std::map<int, cv::Mat> pipelineImages;
	std::vector<cv::Mat> pipelineVisualization;
};

#endif // OPERATIONSCONTROLLER_H
