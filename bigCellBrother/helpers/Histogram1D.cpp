/*
 * Histogram1D.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: tiago
 */

#include "Histogram1D.h"

Histogram1D::Histogram1D() {
	histSize[0]= 256;
	hranges[0]= 0.0;
	hranges[1]= 255.0;
	ranges[0]= hranges;
	channels[0]= 0; // by default, we look at channel 0

}

Histogram1D::~Histogram1D() {
	// TODO Auto-generated destructor stub
}

cv::MatND Histogram1D::getHistogram(const cv::Mat& image) {
	cv::MatND hist;

	// Compute histogram
	cv::calcHist(&image,
		1,	// histogram from 1 image only
		channels, // the channel used
		cv::Mat(), // no mask is used
		hist,	// the resulting histogram
		1,	// it is a 1D histogram
		histSize, // number of bins
		ranges	// pixel value range
	);

	return hist;
}

// Computes the 1D histogram and returns an image of it.
cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image) {
	// Compute histogram first
	cv::MatND hist= getHistogram(image);
	// Get min and max bin values
	double maxVal=0;
	double minVal=0;
	cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);
	// Image on which to display histogram
	cv::Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(255));
	// set highest point at 90% of nbins
	int hpt = static_cast<int>(0.9*histSize[0]);
	// Draw a vertical line for each bin
	for( int h = 0; h < histSize[0]; h++ ) {
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt/maxVal);
		// This function draws a line between 2 points
		cv::line(histImg,cv::Point(h,histSize[0]),cv::Point(h,histSize[0]-intensity), cv::Scalar::all(0));
	}
	return histImg;
}

cv::Mat Histogram1D::applyLookUp(const cv::Mat& image, const cv::Mat& lookup) {
	cv::Mat result;
	cv::LUT(image, lookup, result);
	return result;
}

cv::Mat Histogram1D::stretch(const cv::Mat &image, int minValue) {
	// Compute histogram first
	cv::MatND hist= getHistogram(image);

	// find left extremity of the histogram
	int imin= 0;
	for( ; imin < histSize[0]; imin++ ) {
		//std::cout<<hist.at<float>(imin)<<std::endl;
		if (hist.at<float>(imin) > minValue)
			break;
		}

	// find right extremity of the histogram
	int imax= histSize[0]-1;
	for( ; imax >= 0; imax-- ) {
		if (hist.at<float>(imax) > minValue) break;
	}

	// Create lookup table
	int dim(256);
	cv::Mat lookup(1, // 1 dimension
			&dim,	// 256 entries
			CV_8U);	// uchar

	// Build lookup table
	for (int i=0; i<256; i++) {
		// stretch between imin and imax
		if (i < imin) lookup.at<uchar>(i)= 0;
		else if (i > imax) lookup.at<uchar>(i)= 255; // linear mapping
		else lookup.at<uchar>(i)= static_cast<uchar>(
				255.0*(i-imin)/(imax-imin)+0.5);
	}

	// Apply lookup table
	cv::Mat result;
	result= applyLookUp(image,lookup);

	return result;
}

cv::Mat Histogram1D::invert(const cv::Mat &image) {
	// Create lookup table
	int dim(256);
	cv::Mat lookup(1, // 1 dimension
			&dim,	// 256 entries
			CV_8U);	// uchar

	// Build lookup table
	for (int i=0; i<256; i++) {
		lookup.at<uchar>(i)= static_cast<uchar>(255-i);
	}

	// Apply lookup table
	cv::Mat result;
	result= applyLookUp(image,lookup);

	return result;
}
