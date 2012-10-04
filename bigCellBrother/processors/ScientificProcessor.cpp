/*
 * ScientificProcessor.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: tiago
 */

#include "ScientificProcessor.h"

ScientificProcessor::ScientificProcessor() {
	// TODO Auto-generated constructor stub

}

ScientificProcessor::~ScientificProcessor() {
	// TODO Auto-generated destructor stub
}

cv::Mat ScientificProcessor::processLabels(cv::Mat &markersPic) {
	double min, max;
	cv::minMaxLoc(markersPic,&min,&max);
	cv::Mat smoothedMarkers(markersPic.size(), CV_32S, cv::Scalar::all(0));
	cv::Mat currentLabel(markersPic.size(), CV_8U, cv::Scalar::all(0));
	int nl = markersPic.rows;
	int nc = markersPic.cols; //only valid for single channel pic!

	for(int i = 2; i < max + 1; i++) { //iterate all cell labels
		int ni = 0;
		for (int j=0; j<nl; j++) { // detect current label pixels
			int* data = markersPic.ptr<int>(j);
			char *target = currentLabel.ptr<char>(j);
			for (int k=0; k<nc; k++) {
				if(data[k] == i) {
					target[k] = WHITE;
					ni++;
				}
			}
		}

		if (ni == 0) continue;

		currentLabel = ImageProcessor::applyMorphologyOp(currentLabel, cv::MORPH_CLOSE, 5);
		currentLabel = ImageProcessor::applyMorphologyOp(currentLabel, cv::MORPH_OPEN, 5);

		//do scientific stuff here

		for (int j=0; j<nl; j++) { //draw smooth markers just in case
			int* data = smoothedMarkers.ptr<int>(j);
			char *target = currentLabel.ptr<char>(j);
			for (int k=0; k<nc; k++) {
				if(target[k] != 0) data[k] = i;
			}
		}

		currentLabel = cv::Scalar::all(0);
	}

	return smoothedMarkers;
}
