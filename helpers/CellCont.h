/*
 * CellCont.h
 *
 *  Created on: Oct 9, 2012
 *	  Author: tiago
 */

#ifndef CELLCONT_H_
#define CELLCONT_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <set>
#include <map>
#include <cstdio>
#include <stack>
#include <iostream>
#include <exception>
#include <algorithm>
#include <utility>
#include "../classifier/CellClassifier.h"
#include "../processors/ImageProcessor.h"


#define PI 3.14159265

class CellCont {
public:
	CellCont();
	virtual ~CellCont();

	std::vector<double> getFeatures() const;
	std::set<int> getNeighbors() const;
	std::vector<double> getProbs() const;
	void setFeatures(std::vector<double> features);
	void setNeighbors(std::set<int> neighbors);
	void setProbs(std::vector<double> probs);
	double getCompactness();
	void removeNeighbor(int neighbor);

	void printCellInfo();

	static bool compareByNumNeighbors(CellCont a, CellCont b);
	static CellCont determineLabelProperties(cv::Mat &currentLabelMask, cv::Mat &markersPic,
											int i, CellClassifier *decider = NULL, int smRadius = 27);
	static void detectHeightWidth(cv::RotatedRect &box, double *hDim, double *wDim);
	static double calcDistance(CellCont &first, CellCont &second);
	static void calcFluorescence(CellCont& target, cv::Mat &currentLabelMask,
								cv::Mat &fluorescencePic);

	int getCurLabel() const;
	void setCurLabel(int curLabel);
	cv::Rect getBoundBox() const;
	void setBoundBox(cv::Rect boundBox);
	cv::Point2f getCenter() const;
	void setCenter(double x, double y);
	int getPrevLabel() const;
	void setPrevLabel(int prevLabel);
	double getTime() const;
	void setTime(double time);
	bool getIsCell() const;
	void setIsCell(bool isCell);
	double getFluorescence() const;
	void setFluorescence(double fluorescence);

private:
	inline static void expandRect(cv::Rect& bbox, int padding, int imgHeight, int imgWidth);
	static std::set<int> findNearestNeigbors(cv::Rect& bbox, cv::Mat& currentLabelMask, cv::Mat& markersPic, int self, int distance);
	std::vector<double> features;
	std::set<int> neighbors;
	std::vector<double> probs;
	cv::Rect boundBox;
	int curLabel;
	bool isCell;
	double fluorescence;
	//tracking stuff
	int prevLabel;
	double time;
	cv::Point2f center;
};

#endif /* CELLCONT_H_ */
