#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFuture>
#include <QtConcurrentRun>
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

public slots:
    void setTrainingSetSize(int size);
    void setSVMTrained(double result);

private slots:
    void on_train_released();

private:
	Ui::ListClassifier *ui;
    CreateClassifier *ccl;
};

#endif // DIALOG_H
