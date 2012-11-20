#include "operation.h"

Operation::Operation()
{
}

void Operation::perform()
{
	if(!controller->pipelineReady) return;
	this->execute();
	this->showPreview();
}

void Operation::showPreview()
{
	this->createPreview();
	controller->showCurrentPreview();
}
