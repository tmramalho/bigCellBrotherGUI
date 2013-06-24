#ifndef THRESHOLDOP_H
#define THRESHOLDOP_H

#include <QObject>
#include "operation.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "lib/processors/ImageProcessor.h"

class ThresholdOp : public QObject, public Operation
{
	Q_OBJECT
public:
	ThresholdOp(OperationsController *_controller);

	void execute();
	void createPreview();
	void updateParameters();
	void showPreview();

public slots:
	void updateThreshold(int th);
	void updateWindow(int wi);
	void updateThresholdTH(int th);
	void updateThresholdBG(int th);

private:
	int threshold;
	int window;
	int thresholdTH;
	int thresholdBG;
};

#endif // THRESHOLDOP_H
