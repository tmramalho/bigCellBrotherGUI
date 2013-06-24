#ifndef EXECUTESEQUENCE_H
#define EXECUTESEQUENCE_H

#include <QObject>
#include "operationscontroller.h"
#include "lib/files/filecontainer.h"
#include "lib/processors/ScientificProcessor.h"
#include "lib/helpers/PictureVis.h"

class ExecuteSequence : public QObject
{
	Q_OBJECT

public:

	ExecuteSequence(OperationsController *opCtr);
	void debugSequence(int frameNum);
	void setFileSource(FileContainer *source) { fs = source; }
	void setFluorFileSource(FileContainer *source) { ffs = source; haveFluorescence = true; }
	void setDotFilename(std::string fn) { dotFilename = fn; }
	void setCsvFilename(std::string fn) { csvFilename = fn; }
	void setAviFilename(std::string fn) { aviFilename = fn; }

public slots:
	void run();

signals:
	void incrementProgress(int);
	void sequenceDone();

private:
	OperationsController *ops;
	FileContainer *fs;
	FileContainer *ffs;
	std::string dotFilename;
	std::string csvFilename;
	std::string aviFilename;
	ScientificProcessor lab;
	bool haveFluorescence;
	bool executed;
};

#endif // EXECUTESEQUENCE_H
