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
    QObject::connect(ui->sbThresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThreshold(int)));
	QObject::connect(ui->windowSlider, SIGNAL(valueChanged(int)), co, SLOT(updateWindow(int)));
	QObject::connect(ui->bgThresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdBG(int)));
	QObject::connect(ui->thThresholdSlider, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdTH(int)));
    QObject::connect(ui->sbThresholdBox, SIGNAL(valueChanged(int)), co, SLOT(updateThreshold(int)));
    QObject::connect(ui->windowBox, SIGNAL(valueChanged(int)), co, SLOT(updateWindow(int)));
    QObject::connect(ui->bgThresholdBox, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdBG(int)));
    QObject::connect(ui->thThresholdBox, SIGNAL(valueChanged(int)), co, SLOT(updateThresholdTH(int)));
    QObject::connect(ui->sbThresholdSlider, SIGNAL(valueChanged(int)), ui->sbThresholdBox, SLOT(setValue(int)));
    QObject::connect(ui->windowSlider, SIGNAL(valueChanged(int)), ui->windowBox, SLOT(setValue(int)));
    QObject::connect(ui->bgThresholdSlider, SIGNAL(valueChanged(int)), ui->bgThresholdBox, SLOT(setValue(int)));
    QObject::connect(ui->thThresholdSlider, SIGNAL(valueChanged(int)), ui->thThresholdBox, SLOT(setValue(int)));
    QObject::connect(ui->sbThresholdBox, SIGNAL(valueChanged(int)), ui->sbThresholdSlider, SLOT(setValue(int)));
    QObject::connect(ui->windowBox, SIGNAL(valueChanged(int)), ui->windowSlider, SLOT(setValue(int)));
    QObject::connect(ui->bgThresholdBox, SIGNAL(valueChanged(int)), ui->bgThresholdSlider, SLOT(setValue(int)));
    QObject::connect(ui->thThresholdBox, SIGNAL(valueChanged(int)), ui->thThresholdSlider, SLOT(setValue(int)));
}
