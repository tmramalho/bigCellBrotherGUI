/*
 * Histogram1D.h
 *
 *  Created on: Jul 4, 2012
 *      Author: tiago
 */

#ifndef HISTOGRAM1D_H_
#define HISTOGRAM1D_H_

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Histogram1D {

private:
	int histSize[1]; // number of bins
	float hranges[2]; // min and max pixel value
	const float* ranges[1];
	int channels[1]; // only 1 channel used here

public:
	Histogram1D();
	~Histogram1D();
	cv::MatND getHistogram(const cv::Mat &image);
	cv::Mat getHistogramImage(const cv::Mat &image);
	cv::Mat stretch(const cv::Mat &image, int minValue = 0);
	cv::Mat applyLookUp(const cv::Mat& image, const cv::Mat& lookup);
	cv::Mat invert(const cv::Mat &image);
};

#endif /* HISTOGRAM1D_H_ */
