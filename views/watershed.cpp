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
    QObject::connect(ui->sizeBox, SIGNAL(valueChanged(int)), co, SLOT(updateSize(int)));
    QObject::connect(ui->smoothingBox, SIGNAL(valueChanged(int)), co, SLOT(updateSmoothing(int)));
    QObject::connect(ui->stepBox, SIGNAL(valueChanged(int)), co, SLOT(updateStep(int)));
    QObject::connect(ui->sizeSlider, SIGNAL(valueChanged(int)), ui->sizeBox, SLOT(setValue(int)));
    QObject::connect(ui->smoothingSlider, SIGNAL(valueChanged(int)), ui->smoothingBox, SLOT(setValue(int)));
    QObject::connect(ui->stepSlider, SIGNAL(valueChanged(int)), ui->stepBox, SLOT(setValue(int)));
    QObject::connect(ui->sizeBox, SIGNAL(valueChanged(int)), ui->sizeSlider, SLOT(setValue(int)));
    QObject::connect(ui->smoothingBox, SIGNAL(valueChanged(int)), ui->smoothingSlider, SLOT(setValue(int)));
    QObject::connect(ui->stepBox, SIGNAL(valueChanged(int)), ui->stepSlider, SLOT(setValue(int)));

	QObject::connect(ui->checkBox, SIGNAL(toggled(bool)), co, SLOT(removeBorder(bool)));
}
