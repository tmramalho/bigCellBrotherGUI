#include "createmarkers.h"
#include "ui_createmarkers.h"

CreateMarkers::CreateMarkers(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CreateMarkers)
{
	ui->setupUi(this);
}

CreateMarkers::~CreateMarkers()
{
	delete ui;
}

void CreateMarkers::bindToOp(CreateMarkersOp *co)
{
	QObject::connect(ui->heightSlider, SIGNAL(valueChanged(int)), co, SLOT(updateHeight(int)));
	QObject::connect(ui->widthSlider, SIGNAL(valueChanged(int)), co, SLOT(updateWidth(int)));
    QObject::connect(ui->heightBox, SIGNAL(valueChanged(int)), co, SLOT(updateHeight(int)));
    QObject::connect(ui->widthBox, SIGNAL(valueChanged(int)), co, SLOT(updateWidth(int)));

    QObject::connect(ui->heightSlider, SIGNAL(valueChanged(int)), ui->heightBox, SLOT(setValue(int)));
    QObject::connect(ui->widthSlider, SIGNAL(valueChanged(int)), ui->widthBox, SLOT(setValue(int)));
    QObject::connect(ui->heightBox, SIGNAL(valueChanged(int)), ui->heightSlider, SLOT(setValue(int)));
    QObject::connect(ui->widthBox, SIGNAL(valueChanged(int)), ui->widthSlider, SLOT(setValue(int)));
}
