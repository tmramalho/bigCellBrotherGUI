#ifndef THRESHOLDOP_H
#define THRESHOLDOP_H

#include <QObject>
#include "operation.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageProcessor.h"

class ThresholdOp : public QObject, public Operation
{
	Q_OBJECT
public:
	ThresholdOp();

	void execute();
	void createPreview();
	void showPreview();

public slots:
	void updateThreshold(int th);
	void updateWindow(int wi);
	void updateSmoothing(int sm);
	void updateInvert(bool in);
	void updateThresholdTH(int th);
	void updateInvertTH(bool in);
	void updateThresholdBG(int th);
	void updateSmoothingBG(int sm);
	void updateInvertBG(bool in);

private:
	int threshold;
	int window;
	int smooth;
	bool invert;
	int thresholdTH;
	bool invertTH;
	int thresholdBG;
	int smoothBG;
	bool invertBG;
};

#endif // THRESHOLDOP_H
