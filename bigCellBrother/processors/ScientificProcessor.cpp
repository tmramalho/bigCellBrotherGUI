/*
 * ScientificProcessor.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: tiago
 */

#include "ScientificProcessor.h"

ScientificProcessor::ScientificProcessor() {
	useFluor = false;

}

ScientificProcessor::~ScientificProcessor() {

}

void ScientificProcessor::processLabels(int t, CellClassifier *deciderPtr, int distCutoff) {
	double min, max;
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
		if(newCell.getIsCell()) cellVector.push_back(newCell);
	}

	if(t != 0) {
		double starterDist = (distCutoff > 0)?(double)distCutoff:1e100;
		for(std::vector<CellCont>::iterator it = cellVector.begin(); it != cellVector.end(); it++) {
			double minDist = starterDist;
			int minParent = -1;
			std::vector<CellCont> prevCells = allCells[t-1];
			for(std::vector<CellCont>::iterator jt = prevCells.begin(); jt != prevCells.end(); jt++) {
				double distance = CellCont::calcDistance(*it, *jt);
				if(distance < minDist) {
					minDist = distance;
					minParent = jt->getCurLabel();
				}
			}
			it->setPrevLabel(minParent);
		}
	}

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

	for(uint i = 0; i < allCells.size(); i++) {
		std::vector<CellCont> currCells = allCells[i];
		for(uint j = 0; j < currCells.size(); j++) {
			filestr << "nodet" << i << "L" << currCells[j].getCurLabel() << ", ";
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





