#ifndef CROPIMAGEOP_H
#define CROPIMAGEOP_H

#include <QObject>
#include "operation.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class CropImageOp : public QObject, public Operation
{
	Q_OBJECT

public:
	CropImageOp(OperationsController *_controller);

	void execute();
	void createPreview();
	void updateParameters();
	void showPreview();

	cv::Mat cropExternalImage(cv::Mat &image);
    void resetParameters(int xi, int xf, int yi, int yf);

public slots:

	void updateXBegin(int pos);
	void updateXEnd(int pos);
	void updateYBegin(int pos);
	void updateYEnd(int pos);
    void updateAngle(int an);

private:
	int xb;
	int xe;
	int yb;
	int ye;
    int angle;
};

#endif // CROPIMAGEOP_H
