#ifndef OPERATION_H
#define OPERATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "operationscontroller.h"

class OperationsController;

class Operation
{
public:
	Operation();

	virtual void execute()=0;
	virtual void createPreview()=0;
	void showPreview();
	void perform();

	OperationsController *controller;
};

#endif // OPERATION_H
