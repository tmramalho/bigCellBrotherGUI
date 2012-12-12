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
	CropImageOp();

	void execute();
	void createPreview();
	void showPreview();

public slots:

	void updateXBegin(int pos);
	void updateXEnd(int pos);
	void updateYBegin(int pos);
	void updateYEnd(int pos);

private:
	int xb;
	int xe;
	int yb;
	int ye;
};

#endif // CROPIMAGEOP_H
