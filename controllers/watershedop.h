#ifndef WATERSHEDOP_H
#define WATERSHEDOP_H

#include <QObject>
#include "operation.h"
#include "helpers/PictureVis.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageProcessor.h"
#include "processors/ImageSegmentor.h"

class WatershedOp : public QObject, public Operation
{
	Q_OBJECT

public:
	WatershedOp();
	void execute();
	void createPreview();
	void showPreview();

public slots:
	void updateSmoothing(int sm);
	void updateSize(int sz);
	void updateStep(int sp);
	void removeBorder(bool rm);

private:
	int smoothing;
	int size;
	int nStep;
	bool rmBorder;
};

#endif // WATERSHEDOP_H
