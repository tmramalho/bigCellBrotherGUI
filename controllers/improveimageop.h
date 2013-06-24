#ifndef IMPROVEIMAGEOP_H
#define IMPROVEIMAGEOP_H

#include <QObject>
#include "operation.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/photo/photo.hpp>
#include "lib/processors/ImageProcessor.h"

class ImproveImageOp : public QObject, public Operation
{
	Q_OBJECT
public:
	ImproveImageOp(OperationsController *_controller);

	void execute();
	void createPreview();
	void updateParameters();
	void showPreview();

	cv::Mat pumpImage(cv::Mat &image);

public slots:
	void updateSmoothing(int sm);
	void updateContrast(int ct);
	void updateDoubleRes(int dr);
    void updateNoise(int ds);

private:
	int blurWindow;
	int stretchMinVal;
	int doubleRes;
    int denoisingStrength;
};

#endif // IMPROVEIMAGEOP_H
