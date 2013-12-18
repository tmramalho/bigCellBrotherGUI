#ifndef CROPIMAGE_H
#define CROPIMAGE_H

#include <QDialog>
#include "cropimageop.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class CropImage;
}

class CropImage : public QDialog
{
	Q_OBJECT

public:
	explicit CropImage(QWidget *parent = 0);
	~CropImage();
	void bindToOp(CropImageOp *co);

public slots:
	void updateBounds(int xi, int xf, int yi, int yf);

private:
	Ui::CropImage *ui;
    CropImageOp *cropper;
};

#endif // CROPIMAGE_H
