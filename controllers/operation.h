#ifndef OPERATION_H
#define OPERATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class OperationsController;

class Operation
{
public:
	Operation();

	virtual void execute()=0;
	virtual void createPreview()=0;
	virtual void updateParameters()=0;
	void showPreview();
	void perform();

protected:
	OperationsController *controller;
};

#endif // OPERATION_H
