#include "listclassifier.h"
#include "ui_listclassifier.h"

ListClassifier::ListClassifier(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ListClassifier)
{
	ui->setupUi(this);
}

ListClassifier::~ListClassifier()
{
	delete ui;
}

void ListClassifier::bindToOp(CreateClassifier *cc)
{
	QObject::connect(ui->train, SIGNAL(pressed()), cc, SLOT(applyTrainingSet()));
	QObject::connect(ui->save, SIGNAL(pressed()), cc, SLOT(saveTrainingSamples()));
	QObject::connect(ui->allbad, SIGNAL(pressed()), cc, SLOT(markallBad()));
	QObject::connect(ui->allgood, SIGNAL(pressed()), cc, SLOT(markallGood()));
    QObject::connect(cc, SIGNAL(trainingSetUpdated(int)), this, SLOT(setTrainingSetSize(int)));
    QObject::connect(cc, SIGNAL(SVMTrained(double)), this, SLOT(setSVMTrained(double)));
}

void ListClassifier::setTrainingSetSize(int size)
{
    ui->setSizeLabel->setText(tr("There are %1 cells in the training set").arg(size));
}

void ListClassifier::setSVMTrained(double result)
{
    int percent = (int) (result*100);
    ui->trainedLabel->setText(tr("Classifier ready with %1 \% accuracy").arg(percent));
}
