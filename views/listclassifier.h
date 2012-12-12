#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "createclassifier.h"

namespace Ui {
class ListClassifier;
}

class ListClassifier : public QDialog
{
	Q_OBJECT
	
public:
	explicit ListClassifier(QWidget *parent = 0);
	~ListClassifier();
	void bindToOp(CreateClassifier *cc);
	
private:
	Ui::ListClassifier *ui;
};

#endif // DIALOG_H
