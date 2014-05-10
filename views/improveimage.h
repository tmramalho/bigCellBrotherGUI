#ifndef IMPROVEIMAGE_H
#define IMPROVEIMAGE_H

#include <QtWidgets/QDialog>
#include "improveimageop.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class ImproveImage;
}

class ImproveImage : public QDialog
{
	Q_OBJECT

public:
	explicit ImproveImage(QWidget *parent = 0);
	~ImproveImage();
	void bindToOp(ImproveImageOp *iio);

private:
	Ui::ImproveImage *ui;
};

#endif // IMPROVEIMAGE_H
