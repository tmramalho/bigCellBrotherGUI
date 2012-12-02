#ifndef CREATECLASSIFIER_H
#define CREATECLASSIFIER_H

#include <QObject>
#include <iostream>
#include <QEvent>
#include <QMouseEvent>
#include <QScrollArea>
#include <QLabel>
#include "operation.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class CreateClassifier : public QObject, public Operation
{
	Q_OBJECT
public:
	CreateClassifier();
	void execute();
	void createPreview();

public slots:
	void cellPicked(int i, int j, int bt);

signals:
	void cellFeaturesFound(CellCont);

private:
	std::vector< std::vector<double> > featureList;

};

#endif // CREATECLASSIFIER_H
