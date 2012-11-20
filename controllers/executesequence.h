#ifndef EXECUTESEQUENCE_H
#define EXECUTESEQUENCE_H

#include <QObject>
#include "operationscontroller.h"
#include "filecontainer.h"
#include "processors/ScientificProcessor.h"

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
	void setDistanceCutoff(int dc) { distanceCutoff = dc; }

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
	ScientificProcessor lab;
	int distanceCutoff;
	bool haveFluorescence;
	bool executed;
};

#endif // EXECUTESEQUENCE_H
