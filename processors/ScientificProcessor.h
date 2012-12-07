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
#include "../classifier/CellClassifier.h"

class ScientificProcessor {
public:
	ScientificProcessor();
	virtual ~ScientificProcessor();
	void processLabels(int t, CellClassifier *deciderPtr);
	void printStatistics();
	void createDotFile(std::string filename);
	void createCsvFile(std::string filename);
    cv::Mat getFluorescencePic() const;
    void setFluorescencePic(cv::Mat fluorescencePic);
    cv::Mat getMarkersPic() const;
    void setMarkersPic(cv::Mat markersPic);
	bool isUseFluor() const;
	void setUseFluor(bool useFluor);
	cv::Mat getPreviousMarkersPic() const;
	void setPreviousMarkersPic(cv::Mat previousMarkersPic);
	std::vector<std::vector<CellCont> > &getAllCells();
	void setAllCells(std::vector<std::vector<CellCont> > allCells);
	bool isFirstFrame() const;
	void setFirstFrame(bool firstFrame);

private:
	int calculateMaxOverlap(CellCont &newCell, cv::Mat &currentLabelMask, std::vector<int> &labels);
	std::vector<std::vector<CellCont> > allCells;
	cv::Mat markersPic;
	cv::Mat previousMarkersPic;
	cv::Mat fluorescencePic;
	bool useFluor;
	bool firstFrame;
};

#endif /* SCIENTIFICPROCESSOR_H_ */
