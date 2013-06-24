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
    QObject::connect(ui->contrastSlider, SIGNAL(valueChanged(int)), ui->contrastBox, SLOT(setValue(int)));
    QObject::connect(ui->noiseSlider, SIGNAL(valueChanged(int)), iio, SLOT(updateNoise(int)));
    QObject::connect(ui->noiseSlider, SIGNAL(valueChanged(int)), ui->noiseBox, SLOT(setValue(int)));
    QObject::connect(ui->smoothSlider, SIGNAL(valueChanged(int)), iio, SLOT(updateSmoothing(int)));
    QObject::connect(ui->smoothSlider, SIGNAL(valueChanged(int)), ui->smoothBox, SLOT(setValue(int)));
    QObject::connect(ui->contrastBox, SIGNAL(valueChanged(int)), iio, SLOT(updateContrast(int)));
    QObject::connect(ui->contrastBox, SIGNAL(valueChanged(int)), ui->contrastSlider, SLOT(setValue(int)));
    QObject::connect(ui->noiseBox, SIGNAL(valueChanged(int)), iio, SLOT(updateNoise(int)));
    QObject::connect(ui->noiseBox, SIGNAL(valueChanged(int)), ui->noiseSlider, SLOT(setValue(int)));
    QObject::connect(ui->smoothBox, SIGNAL(valueChanged(int)), iio, SLOT(updateSmoothing(int)));
    QObject::connect(ui->smoothBox, SIGNAL(valueChanged(int)), ui->smoothSlider, SLOT(setValue(int)));

	QObject::connect(ui->doubleRes, SIGNAL(currentIndexChanged(int)), iio, SLOT(updateDoubleRes(int)));
}

