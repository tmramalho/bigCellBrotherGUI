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
	QObject::connect(ui->goodCells, SIGNAL(pressed()), cc, SLOT(setGoodMode()));
	QObject::connect(ui->badCells,  SIGNAL(pressed()), cc, SLOT(setBadMode()));
}
