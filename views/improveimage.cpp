#include "improveimage.h"
#include "ui_improveimage.h"

ImproveImage::ImproveImage(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ImproveImage)
{
    ui->setupUi(this);
}

ImproveImage::~ImproveImage()
{
	delete ui;
}

void ImproveImage::bindToOp(ImproveImageOp *iio)
{
	QObject::connect(ui->contrastSlider, SIGNAL(valueChanged(int)), iio, SLOT(updateContrast(int)));
	QObject::connect(ui->smoothSlider, SIGNAL(valueChanged(int)), iio, SLOT(updateSmoothing(int)));
	QObject::connect(ui->doubleRes, SIGNAL(currentIndexChanged(int)), iio, SLOT(updateDoubleRes(int)));
}

