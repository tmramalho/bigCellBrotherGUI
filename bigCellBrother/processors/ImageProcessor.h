/*
 * ImageProcessor.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tiago
 */

#ifndef IMAGEPROCESSOR_H_
#define IMAGEPROCESSOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include <vector>
#include <cstdio>
#include <iostream>
#include <exception>
#include "../helpers/Histogram1D.h"

#define WHITE 255
#define BLACK 0

using std::vector;

class ImageProcessor {
public:
	ImageProcessor();
	~ImageProcessor();

	//basic image processing stuff
	static cv::Mat simplifyImage(cv::Mat &origImage, int blurWindow, int stretchMinVal, int equalize = false);
	static cv::Mat threshold(cv::Mat &origImage, int thresholdVal, bool invert);
	static cv::Mat adaptiveThreshold(cv::Mat &origImage, int thresholdVal, int adaptiveWindow, bool invert);
	static cv::Mat applyMorphologyOp(cv::Mat &origImage, int operation, int kernelSize);
	static cv::Mat floodBackground(cv::Mat &origImage, int x, int y, int bound);
	static cv::Mat cannyOp(cv::Mat &origImage, int ub, int lb, int windowSize);
	static cv::Mat invertImage(cv::Mat &origImage);
	static cv::Mat getHistogram(cv::Mat &origImage);
	static cv::Mat distanceTransform(cv::Mat &origImage);
	static cv::Mat distanceTransformWithVoronoi(cv::Mat &origImage, cv::Mat &voronoiImage);
	static cv::Mat erode(cv::Mat &targetImage, int kernelSize);
	static cv::Mat dilate(cv::Mat &targetImage, int kernelSize);
	static cv::Mat laplacian(cv::Mat &targetImage, int kernelSize);
	static cv::Mat harrisCorners(cv::Mat &targetImage, int blockSize, double k, double threshold);
	static bool checkIfEmpty(cv::Mat &origImage);
};

#endif /* IMAGEPROCESSOR_H_ */
