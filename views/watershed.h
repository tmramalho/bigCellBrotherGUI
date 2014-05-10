#ifndef WATERSHED_H
#define WATERSHED_H

#include <QtWidgets/QDialog>
#include "watershedop.h"

namespace Ui {
class Watershed;
}

class Watershed : public QDialog
{
	Q_OBJECT

public:
	explicit Watershed(QWidget *parent = 0);
	~Watershed();
	void bindToOp(WatershedOp *co);

private:
	Ui::Watershed *ui;
};

#endif // WATERSHED_H
