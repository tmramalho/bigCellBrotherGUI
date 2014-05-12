#include "cropimage.h"
#include "ui_cropimage.h"

CropImage::CropImage(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CropImage)
{
	ui->setupUi(this);
}

CropImage::~CropImage()
{
	delete ui;
}

void CropImage::bindToOp(CropImageOp *co)
{
    cropper = co;
	QObject::connect(ui->xbs, SIGNAL(valueChanged(int)), co, SLOT(updateXBegin(int)));
    QObject::connect(ui->xbs, SIGNAL(valueChanged(int)), ui->spinBox, SLOT(setValue(int)));
	QObject::connect(ui->xes, SIGNAL(valueChanged(int)), co, SLOT(updateXEnd(int)));
    QObject::connect(ui->xes, SIGNAL(valueChanged(int)), ui->spinBox_2, SLOT(setValue(int)));
    QObject::connect(ui->ybs, SIGNAL(valueChanged(int)), co, SLOT(updateYBegin(int)));
    QObject::connect(ui->ybs, SIGNAL(valueChanged(int)), ui->spinBox_3, SLOT(setValue(int)));
	QObject::connect(ui->yes, SIGNAL(valueChanged(int)), co, SLOT(updateYEnd(int)));
    QObject::connect(ui->yes, SIGNAL(valueChanged(int)), ui->spinBox_4, SLOT(setValue(int)));
    QObject::connect(ui->angle, SIGNAL(valueChanged(int)), co, SLOT(updateAngle(int)));
    QObject::connect(ui->angle, SIGNAL(valueChanged(int)), ui->spinBox_5, SLOT(setValue(int)));

    QObject::connect(ui->spinBox, SIGNAL(valueChanged(int)), co, SLOT(updateXBegin(int)));
    QObject::connect(ui->spinBox, SIGNAL(valueChanged(int)), ui->xbs, SLOT(setValue(int)));
    QObject::connect(ui->spinBox_2, SIGNAL(valueChanged(int)), co, SLOT(updateXEnd(int)));
    QObject::connect(ui->spinBox_2, SIGNAL(valueChanged(int)), ui->xes, SLOT(setValue(int)));
    QObject::connect(ui->spinBox_3, SIGNAL(valueChanged(int)), co, SLOT(updateYBegin(int)));
    QObject::connect(ui->spinBox_3, SIGNAL(valueChanged(int)), ui->ybs, SLOT(setValue(int)));
    QObject::connect(ui->spinBox_4, SIGNAL(valueChanged(int)), co, SLOT(updateYEnd(int)));
    QObject::connect(ui->spinBox_4, SIGNAL(valueChanged(int)), ui->yes, SLOT(setValue(int)));
    QObject::connect(ui->spinBox_5, SIGNAL(valueChanged(int)), co, SLOT(updateAngle(int)));
    QObject::connect(ui->spinBox_5, SIGNAL(valueChanged(int)), ui->angle, SLOT(setValue(int)));
}

void CropImage::updateBounds(int xi, int xf, int yi, int yf)
{
    cropper->resetParameters(xi, xf, yi, yf);
	ui->xbs->setMaximum(xf);
	ui->xbs->setSingleStep(1);
	ui->xbs->setTickInterval(xf/10);
	ui->xbs->setValue(xi);
    ui->spinBox->setMaximum(xf);
    ui->spinBox->setValue(xi);
	ui->xes->setMaximum(xf);
	ui->xes->setSingleStep(1);
	ui->xes->setTickInterval(xf/10);
	ui->xes->setValue(xf);
    ui->spinBox_2->setMaximum(xf);
    ui->spinBox_2->setValue(xf);
	ui->ybs->setMaximum(yf);
	ui->ybs->setSingleStep(1);
	ui->ybs->setTickInterval(yf/10);
	ui->ybs->setValue(yi);
    ui->spinBox_3->setMaximum(yf);
    ui->spinBox_3->setValue(yi);
	ui->yes->setMaximum(yf);
	ui->yes->setSingleStep(1);
	ui->yes->setTickInterval(yf/10);
	ui->yes->setValue(yf);
    ui->spinBox_4->setMaximum(yf);
    ui->spinBox_4->setValue(yf);
}
