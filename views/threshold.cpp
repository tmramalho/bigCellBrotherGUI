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
	QObject::connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThreshold(int)));
	QObject::connect(ui->windowSlider, SIGNAL(valueChanged(int)), co, SLOT(updateWindow(int)));
	QObject::connect(ui->smoothSlider, SIGNAL(valueChanged(int)), co, SLOT(updateSmoothing(int)));
	//QObject::connect(ui->invertOp, SIGNAL(toggled(bool)), co, SLOT(updateInvert(bool)));
	QObject::connect(ui->bgThresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdBG(int)));
	//QObject::connect(ui->bgSmoothSlider, SIGNAL(valueChanged(int)), co, SLOT(updateSmoothingBG(int)));
	//QObject::connect(ui->invertBg, SIGNAL(toggled(bool)), co, SLOT(updateInvertBG(bool)));
	QObject::connect(ui->thThresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdTH(int)));
	//QObject::connect(ui->thInvertOp, SIGNAL(toggled(bool)), co, SLOT(updateInvertTH(bool)));
}
