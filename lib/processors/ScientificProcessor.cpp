/*
 * ScientificProcessor.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: tiago
 */

#include "ScientificProcessor.h"

ScientificProcessor::ScientificProcessor() {
	useFluor = false;
    firstFrame = true;
}

ScientificProcessor::~ScientificProcessor() {

}

void ScientificProcessor::processLabels(int t) {
	double min, max;

	cv::minMaxLoc(previousMarkersPic,&min,&max);
	std::vector<int> labels(max+1, 0);

	cv::minMaxLoc(markersPic,&min,&max);
	cv::Mat currentLabelMask(markersPic.size(), CV_8U);
	std::vector<CellCont> cellVector;

	for(int i = 2; i < max + 1; i++) { //iterate all cell labels
		currentLabelMask = cv::Scalar::all(BLACK);
		cv::compare(markersPic, i, currentLabelMask, cv::CMP_EQ);

		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		CellCont newCell = CellCont::determineLabelProperties(currentLabelMask, markersPic, i);
		newCell.setTime(t);
		//std::cout << "Looking at cell " << i << std::endl;
        CellCont::calcFluorescence(newCell, currentLabelMask, fluorescenceArray);
		if(!firstFrame) {
			int parent = calculateMaxOverlap(newCell, currentLabelMask, labels);
			newCell.setPrevLabel(parent);
		}
        if(newCell.getIsCell()) {
            cellVector.push_back(newCell);
            saveCellInArray(newCell, t);
        }

	}

	if (firstFrame) { firstFrame = false; }

    previousCells.clear();
    previousCells = currentCells;
    currentCells.clear();
    currentCells = cellVector;
    //std::cout << cellVector.size() << " cells found" << std::endl;
}

void ScientificProcessor::printStatistics() {
    std::cout << "There are " << currentCells.size() << " cells here." << std::endl;
    for(unsigned int j = 0; j < currentCells.size(); j++) {
        if(currentCells[j].getPrevLabel() == -1) std::cout << "new cell " << std::endl;
        else {
            std::cout << "daughter of " << currentCells[j].getPrevLabel();
        }
    }
}

void ScientificProcessor::createDotFile(std::fstream &filestr) {
    filestr << "digraph test {" << std::endl;
}

void ScientificProcessor::finishDotFile(std::fstream &filestr) {
    filestr << "}" << std::endl;
    filestr.close();
}

void ScientificProcessor::updateDotFile(std::fstream &filestr, int i) {
    filestr << "{rank=same; ";
    for(unsigned int j = 0; j < currentCells.size(); j++) {
        filestr << "nodet" << i << "L" << currentCells[j].getCurLabel() << " ";
    }
    filestr << "}" << std::endl;
    for(unsigned int j = 0; j < currentCells.size(); j++) {
        if(currentCells[j].getPrevLabel() != -1) {
            //parent
            filestr << "nodet" << i-1 << "L" << currentCells[j].getPrevLabel() << "->";
            //daughter
            filestr << "nodet" << i << "L" << currentCells[j].getCurLabel() << std::endl;
        }
    }
}

void ScientificProcessor::createCsvFile(std::fstream &filestr, unsigned int nfl) {
    /* Header */
    filestr << "t, label, tp, parent, cx, cy, h, w, area, angle, ";
    for(unsigned int i = 0; i < nfl; i++)
        filestr << "flav" << i << ", flsd" << i << ", ";
    filestr << std::endl;
}

void ScientificProcessor::finishCsvFile(std::fstream &filestr) {
    filestr.close();
}

void ScientificProcessor::updateCsvFile(std::fstream &filestr, int i) {
    /* Data */
    for(unsigned int j = 0; j < currentCells.size(); j++) {
        filestr << i << ", " << currentCells[j].getCurLabel() << ", ";
        filestr << currentCells[j].getParentTime() << ", ";
        filestr << currentCells[j].getPrevLabel() << ", ";
        cv::Point2f center = currentCells[j].getCenter();
        filestr << center.x << ", " << center.y << ", ";
        std::vector <double> feats = currentCells[j].getFeatures();
        filestr << feats[0] << ", " << feats[1] << ", " << feats[2] << ", ";
        filestr << currentCells[j].getAngle();
        std::vector <double> flVal = currentCells[j].getFluorescence();
        std::vector <double> flValSq = currentCells[j].getFluorescenceSq();
        for(unsigned int j = 0; j < flVal.size(); j++) {
            filestr << ", " << flVal[j] << ", " << flValSq[j];
        }
        filestr << std::endl;
    }
}

void ScientificProcessor::addFluorescencePic(cv::Mat fluorescencePic, int j) {
    cv::Mat target;
	if(fluorescencePic.rows != markersPic.rows || fluorescencePic.cols != markersPic.cols) {
		std::cerr << "The fluorescence pic is wrong" << std::endl;
		std::cerr << fluorescencePic.rows << ", " << fluorescencePic.cols << std::endl;
		std::cerr << markersPic.rows << ", " << markersPic.cols << std::endl;
	}
    fluorescencePic.convertTo(target, CV_64F, 1, 0);
    /* this assumes the caller always provides the pictures in order.
     * it's not a great idea but I'm sure I will always provide it in order */
    if(fluorescenceArray.size() <= (unsigned int) j) fluorescenceArray.push_back(target);
    else fluorescenceArray[j] = target;
}

cv::Mat ScientificProcessor::getMarkersPic() const {
	return markersPic;
}

void ScientificProcessor::setMarkersPic(cv::Mat markersPic) {
	this->markersPic = markersPic;
}

bool ScientificProcessor::isUseFluor() const {
	return useFluor;
}

void ScientificProcessor::setUseFluor(bool useFluor) {
	this->useFluor = useFluor;
}

int ScientificProcessor::calculateMaxOverlap(CellCont& newCell,
		cv::Mat& currentLabelMask, std::vector<int> &labels) {
	std::fill_n(labels.begin(), labels.size(), 0);
	cv::Rect cellRoi = newCell.getBoundBox();
	cv::Mat cellMask = currentLabelMask(cellRoi);
	cv::Mat prevCells = previousMarkersPic(cellRoi);
	cv::Mat overlap;
	prevCells.copyTo(overlap, cellMask);
	int nl = overlap.rows;
	int nc = overlap.cols;
	for (int j=0; j<nl; j++) {
		int* data = overlap.ptr<int>(j);
		for (int i=0; i<nc; i++) {
			if(data[i] > 1) labels[data[i]]++;
		}
	}

	std::vector<int>::iterator resultPos = std::max_element(labels.begin(), labels.end());
	int result = std::distance(labels.begin(), resultPos);
	if (result > 1) return result;
    else return -1;
}

void ScientificProcessor::saveCellInArray(CellCont &newCell, int t)
{
    std::map<int, std::vector<double> > currentCells = allCells[t];
    int cl = newCell.getCurLabel();
    std::vector<double> cells = currentCells[cl];
    cells.push_back(newCell.getCurLabel());
    cells.push_back(newCell.getParentTime());
    cells.push_back(newCell.getPrevLabel());
    cv::Point2f center = newCell.getCenter();
    cells.push_back(center.x);
    cells.push_back(center.y);
    std::vector <double> feats = newCell.getFeatures();
    cells.push_back(feats[0]);
    cells.push_back(feats[1]);
    cells.push_back(feats[2]);
    cells.push_back(newCell.getAngle());
    std::vector <double> flVal = newCell.getFluorescence();
    std::vector <double> flValSq = newCell.getFluorescenceSq();
    for(unsigned int j = 0; j < flVal.size(); j++) {
         cells.push_back(flVal[j]);
         cells.push_back(flValSq[j]);
    }
    currentCells[cl] = cells;
    allCells[t] = currentCells;
}
std::map<int, std::map<int, std::vector<double> > > ScientificProcessor::getAllCells() const
{
    return allCells;
}

void ScientificProcessor::serializeAllCells(std::fstream &filestr)
{
    boost::archive::text_oarchive oa(filestr);
    oa << allCells;
    filestr.close();
}

void ScientificProcessor::loadAllCells(std::fstream &filestr)
{
    boost::archive::text_iarchive ia(filestr);
    ia >> allCells;
    filestr.close();
}

void ScientificProcessor::saveAllCellsToCsv(std::fstream &filestr)
{
    std::map<int, std::vector<double> > firstTime = allCells.begin()->second;
    std::vector<double> firstCell = firstTime.begin()->second;
    std::cout << firstCell.size() << std::endl;
    int nfl = (firstCell.size() - 9)/2;

    /* Header */
    filestr << "t, label, tp, parent, cx, cy, h, w, area, angle, ";
    for(unsigned int i = 0; i < nfl; i++)
        filestr << "flav" << i << ", flsd" << i << ", ";
    filestr << std::endl;

    /* Data */
    for(std::map<int, std::map<int, std::vector<double> > >::iterator it = allCells.begin();
        it != allCells.end(); ++it) {
        int t = it->first;
        std::map<int, std::vector<double> > currentTime = it->second;
        for(std::map<int, std::vector<double> >::iterator itm = currentTime.begin();
            itm != currentTime.end(); ++itm) {
            std::vector<double> features = itm->second;
            filestr << t << ",";
            for(std::vector<double>::iterator itv = features.begin();
                 itv != features.end(); ++itv) {
                filestr << *itv << ",";
            }
            filestr << std::endl;
        }
    }
    filestr.close();
}

void ScientificProcessor::removeAncestor(int time, int label)
{
    allCells[time][label][2] = -1;
}

void ScientificProcessor::addAncestor(int time, int label, int prevTime, int prevLabel)
{
    allCells[time][label][2] = prevLabel;
    allCells[time][label][1] = prevTime;
}

cv::Mat ScientificProcessor::getPreviousMarkersPic() const {
    return previousMarkersPic;
}

void ScientificProcessor::setPreviousMarkersPic(cv::Mat previousMarkersPic) {
    this->previousMarkersPic = previousMarkersPic;
}

bool ScientificProcessor::isFirstFrame() const {
	return firstFrame;
}

void ScientificProcessor::setFirstFrame(bool firstFrame) {
    this->firstFrame = firstFrame;
}

void ScientificProcessor::reset()
{
    currentCells.clear();
    previousCells.clear();
    fluorescenceArray.clear();
    allCells.clear();
}

std::vector<CellCont> ScientificProcessor::getCurrentCells() const
{
    return currentCells;
}

void ScientificProcessor::setCurrentCells(const std::vector<CellCont> &value)
{
    currentCells = value;
}

std::vector<CellCont> ScientificProcessor::getPreviousCells() const
{
    return previousCells;
}

void ScientificProcessor::setPreviousCells(const std::vector<CellCont> &value)
{
    previousCells = value;
}
