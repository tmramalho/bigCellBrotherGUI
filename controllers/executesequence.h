#ifndef EXECUTESEQUENCE_H
#define EXECUTESEQUENCE_H

#include <QObject>
#include <QFileInfo>
#include <vector>
#include <time.h>
#include "operationscontroller.h"
#include "lib/files/filecontainer.h"
#include "lib/files/videocontainer.h"
#include "lib/files/tiffcontainer.h"
#include "lib/processors/ScientificProcessor.h"
#include "lib/helpers/PictureVis.h"

class ExecuteSequence : public QObject
{
	Q_OBJECT

public:

	ExecuteSequence(OperationsController *opCtr);
    ~ExecuteSequence();
	void debugSequence(int frameNum);
    void setFileSource(FileContainer *source);
    void setFluorFileSource(FileContainer *source);
	void setDotFilename(std::string fn) { dotFilename = fn; }
	void setCsvFilename(std::string fn) { csvFilename = fn; }
	void setAviFilename(std::string fn) { aviFilename = fn; }
    void setSaveFrames(bool f) { saveFrames = f; }

    void run();
    void batchRun(std::vector<QFileInfoList>& stacks);

signals:
    void incrementProgress(float, float);
    void sequenceDone();
    void brightfieldAdded(std::string);
    void fluorescenceAdded(std::string);

public slots:
    void cancelRun();

private:
    OperationsController *ops;
	FileContainer *fs;
    std::vector<FileContainer *> ffs;
    std::string dotFilename;
    std::string csvFilename;
    std::string aviFilename;
	ScientificProcessor lab;
	bool haveFluorescence;
    bool executed;
    bool allowRun;
    bool saveFrames;
    FileContainer *openFile(QFileInfo &fi, bool rescale);
    void analyseFrame(FileContainer *bfPic, std::vector<FileContainer *> &flPics, int i,
                      std::fstream &filestrDOT, std::fstream &filestrCSV);
};

#endif // EXECUTESEQUENCE_H
