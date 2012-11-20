/*
 * ImageSegmentor.h
 *
 *  Created on: Jul 4, 2012
 *	  Author: tiago
 */

#ifndef IMAGESEGMENTOR_H_
#define IMAGESEGMENTOR_H_

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
#include "../helpers/markersCont.h"
#include "../helpers/CellCont.h"
#include "ImageProcessor.h"
#include "../classifier/CellClassifier.h"

#define WHITE 255
#define BLACK 0

class ImageSegmentor {
public:
	ImageSegmentor();
	~ImageSegmentor();

	void createMarkers(cv::Mat& origImage, int maxHeight, int maxWidth);
	void watershed();
	void removeSmallMarkers(int th);
	void findCellMarkers(CellClassifier *deciderPtr);
	void smoothLabels(int kernelSize);

	//getters and setters
	cv::Mat getBoostedImage() const;
	cv::Mat getMarkersPic() const;
	cv::Mat getOriginalImage() const;
	cv::Mat getBackgroundMask() const;
	markersCont getWatershedMarkers() const;
	void setBoostedImage(cv::Mat &boostedImage);
	void setOriginalImage(cv::Mat &originalImage);
	void setMarkersPic(cv::Mat &markersPic);
	void setWatershedMarkers(markersCont watershedMarkers);
	void setBackgroundMask(cv::Mat &backgroundMask);

private:
	void addBackgroundMask();
	void breakLargeContours(cv::Mat &contourStorage, cv::Mat laplace, vector<vector<cv::Point> > &contours, vector<cv::Vec4i> &hierarchy,
			int i, int maxHeight, int maxWidth);
	double calcMergedScore(cv::Mat &currentLabelMask, int neighborLabel);
	CellCont mergeLabels(cv::Mat &currentLabelMask, int neighborLabel, int currentLabel);
	void removeLabel(cv::Mat &currentLabelMask);
	void clearBorderValues();

	cv::Mat markersPic;
	cv::Mat originalImage;
	cv::Mat boostedImage;
	cv::Mat backgroundMask;
	markersCont watershedMarkers;

	CellClassifier *decider;

	int mergeSmoothRadius;
	int boxPadding;
};

#endif /* IMAGESEGMENTOR_H_ */
