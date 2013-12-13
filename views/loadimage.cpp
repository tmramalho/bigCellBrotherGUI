#include "LoadImage.h"
#include "ui_LoadImage.h"

LoadImage::LoadImage(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::LoadImage)
{
	ui->setupUi(this);
    file = NULL;
}

LoadImage::~LoadImage()
{
	delete ui;
}

void LoadImage::bindToFile(FileContainer *fi)
{
    if (file != NULL) {
        QObject::disconnect(ui->maxSlider, SIGNAL(valueChanged(int)), file, SLOT(changeVMax(int)));
        QObject::disconnect(ui->minSlider, SIGNAL(valueChanged(int)), file, SLOT(changeVMin(int)));
        QObject::disconnect(ui->maxBox, SIGNAL(valueChanged(int)), file, SLOT(changeVMax(int)));
        QObject::disconnect(ui->minBox, SIGNAL(valueChanged(int)), file, SLOT(changeVMin(int)));
    }
    file = fi;
    QObject::connect(ui->maxSlider, SIGNAL(valueChanged(int)), file, SLOT(changeVMax(int)));
    QObject::connect(ui->minSlider, SIGNAL(valueChanged(int)), file, SLOT(changeVMin(int)));
    QObject::connect(ui->maxBox, SIGNAL(valueChanged(int)), file, SLOT(changeVMax(int)));
    QObject::connect(ui->minBox, SIGNAL(valueChanged(int)), file, SLOT(changeVMin(int)));
}

void LoadImage::bindToOps() {
    QObject::connect(ui->maxSlider, SIGNAL(valueChanged(int)), ui->maxBox, SLOT(setValue(int)));
    QObject::connect(ui->minSlider, SIGNAL(valueChanged(int)), ui->minBox, SLOT(setValue(int)));
    QObject::connect(ui->maxBox, SIGNAL(valueChanged(int)), ui->maxSlider, SLOT(setValue(int)));
    QObject::connect(ui->minBox, SIGNAL(valueChanged(int)), ui->minSlider, SLOT(setValue(int)));
}
