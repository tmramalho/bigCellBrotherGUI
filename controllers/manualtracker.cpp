#include "manualtracker.h"

ManualTracker::ManualTracker(ExecuteSequence *es, OperationsController *_oc, QObject *parent) :
    QObject(parent)
{
    this->sp = &(es->getLab());
    this->oc = _oc;
    parentLabel = 0;
    parentFrameNumber = 0;
    childFrameNumber = 0;
}

void ManualTracker::changeParentImage(int frameNum, int _parentLabel)
{
    cv::Mat rawFrame = images->grabFrameNumber(frameNum);
    oc->resetPipeline(rawFrame);
    parentMarkers = oc->runFullPipeline();
    curFrame = oc->getPipelineImage(2);
    parentLabel = _parentLabel;
    parentFrameNumber = frameNum;

    QImage updatedFrame = updateParentImage();
    emit newParentFrame(updatedFrame);
}

void ManualTracker::changeChildImage(int frameNum)
{
    cv::Mat rawFrame = images->grabFrameNumber(frameNum);
    oc->resetPipeline(rawFrame);
    childMarkers = oc->runFullPipeline();
    nextFrame = oc->getPipelineImage(2);
    childFrameNumber = frameNum;
    updateChildCells();

    QImage childFrame = updateChildImage();
    emit newChildFrame(childFrame);
}

void ManualTracker::setChildAsParent()
{
    if(parentLabel > 1 && childLabels.size() > 0) {
        changeParentImage(childFrameNumber, childLabels[0]);
    } else {
        changeParentImage(childFrameNumber);
    }
    changeChildImage(childFrameNumber+1);

}

void ManualTracker::setParentAsChild()
{
    if(parentLabel > 1) {
        std::map<int, std::vector<double> > cells;

        try {
            cells = (sp->getAllCells()).at(parentFrameNumber);
        } catch (const std::out_of_range& oor) {
            std::cerr << "Out of Range error: " << oor.what() << '\n';
        }

        //find out who its parent is and go there
        int newParentLabel = cells[parentLabel][1];
        int newParentTime = cells[parentLabel][2];
        int newChildTime = parentFrameNumber;

        changeParentImage(newParentTime, newParentLabel);
        changeChildImage(newChildTime);
    } else {
        changeParentImage(childFrameNumber);
        changeChildImage(childFrameNumber+1);
    }
}

void ManualTracker::curCellPicked(int i, int j, int bt)
{
    int label = parentMarkers.at<int>(i, j);
    std::cout << "Clicked on parent "<< label << std::endl;
    parentLabel = label;
    QImage updatedFrame = updateParentImage();
    emit newParentFrame(updatedFrame);

    updateChildCells();
    QImage childFrame = updateChildImage();
    emit newChildFrame(childFrame);
}

void ManualTracker::nextCellPicked(int i, int j, int bt)
{
    int label = childMarkers.at<int>(i, j);
    std::cout << "Clicked on child "<< label << std::endl;
    if(parentLabel > 1) {
        updateMasterData(label);
    }
}

QImage ManualTracker::convertMatToQt(cv::Mat result)
{
    cv::Mat image;
    if(result.type() == CV_8U)
        cv::cvtColor(result, image, CV_GRAY2RGB);
    else
        cv::cvtColor(result, image, CV_BGR2RGB);

    return QImage((const unsigned char*)(image.data),
                  image.cols, image.rows, image.step, QImage::Format_RGB888);
}

QImage ManualTracker::updateParentImage()
{
    cv::Mat frame;

    if(parentLabel < 2) {
        frame = PictureVis::drawMarkersOnPicture(curFrame, parentMarkers);
    } else {
        frame = PictureVis::highlightMarker(curFrame, parentMarkers, parentLabel);
    }

    return convertMatToQt(frame);
}

QImage ManualTracker::updateChildImage()
{
    cv::Mat frame;

    if (childLabels.size() == 0 || parentLabel < 2) {
        frame = PictureVis::drawMarkersOnPicture(nextFrame, childMarkers);
    } else {
        frame = PictureVis::highlightMarkerVector(nextFrame, childMarkers, childLabels);
    }

    return convertMatToQt(frame);
}

void ManualTracker::updateChildCells()
{
    childLabels.clear();
    if(parentLabel < 2) return;

    std::map<int, std::vector<double> > childCells;

    try {
        childCells = (sp->getAllCells()).at(childFrameNumber);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
    }

    if(childCells.size() != 0) {
        for (std::map<int, std::vector<double> >::iterator it=childCells.begin();
             it!=childCells.end(); ++it) {
            if(it->second[1] == parentLabel && it->second[2] == parentFrameNumber) {
                childLabels.push_back(it->first);
            }
        }
        std::cout << "there are " << childCells.size() << " cells here, and " << childLabels.size()
                  << " are descendants" << std::endl;
    }
}

void ManualTracker::updateMasterData(int label)
{
    if(parentLabel < 2) return;
    if(label < 2) return;

    std::vector<int>::iterator p = std::find(childLabels.begin(), childLabels.end(), label);
    if(p != childLabels.end()) {
        //remove label
        childLabels.erase(p);
        sp->removeAncestor(childFrameNumber, label);
    } else {
        //add label
        childLabels.push_back(label);
        sp->addAncestor(childFrameNumber, label, parentFrameNumber, parentLabel);
    }

    QImage childFrame = updateChildImage();
    emit newChildFrame(childFrame);
}
