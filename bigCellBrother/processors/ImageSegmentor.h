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
#include <opencv2/ml/ml.hpp>

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

	//segmentation stuff
	cv::Mat drawMarkers(const markersCont &mc);
	markersCont makeNiceMarkers(cv::Mat& origImage, int defectSize, int maxHeight, int maxWidth);
	cv::Mat addBackgroundMask(cv::Mat &markersPic, cv::Mat &backgroundMask);
	cv::Mat watershed(cv::Mat &improvImage, cv::Mat &markers);

	static vector<double> calcRatios(const cv::Mat &cellMask, vector<cv::RotatedRect> &boxes, int cellPixels);
	static void drawRotatedRect(cv::Mat &mask, vector<cv::RotatedRect> &boxes);
	static void drawRotatedRect(cv::Mat &mask, cv::RotatedRect &boxes);

private:
	vector<cv::Vec3b> getRandomColorTab(int numComp);
	void drawMinAreaRect(cv::Mat &target, cv::RotatedRect &box, cv::Vec3b &color);
};

#endif /* IMAGESEGMENTOR_H_ */
