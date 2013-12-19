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
#include "lib/helpers/PictureVis.h"
#include "lib/classifier/SVMachine.h"
#include "lib/helpers/CellCont.h"

class CreateClassifier : public QObject, public Operation
{
	Q_OBJECT
public:
	CreateClassifier(OperationsController* _controller);
	void execute();
	void createPreview();
	void updateParameters();

    void applyTrainingSet();

public slots:
	void frameChanged(int fr);
	void cellPicked(int i, int j, int bt);
    void saveTrainingSamples();
	void markallGood();
	void markallBad();
    void reset();

signals:
    void trainingSetUpdated(int size);
    void SVMTrained(double result);

private:
	int currentFrame;
    std::map<std::pair<int, int>, std::vector<double> > goodFeatures;
    std::map<std::pair<int, int>, std::vector<double> > badFeatures;
	std::map<int, std::set<int> > svmGoodLabelsbyFrame;
	std::map<int, std::set<int> > svmBadLabelsbyFrame;
	std::map<int, std::set<int> > goodLabelsbyFrame;
	std::map<int, std::set<int> > badLabelsbyFrame;
	SVMachine *decider;
};

#endif // CREATECLASSIFIER_H
