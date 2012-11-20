#include "watershed.h"
#include "ui_watershed.h"

Watershed::Watershed(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Watershed)
{
	ui->setupUi(this);
}

Watershed::~Watershed()
{
	delete ui;
}

void Watershed::bindToOp(WatershedOp *co)
{
	QObject::connect(ui->sizeSlider, SIGNAL(valueChanged(int)), co, SLOT(updateSize(int)));
	QObject::connect(ui->smoothingSlider, SIGNAL(valueChanged(int)), co, SLOT(updateSmoothing(int)));
	QObject::connect(ui->stepSlider, SIGNAL(valueChanged(int)), co, SLOT(updateStep(int)));
}
