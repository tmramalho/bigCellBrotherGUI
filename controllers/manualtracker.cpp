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

QImage ManualTracker::changeParentImage(int frameNum)
{
    cv::Mat rawFrame = images->grabFrameNumber(frameNum);
    oc->resetPipeline(rawFrame);
    parentMarkers = oc->runFullPipeline();
    curFrame = oc->getPipelineImage(2);
    parentLabel = 0;
    parentFrameNumber = frameNum;

    return updateParentImage();
}

QImage ManualTracker::changeChildImage(int frameNum)
{
    cv::Mat rawFrame = images->grabFrameNumber(frameNum);
    oc->resetPipeline(rawFrame);
    childMarkers = oc->runFullPipeline();
    nextFrame = oc->getPipelineImage(2);
    childFrameNumber = frameNum;

    return updateChildImage();
}

void ManualTracker::curCellPicked(int i, int j, int bt)
{
    std::cout << "Parent chosen " << i << ", " << j << ", " << bt << std::endl;
    int label = parentMarkers.at<int>(i, j);
    std::cout << parentLabel << std::endl;
    parentLabel = label;
    QImage updatedFrame = updateParentImage();
    emit newParentFrame(updatedFrame);
}

void ManualTracker::nextCellPicked(int i, int j, int bt)
{
    std::cout << "Child chosen " << i << ", " << j << ", " << bt << std::endl;
    int label = childMarkers.at<int>(i, j);
    std::cout << label << std::endl;
}

QImage ManualTracker::convertMatToQt(cv::Mat result)
{
    cv::Mat image;
    std::cout << result.type() << std::endl;
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
    std::map<int, std::vector<double> > childCells;
    cv::Mat frame;

    try {
        childCells = (sp->getAllCells()).at(childFrameNumber);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
    }
    std::cout << childFrameNumber << " : " << childCells.size() << std::endl;
    if (childCells.size() == 0 || parentLabel < 2) {
        frame = PictureVis::drawMarkersOnPicture(nextFrame, childMarkers);
    } else {
        std::vector<int> childLabels;
        for (std::map<int, std::vector<double> >::iterator it=childCells.begin();
             it!=childCells.end(); ++it) {
            if(it->second[1] == parentLabel && it->second[2] == parentFrameNumber) {
                childLabels.push_back(it->first);
            }
        }
        frame = PictureVis::highlightMarkerVector(nextFrame, childMarkers, childLabels);
        std::cout << "there are " << childCells.size() << "cells here, and " << childLabels.size()
                  << "are descendants" << std::endl;
    }



    return convertMatToQt(frame);
}
