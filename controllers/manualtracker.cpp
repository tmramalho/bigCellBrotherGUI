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
    oc->runPipelineUntil("Improve Image");
    curFrame = oc->getPipelineImage(2);
    parentMarkers = generateMarkersForFrame(frameNum, curFrame.size());
    parentLabel = _parentLabel;
    parentFrameNumber = frameNum;

    QImage updatedFrame = updateParentImage();
    emit newParentFrame(updatedFrame);
}

void ManualTracker::changeChildImage(int frameNum)
{
    cv::Mat rawFrame = images->grabFrameNumber(frameNum);
    oc->resetPipeline(rawFrame);
    oc->runPipelineUntil("Improve Image");
    nextFrame = oc->getPipelineImage(2);
    childMarkers = generateMarkersForFrame(frameNum, nextFrame.size());
    childFrameNumber = frameNum;
    updateChildCells();
    updateParentCells();

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
        int newParentLabel = cells[parentLabel][2];
        int newParentTime = cells[parentLabel][1];
        int newChildTime = parentFrameNumber;

        changeParentImage(newParentTime, newParentLabel);
        changeChildImage(newChildTime);
    } else {
        changeParentImage(childFrameNumber);
        changeChildImage(childFrameNumber+1);
    }
}

void ManualTracker::saveLineage(std::string fname)
{
    std::fstream filestr(fname.c_str(), std::fstream::trunc | std::fstream::out);
    sp->saveAllCellsToCsv(filestr);
    emit exportDone();
}

cv::Mat ManualTracker::generateMarkersForFrame(int fnum, cv::Size fsize)
{
    std::map<int, std::vector<double> > cells;
    cv::Mat markers(fsize, CV_32S, cv::Scalar::all(0));

    try {
        cells = (sp->getAllCells()).at(fnum);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
        return markers;
    }

    std::vector< std::vector<cv::Point2f> > contours(1);

    for (std::map<int, std::vector<double> >::iterator it=cells.begin();
         it!=cells.end(); ++it) {
        std::vector<double> curCell = it->second;
        cv::Point2f center(curCell[3], curCell[4]);
        cv::Size2f size(curCell[6], curCell[5]);
        cv::RotatedRect box(center, size, curCell[8]);
        cv::Point2f vertices[4];
        box.points(vertices);
        cv::Point2i intVertices[4];
        for (int i=0; i<4; i++) {
            intVertices[i].x = vertices[i].x;
            intVertices[i].y = vertices[i].y;
        }
        cv::fillConvexPoly(markers, intVertices, 4, curCell[0]);
    }

    return markers;
}

void ManualTracker::curCellPicked(int i, int j, int bt)
{
    int label = parentMarkers.at<int>(i, j);
    std::cout << "Clicked on parent "<< label << "(" << i << ", " << j << ")" << std::endl;
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
    std::cout << "Clicked on child "<< label << "(" << i << ", " << j << ")" << std::endl;
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
        frame = PictureVis::drawParentMarkersOnPicture(curFrame, parentMarkers, parentLabels);
    } else {
        frame = PictureVis::highlightMarker(curFrame, parentMarkers, parentLabel);
    }

    return convertMatToQt(frame);
}

QImage ManualTracker::updateChildImage()
{
    cv::Mat frame;

    std::map<int, std::vector<double> > childCells;

    try {
        childCells = (sp->getAllCells()).at(childFrameNumber);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
    }

    if (childLabels.size() == 0 || parentLabel < 2) {
        frame = PictureVis::drawChildMarkersOnPicture(nextFrame, childMarkers, childCells);
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
            if(it->second[2] == parentLabel && it->second[1] == parentFrameNumber) {
                childLabels.push_back(it->first);
            }
        }
        std::cout << "there are " << childCells.size() << " cells here, and " << childLabels.size()
                  << " are descendants" << std::endl;
    }
}

void ManualTracker::updateParentCells()
{
    parentLabels.clear();
    std::map<int, std::vector<double> > childCells;

    try {
        childCells = (sp->getAllCells()).at(childFrameNumber);
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
    }

    if(childCells.size() != 0) {
        for (std::map<int, std::vector<double> >::iterator it=childCells.begin();
             it!=childCells.end(); ++it) {
            if(it->second[1] == parentFrameNumber) {
                parentLabels.insert(it->second[2]);
            }
        }
    }

    QImage updatedFrame = updateParentImage();
    emit newParentFrame(updatedFrame);
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

    updateParentCells();
}
