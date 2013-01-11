#ifndef IMPROVEIMAGEOP_H
#define IMPROVEIMAGEOP_H

#include <QObject>
#include "operation.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageProcessor.h"

class ImproveImageOp : public QObject, public Operation
{
	Q_OBJECT
public:
	ImproveImageOp();

	void execute();
	void createPreview();
	void showPreview();

	cv::Mat pumpImage(cv::Mat &image);

public slots:
	void updateSmoothing(int sm);
	void updateContrast(int ct);
	void updateDoubleRes(int dr);

private:
	int blurWindow;
	int stretchMinVal;
	int doubleRes;
};

#endif // IMPROVEIMAGEOP_H
