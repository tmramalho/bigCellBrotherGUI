/*
 * ImageSegmentor.h
 *
 *  Created on: Jul 4, 2012
 *      Author: tiago
 */

#ifndef IMAGESEGMENTOR_H_
#define IMAGESEGMENTOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <cstdio>
#include <iostream>
#include <exception>
#include "../helpers/markersCont.h"
#include "ImageProcessor.h"

#define WHITE 255
#define BLACK 0

using std::vector;

class ImageSegmentor {
public:
	ImageSegmentor();
	~ImageSegmentor();

	markersCont createMarkers(cv::Mat& origImage, int maxHeight, int maxWidth);
	cv::Mat addBackgroundMask(cv::Mat &markersPic, cv::Mat &backgroundMask);
	cv::Mat watershed(cv::Mat &improvImage, cv::Mat &markers);
	void removeSmallMarkers(cv::Mat &markersPic, int th);

private:
	void breakLargeContours(cv::Mat &contourStorage, vector<vector<cv::Point> > &contours, vector<cv::Vec4i> &hierarchy,
			int i, int maxHeight, int maxWidth);
	inline static void detectHeightWidth(cv::RotatedRect &box, double *hDim, double *wDim);
};

#endif /* IMAGESEGMENTOR_H_ */
