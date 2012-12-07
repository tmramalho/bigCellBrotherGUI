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

void ScientificProcessor::processLabels(int t, CellClassifier *deciderPtr) {
	double min, max;

	cv::minMaxLoc(previousMarkersPic,&min,&max);
	std::vector<int> labels(max+1, 0);

	cv::minMaxLoc(markersPic,&min,&max);
	cv::Mat currentLabelMask(markersPic.size(), CV_8U);
	std::vector<CellCont> cellVector;
	CellClassifier *decider = deciderPtr;

	for(int i = 2; i < max + 1; i++) { //iterate all cell labels
		currentLabelMask = cv::Scalar::all(BLACK);
		cv::compare(markersPic, i, currentLabelMask, cv::CMP_EQ);

		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		CellCont newCell = CellCont::determineLabelProperties(currentLabelMask, markersPic, i, decider);
		newCell.setTime(t);
		if(useFluor) CellCont::calcFluorescence(newCell, currentLabelMask, fluorescencePic);
		if(!firstFrame) {
			int parent = calculateMaxOverlap(newCell, currentLabelMask, labels);
			newCell.setPrevLabel(parent);
		}
		if(newCell.getIsCell()) cellVector.push_back(newCell);
	}

	if (firstFrame) { firstFrame = false; }

	allCells.push_back(cellVector);
}

void ScientificProcessor::printStatistics() {
	for(uint i = 0; i < allCells.size(); i++) {
		std::vector<CellCont> currCells = allCells[i];
		std::cout << "There are " << currCells.size() << " cells here." << std::endl;
		for(uint j = 0; j < currCells.size(); j++) {
			if(currCells[j].getPrevLabel() == -1) std::cout << "new cell " << std::endl;
			else {
				std::vector<double> pr = currCells[j].getProbs();
				std::cout << "daughter of " << currCells[j].getPrevLabel();
				std::cout << "prob " << pr[0] << std::endl;
			}
		}
	}
}

void ScientificProcessor::createDotFile(std::string filename) {
	std::fstream filestr(filename.c_str(), std::fstream::trunc | std::fstream::out);

	filestr << "digraph test {" << std::endl;
	for(uint i = 0; i < allCells.size(); i++) {
		std::vector<CellCont> currCells = allCells[i];
		filestr << "{rank=same; ";
		for(uint j = 0; j < currCells.size(); j++) {
			filestr << "nodet" << i << "L" << currCells[j].getCurLabel() << " ";
		}
		filestr << "}" << std::endl;
		for(uint j = 0; j < currCells.size(); j++) {
			if(currCells[j].getPrevLabel() != -1) {
				//parent
				filestr << "nodet" << i-1 << "L" << currCells[j].getPrevLabel() << "->";
				//daughter
				filestr << "nodet" << i << "L" << currCells[j].getCurLabel() << std::endl;
			}
		}
	}
	filestr << "}" << std::endl;
	filestr.close();
}

void ScientificProcessor::createCsvFile(std::string filename) {
	std::fstream filestr(filename.c_str(), std::fstream::trunc | std::fstream::out);
	filestr << "t, label, cx, cy, h, w, area, fl" << std::endl;
	for(uint i = 0; i < allCells.size(); i++) {
		std::vector<CellCont> currCells = allCells[i];
		for(uint j = 0; j < currCells.size(); j++) {
			filestr << i << ", " << currCells[j].getCurLabel() << ", ";
			cv::Point2f center = currCells[j].getCenter();
			filestr << center.x << ", " << center.y << ", ";
			std::vector <double> feats = currCells[j].getFeatures();
			filestr << feats[0] << ", " << feats[1] << ", " << feats[2] << ", ";
			filestr << currCells[j].getFluorescence() << std::endl;
		}
	}

	filestr.close();
}

cv::Mat ScientificProcessor::getFluorescencePic() const {
	return fluorescencePic;
}

void ScientificProcessor::setFluorescencePic(cv::Mat fluorescencePic) {
	fluorescencePic.convertTo(this->fluorescencePic, CV_64F, 1, 0);
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

cv::Mat ScientificProcessor::getPreviousMarkersPic() const {
	return previousMarkersPic;
}

void ScientificProcessor::setPreviousMarkersPic(cv::Mat previousMarkersPic) {
	this->previousMarkersPic = previousMarkersPic;
}

std::vector<std::vector<CellCont> > &ScientificProcessor::getAllCells() {
	return allCells;
}

void ScientificProcessor::setAllCells(
		std::vector<std::vector<CellCont> > allCells) {
	this->allCells = allCells;
}

bool ScientificProcessor::isFirstFrame() const {
	return firstFrame;
}

void ScientificProcessor::setFirstFrame(bool firstFrame) {
	this->firstFrame = firstFrame;
}








