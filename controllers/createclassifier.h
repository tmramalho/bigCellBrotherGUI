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
#include "helpers/PictureVis.h"

class CreateClassifier : public QObject, public Operation
{
	Q_OBJECT
public:
	CreateClassifier();
	void execute();
	void createPreview();

public slots:
	void frameChanged(int fr);
	void cellPicked(int i, int j, int bt);
	void setGoodMode();
	void setBadMode();
	void applyTrainingSet();

private:
	bool mode;
	int currentFrame;
	std::map<int, std::set<int> > detectedLabelsbyFrame;
	std::map<int, std::set<int> > rejectedLabelsbyFrame;

};

#endif // CREATECLASSIFIER_H
