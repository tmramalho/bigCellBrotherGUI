/*
 * ScientificProcessor.h
 *
 *  Created on: Oct 4, 2012
 *      Author: tiago
 */

#ifndef SCIENTIFICPROCESSOR_H_
#define SCIENTIFICPROCESSOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>
#include <set>
#include <map>
#include <cstdio>
#include <stack>
#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include <utility>
#include "../helpers/CellCont.h"
#include "ImageProcessor.h"

class ScientificProcessor {
public:
	ScientificProcessor();
	virtual ~ScientificProcessor();
	void processLabels(int t);
	void printStatistics();
    void createDotFile(std::fstream &filestr);
    void updateDotFile(std::fstream &filestr, int i);
    void finishDotFile(std::fstream &filestr);
    void createCsvFile(std::fstream &filestr, unsigned int nfl);
    void updateCsvFile(std::fstream &filestr, int i);
    void finishCsvFile(std::fstream &filestr);
    void addFluorescencePic(cv::Mat fluorescencePic, int j);
    cv::Mat getMarkersPic() const;
    void setMarkersPic(cv::Mat markersPic);
	bool isUseFluor() const;
	void setUseFluor(bool useFluor);
	cv::Mat getPreviousMarkersPic() const;
    void setPreviousMarkersPic(cv::Mat previousMarkersPic);
	bool isFirstFrame() const;
	void setFirstFrame(bool firstFrame);
    void reset();

    std::vector<CellCont> getCurrentCells() const;
    void setCurrentCells(const std::vector<CellCont> &value);

    std::vector<CellCont> getPreviousCells() const;
    void setPreviousCells(const std::vector<CellCont> &value);

    std::map<int, std::map<int, std::vector<double> > > getAllCells() const;
    void serializeAllCells(std::fstream &filestr);
    void loadAllCells(std::fstream &filestr);
    void saveAllCellsToCsv(std::fstream &filestr);
    void removeAncestor(int time, int label);
    void addAncestor(int time, int label, int prevTime, int prevLabel);

private:
    int calculateMaxOverlap(CellCont &newCell, cv::Mat &currentLabelMask, std::vector<int> &labels);
    void saveCellInArray(CellCont &newCell, int t);
    std::vector<CellCont> currentCells;
    std::vector<CellCont> previousCells;
    std::map<int, std::map<int, std::vector<double> > > allCells;
	cv::Mat markersPic;
	cv::Mat previousMarkersPic;
    std::vector<cv::Mat> fluorescenceArray;
	bool useFluor;
    bool firstFrame;
};

#endif /* SCIENTIFICPROCESSOR_H_ */
