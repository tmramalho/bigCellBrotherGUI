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
	QObject::connect(ui->xbs, SIGNAL(valueChanged(int)), co, SLOT(updateXBegin(int)));
	QObject::connect(ui->xes, SIGNAL(valueChanged(int)), co, SLOT(updateXEnd(int)));
	QObject::connect(ui->ybs, SIGNAL(valueChanged(int)), co, SLOT(updateYBegin(int)));
	QObject::connect(ui->yes, SIGNAL(valueChanged(int)), co, SLOT(updateYEnd(int)));
}

void CropImage::updateBounds(int xi, int xf, int yi, int yf)
{
	ui->xbs->setMaximum(xf);
	ui->xbs->setSingleStep(1);
	ui->xbs->setTickInterval(xf/10);
	ui->xbs->setValue(xi);
	ui->xes->setMaximum(xf);
	ui->xes->setSingleStep(1);
	ui->xes->setTickInterval(xf/10);
	ui->xes->setValue(xf);
	ui->ybs->setMaximum(yf);
	ui->ybs->setSingleStep(1);
	ui->ybs->setTickInterval(yf/10);
	ui->ybs->setValue(yi);
	ui->yes->setMaximum(yf);
	ui->yes->setSingleStep(1);
	ui->yes->setTickInterval(yf/10);
	ui->yes->setValue(yf);
}
