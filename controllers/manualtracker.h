#ifndef MANUALTRACKER_H
#define MANUALTRACKER_H

#include <QObject>
#include <stdexcept>
#include "executesequence.h"
#include "operationscontroller.h"
#include "lib/processors/ScientificProcessor.h"
#include "lib/helpers/PictureVis.h"

class ManualTracker : public QObject
{
    Q_OBJECT
public:
    explicit ManualTracker(ExecuteSequence *es, OperationsController *_oc, QObject *parent = 0);
    void setFileSource(FileContainer *vb) { images = vb; }
    void changeParentImage(int frameNum, int _parentLabel=0);
    void changeChildImage(int frameNum);
    void setChildAsParent();
    void setParentAsChild();
    void saveLineage(std::string fname);
    cv::Mat generateMarkersForFrame(int fnum, cv::Size fsize);
    int getParentFrameNumber() { return parentFrameNumber; }
    int getChildFrameNumber() { return childFrameNumber; }

signals:
    void newParentFrame(QImage updatedFrame);
    void newChildFrame(QImage updatedFrame);
    void exportDone();

public slots:
    void curCellPicked(int i, int j, int bt);
    void nextCellPicked(int i, int j, int bt);

private:
    ScientificProcessor *sp;
    OperationsController *oc;
    FileContainer *images;

    QImage convertMatToQt(cv::Mat result);
    QImage updateParentImage();
    QImage updateChildImage();
    void updateChildCells();
    void updateParentCells();
    void updateMasterData(int label);
    std::vector<int> childLabels;
    std::set<int> parentLabels;
    cv::Mat parentMarkers;
    cv::Mat childMarkers;
    cv::Mat curFrame;
    cv::Mat nextFrame;
    int parentLabel;
    int parentFrameNumber;
    int childFrameNumber;
};

#endif // MANUALTRACKER_H
