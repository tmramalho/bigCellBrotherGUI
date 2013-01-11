#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <QDialog>
#include "thresholdop.h"

namespace Ui {
class Threshold;
}

class Threshold : public QDialog
{
	Q_OBJECT

public:
	explicit Threshold(QWidget *parent = 0);
	~Threshold();
	void bindToOp(ThresholdOp *co);

private:
	Ui::Threshold *ui;
};

#endif // THRESHOLD_H
