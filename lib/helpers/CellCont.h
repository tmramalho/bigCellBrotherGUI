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
#include "../processors/ImageProcessor.h"

#define NUMFEATURES 11
#define PI 3.14159265

class CellCont {
public:
	CellCont();
	virtual ~CellCont();

	std::vector<double> getFeatures() const;
	std::set<int> getNeighbors() const;
	void setFeatures(std::vector<double> features);
	void setNeighbors(std::set<int> neighbors);
	double getCompactness();
	void removeNeighbor(int neighbor);

	void printCellInfo();

	static bool compareByNumNeighbors(CellCont a, CellCont b);
	static CellCont determineLabelProperties(cv::Mat &currentLabelMask, cv::Mat &markersPic,
											int i, int smRadius = 27);
	static void detectHeightWidth(cv::RotatedRect &box, double *hDim, double *wDim);
	static double calcDistance(CellCont &first, CellCont &second);
    static void calcFluorescence(CellCont& target, cv::Mat &currentLabelMask,
                                std::vector<cv::Mat> &fluorescenceArr);

	int getCurLabel() const;
	void setCurLabel(int curLabel);
	cv::Rect getBoundBox() const;
	void setBoundBox(cv::Rect boundBox);
	cv::Point2f getCenter() const;
	void setCenter(double x, double y);
	int getPrevLabel() const;
	void setPrevLabel(int prevLabel);
    int getTime() const;
    void setTime(int time);
    void setNewParent(int label, int time);
	bool getIsCell() const;
    void setIsCell(bool isCell);
    std::vector<double> getFluorescence() const;
    void setFluorescence(double fluorescence, int j);
	static int getNumFeatures() { return NUMFEATURES; }
	double getAngle() const { return angle; }
	void setAngle(double angle) { this->angle = angle; }
    std::vector<double> getFluorescenceSq() const;
    void setFluorescenceSq(const std::vector<double> &value);

    int getParentTime() const;
    void setParentTime(int value);

private:
    inline static void expandRect(cv::Rect& bbox, int padding, int imgHeight, int imgWidth);
    static std::set<int> findNearestNeigbors(cv::Rect& bbox, cv::Mat& currentLabelMask, cv::Mat& markersPic, int self, int distance);
	std::vector<double> features;
	std::set<int> neighbors;
	cv::Rect boundBox;
	int curLabel;
	bool isCell;
    std::vector<double> fluorescence;
    std::vector<double> fluorescenceSq;
	//tracking stuff
	int prevLabel;
    int time;
    int parentTime;
	double angle;
    cv::Point2f center;
};

#endif /* CELLCONT_H_ */
