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
}
