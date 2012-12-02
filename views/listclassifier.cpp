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

}

void ListClassifier::cellFeaturesAccepted(CellCont selectedCell)
{
	//selectedCell.printCellInfo();
}
