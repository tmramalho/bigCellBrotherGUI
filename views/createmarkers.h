#ifndef CREATEMARKERS_H
#define CREATEMARKERS_H

#include <QDialog>
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
	
private:
	Ui::CreateMarkers *ui;
};

#endif // CREATEMARKERS_H
