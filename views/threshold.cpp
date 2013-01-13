#include "threshold.h"
#include "ui_threshold.h"

Threshold::Threshold(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Threshold)
{
	ui->setupUi(this);
}

Threshold::~Threshold()
{
	delete ui;
}

void Threshold::bindToOp(ThresholdOp *co)
{
	//QObject::connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThreshold(int)));
	QObject::connect(ui->windowSlider, SIGNAL(valueChanged(int)), co, SLOT(updateWindow(int)));
	QObject::connect(ui->bgThresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdBG(int)));
	QObject::connect(ui->thThresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdTH(int)));
}
