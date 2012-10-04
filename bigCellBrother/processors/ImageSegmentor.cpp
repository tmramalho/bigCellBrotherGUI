/*
 * ImageSegmentor.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: tiago
 */

#include "ImageSegmentor.h"

ImageSegmentor::ImageSegmentor() {

}

ImageSegmentor::~ImageSegmentor() {
	// TODO Auto-generated destructor stub
}

cv::Mat ImageSegmentor::watershed(cv::Mat &improvImage, cv::Mat &markers) {
	cv::Mat wsTargetImage;
	cv::cvtColor(improvImage, wsTargetImage, CV_GRAY2BGR);

	cv::watershed( wsTargetImage, markers );

	return markers;
}

cv::Mat ImageSegmentor::addBackgroundMask(cv::Mat& markersPic, cv::Mat& backgroundMask) {
	cv::Mat merged;
	markersPic.copyTo(merged);

	int nl = markersPic.rows;
	int nc = markersPic.cols; //only valid for single channel pic!
	for (int j=0; j<nl; j++) {
		char* bgdata = backgroundMask.ptr<char>(j);
		int * medata = merged.ptr<int>(j);
		for (int i=0; i<nc; i++) {
			if(bgdata[i] != 0) medata[i] = 1;
		}
	}

	return merged;
}

markersCont ImageSegmentor::createMarkers(cv::Mat& origImage, int maxHeight, int maxWidth) {
	int compCount = 0;
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	vector<int> assocCompContour;
	cv::Mat contourSourceImage;
	cv::Mat finalStorage(origImage.size(), CV_8U, cv::Scalar::all(BLACK));
	cv::Mat tempStorage(origImage.size(), CV_8U, cv::Scalar::all(BLACK));
	cv::Mat markers(origImage.size(), CV_32S, cv::Scalar::all(0));

	origImage.copyTo(contourSourceImage);

	cv::findContours(contourSourceImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	for( uint i = 0; i< contours.size(); i++ ) {
		if(hierarchy[i][3] != -1) continue; //it's a hole!
		this->breakLargeContours(tempStorage, contours, hierarchy, i, maxHeight, maxWidth);
		cv::bitwise_or(finalStorage, tempStorage, finalStorage);
	}

	//finalStorage = ImageProcessor::applyMorphologyOp(finalStorage, cv::MORPH_OPEN, 3);

	cv::findContours(finalStorage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	for( uint i = 0; i< contours.size(); i++ ) {
		if(hierarchy[i][3] != -1) continue; //it's a hole!
		assocCompContour.push_back(i);
		compCount++;
		cv::drawContours(markers, contours, i, cv::Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
	}

	markersCont mc;
	mc.markers = markers;
	mc.compCount = compCount;
	mc.contours = contours;
	mc.hierarchy = hierarchy;
	mc.assocCompContour = assocCompContour;

	return mc;
}

void ImageSegmentor::breakLargeContours(cv::Mat &contourStorage, vector<vector<cv::Point> > &contours, vector<cv::Vec4i> &hierarchy,
										int i, int maxHeight, int maxWidth) {
	cv::Rect boundBox = cv::boundingRect(contours[i]);
	cv::RotatedRect box = cv::minAreaRect(contours[i]);
	vector<vector<cv::Point> > contoursTemp;
	vector<cv::Vec4i> hierarchyTemp;
	double hDim, wDim;
	int hits = 0;

	detectHeightWidth(box, &hDim, &wDim);
	contourStorage = cv::Scalar::all(BLACK);
	cv::drawContours(contourStorage, contours, i, cv::Scalar::all(WHITE), -1, 8, hierarchy, INT_MAX);

	if(hDim > maxHeight || wDim > maxWidth) { //contour too big, break it

		cv::Mat contourRoi = contourStorage(boundBox); //select only region of interest
		cv::Mat splitLabels(contourRoi.size(), CV_8U, cv::Scalar::all(BLACK));
		cv::Mat distTrans = ImageProcessor::distanceTransform(contourRoi);

		for (int th = 20; th < 250; th += 10) { //increase threshold for distance transf.
			cv::Mat threshResult = ImageProcessor::threshold(distTrans, th, false);
			cv::Mat newContour; threshResult.copyTo(newContour);
			//find connected components in the thresholded pic
			cv::findContours(newContour, contoursTemp, hierarchyTemp, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
			int cc = contoursTemp.size();
			for (int j = 0; j < cc; j++) {
				cv::RotatedRect boxTemp = cv::minAreaRect(contoursTemp[j]);
				detectHeightWidth(boxTemp, &hDim, &wDim);
				//draw the connected components which obey the criterion
				if (hDim < maxHeight && wDim < maxWidth) {
					cv::drawContours(splitLabels, contoursTemp, j, cv::Scalar::all(WHITE), -1, 8, hierarchyTemp, INT_MAX);
					hits++;
				}
			}
		}

		if(hits == 0) { //failed at breaking it apart
			cv::drawContours(contourStorage, contours, i, cv::Scalar::all(WHITE), -1, 8, hierarchy, INT_MAX);
		} else { //draw the split bit
			splitLabels.copyTo(contourRoi);
		}
	}
}

inline void ImageSegmentor::detectHeightWidth(cv::RotatedRect &box, double *hDim, double *wDim) {
	if(box.size.width > box.size.height) {
		*hDim = box.size.width;
		*wDim = box.size.height;
	} else {
		*hDim = box.size.height;
		*wDim = box.size.width;
	}
}

void ImageSegmentor::removeSmallMarkers(cv::Mat &markersPic, int th) {
	double min, max;
	cv::minMaxLoc(markersPic,&min,&max);
	vector<int> markerSize(max + 2, 0); // there are pixels with value -1

	int nl = markersPic.rows;
	int nc = markersPic.cols; //only valid for single channel pic!
	for (int j=0; j<nl; j++) {
		int* data = markersPic.ptr<int>(j);
		for (int i=0; i<nc; i++) {
			markerSize[data[i]+1]++;
		}
	}

	for (int j=0; j<nl; j++) {
		int* data = markersPic.ptr<int>(j);
		for (int i=0; i<nc; i++) {
			int label = data[i];
			if (label <= 1) { data[i] = 0; continue; } //set border and bg pixels to zero
			if (label > 1 && markerSize[label+1] < th) {
				data[i] = 0;
			}
		}
	}
}

