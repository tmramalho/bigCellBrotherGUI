#include "listopenfiles.h"
#include "ui_listopenfiles.h"

ListOpenFiles::ListOpenFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListOpenFiles)
{
    ui->setupUi(this);
}

ListOpenFiles::~ListOpenFiles()
{
    delete ui;
}

void ListOpenFiles::setOp(ExecuteSequence *_ex)
{
    ex = _ex;
    QObject::connect(ex, SIGNAL(brightfieldAdded(std::string)), this, SLOT(addBrightfield(std::string)));
    QObject::connect(ex, SIGNAL(fluorescenceAdded(std::string)), this, SLOT(addFluorescence(std::string)));
}

void ListOpenFiles::addBrightfield(std::string name)
{
    ui->bfList->clear();
    ui->flList->clear();
    QString fn(name.c_str());
    new QListWidgetItem(fn, ui->bfList);
}

void ListOpenFiles::addFluorescence(std::string name)
{
    QString fn(name.c_str());
    new QListWidgetItem(fn, ui->flList);
}
