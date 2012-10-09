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
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	vector<int> contourList;
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

	int nc = 0;
	for( uint i = 0; i< contours.size(); i++ ) {
		if(hierarchy[i][3] != -1) continue; //it's a hole!
		contourList.push_back(i);
		nc++;
		cv::drawContours(markers, contours, i, cv::Scalar::all(nc+1), -1, 8, hierarchy, INT_MAX);
	}

	markersCont mc;
	mc.markers = markers;
	mc.contours = contours;
	mc.hierarchy = hierarchy;
	mc.contourList = contourList;

	return mc;
}

void ImageSegmentor::breakLargeContours(cv::Mat &contourStorage, vector<vector<cv::Point> > &contours, vector<cv::Vec4i> &hierarchy,
										int i, int maxHeight, int maxWidth) {
	cv::Rect boundBox = cv::boundingRect(contours[i]);
	cv::RotatedRect box = cv::minAreaRect(contours[i]);
	vector<vector<cv::Point> > ctours;
	vector<cv::Vec4i> hrchy;
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
			cv::findContours(newContour, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
			int cc = ctours.size();
			for (int j = 0; j < cc; j++) {
				cv::RotatedRect boxTemp = cv::minAreaRect(ctours[j]);
				detectHeightWidth(boxTemp, &hDim, &wDim);
				//draw the connected components which obey the criterion
				if (hDim < maxHeight && wDim < maxWidth) {
					cv::drawContours(splitLabels, ctours, j, cv::Scalar::all(WHITE), -1, 8, hrchy, INT_MAX);
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

cv::Mat ImageSegmentor::findCellMarkers(cv::Mat& improvImage,
		cv::Mat& markersPic) {
	double min, max, mi, ma;
	cv::minMaxLoc(markersPic,&min,&max);
	cv::Mat currentLabelMask(markersPic.size(), CV_8U);
	cv::Mat currentLabelCtour;
	std::set<int> ngb;
	std::set<int>::iterator it;
	vector<vector<cv::Point> > ctours;
	vector<cv::Vec4i> hrchy;
	std::vector<double> features (2);
	CellClassifier decider;
	decider.setHeight(40);
	decider.setWidth(12);
	decider.setHeightSigma(100);
	decider.setWidthSigma(16);

	for(int i = 2; i < max + 1; i++) { //iterate all cell labels
		currentLabelMask = cv::Scalar::all(BLACK);
		cv::compare(markersPic, i, currentLabelMask, cv::CMP_EQ);

		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;

		currentLabelMask.copyTo(currentLabelCtour);
		cv::findContours(currentLabelCtour, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
		cv::RotatedRect box = cv::minAreaRect(ctours[0]);
		cv::Rect bbox = cv::boundingRect(ctours[0]);

		//save cell features
		detectHeightWidth(box, &mi, &ma);
		features[0] = mi;
		features[1] = ma;
		double pr = decider.calculateLogProbFeatures(features, 1);
		std::cout << i << " : " << pr << " woot ";

		//save cell neighbors
		expandRect(bbox, 20, markersPic.rows, markersPic.cols);
		ngb = findNearestNeigbors(bbox, markersPic, currentLabelMask, i, 5);

		//save stuff in CellCont, setup arrays at the beginning

		for (it=ngb.begin(); it!=ngb.end(); it++)
			std::cout << " " << *it;

		std::cout << std::endl;

		cv::imshow("curr label", currentLabelMask); //DEBUG
		cv::waitKey(0);

	}

	return markersPic;
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

std::set<int> ImageSegmentor::findNearestNeigbors(cv::Rect &bbox,
		cv::Mat &markersPic, cv::Mat &currentLabelMask, int self, int distance) {
	cv::Mat maskRoi = currentLabelMask(bbox);
	cv::Mat markersRoi = markersPic(bbox);
	std::set<int> neighborSet;

	cv::Mat maskRoiBig = ImageProcessor::dilate(maskRoi, distance);

	int nl = markersRoi.rows;
	int nc = markersRoi.cols; //only valid for single channel pic!
	for (int j=0; j<nl; j++) {
		int* data = markersRoi.ptr<int>(j);
		uchar* mask = maskRoiBig.ptr<uchar>(j);
		for (int i=0; i<nc; i++) {
			if(mask[i] != 0 && data[i] != self && data[i] > 1) {
				neighborSet.insert(data[i]);
			}
		}
	}

	return neighborSet;
}

inline void ImageSegmentor::expandRect(cv::Rect& bbox, int padding, int imgHeight, int imgWidth) {
	if(bbox.x - padding >= 0) bbox.x -= padding;
	else bbox.x = 0;
	if(bbox.y - padding >= 0) bbox.y -= padding;
	else bbox.y = 0;
	if(bbox.y + bbox.height + padding + padding < imgHeight) bbox.height += padding + padding;
	else bbox.height = imgHeight - bbox.y;
	if(bbox.x + bbox.width  + padding + padding < imgWidth ) bbox.width  += padding + padding;
	else bbox.width  = imgWidth  - bbox.x;

}



