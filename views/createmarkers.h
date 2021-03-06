#ifndef CREATEMARKERS_H
#define CREATEMARKERS_H

#include <QtWidgets/QDialog>
#include "createmarkersop.h"

namespace Ui {
class CreateMarkers;
}

class CreateMarkers : public QDialog
{
	Q_OBJECT

public:
	explicit CreateMarkers(QWidget *parent = 0);
	~CreateMarkers();
	void bindToOp(CreateMarkersOp *co);

public slots:
    void updateParameters(const std::map <std::string, double> &param);

private:
	Ui::CreateMarkers *ui;
};

#endif // CREATEMARKERS_H
