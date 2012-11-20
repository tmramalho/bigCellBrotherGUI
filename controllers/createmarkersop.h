#ifndef CREATEMARKERSOP_H
#define CREATEMARKERSOP_H

#include <QObject>
#include "operation.h"
#include "helpers/PictureVis.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageProcessor.h"

class CreateMarkersOp : public QObject, public Operation
{
	Q_OBJECT
public:
	CreateMarkersOp();

	void execute();
	void createPreview();
	void showPreview();

public slots:
	void updateWidth(int w);
	void updateHeight(int h);
private:
	int width;
	int height;
};

#endif // CREATEMARKERSOP_H
