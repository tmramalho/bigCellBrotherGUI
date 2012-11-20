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
	void processLabels(int t, CellClassifier *deciderPtr, int distCutoff);
	void printStatistics();
	void createDotFile(std::string filename);
	void createCsvFile(std::string filename);
    cv::Mat getFluorescencePic() const;
    void setFluorescencePic(cv::Mat fluorescencePic);
    cv::Mat getMarkersPic() const;
    void setMarkersPic(cv::Mat markersPic);
	bool isUseFluor() const;
	void setUseFluor(bool useFluor);
	std::vector<std::vector<CellCont> > &getAllCells() { return allCells; };

private:
	std::vector<std::vector<CellCont> > allCells;
	cv::Mat markersPic;
	cv::Mat fluorescencePic;
	bool useFluor;
};

#endif /* SCIENTIFICPROCESSOR_H_ */
