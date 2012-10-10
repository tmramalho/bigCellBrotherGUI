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
	std::vector< vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<int> contourList;
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

void ImageSegmentor::breakLargeContours(cv::Mat &contourStorage, vector< vector<cv::Point> > &contours, vector<cv::Vec4i> &hierarchy,
										int i, int maxHeight, int maxWidth) {
	cv::Rect boundBox = cv::boundingRect(contours[i]);
	cv::RotatedRect box = cv::minAreaRect(contours[i]);
	std::vector< vector<cv::Point> > ctours;
	std::vector<cv::Vec4i> hrchy;
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
	int maxNumNeighbors = 0;
	double min, max;
	cv::minMaxLoc(markersPic,&min,&max);
	cv::Mat currentLabelMask(markersPic.size(), CV_8U);
	std::set<int>::iterator setIt;
	std::map<int, CellCont> labels;
	std::map<int, CellCont>::iterator it;
	double probThreshold = 10;

	for(int i = 2; i < max + 1; i++) { //iterate all cell labels
		currentLabelMask = cv::Scalar::all(BLACK);
		cv::compare(markersPic, i, currentLabelMask, cv::CMP_EQ);

		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		CellCont newLabel = determineLabelProperties(currentLabelMask, markersPic, i);

		if((int) newLabel.getNeighbors().size() > maxNumNeighbors) maxNumNeighbors = (int) newLabel.getNeighbors().size();
		labels.insert( std::pair<int, CellCont> (i, newLabel) );
	}

	// isolated cells
	for(it = labels.begin(); it != labels.end(); it++) {
		currentLabelMask = cv::Scalar::all(BLACK);
		CellCont& currentCell = (*it).second;

		if(currentCell.getNeighbors().size() == 0) {
			if(currentCell.getProbs().at(0) < probThreshold) {
				labels.erase(it);
				//TODO:remove from markersPic
			}
		}
	}

	// cells with neighbors
	int numNeighbors = 1;

	while(numNeighbors < maxNumNeighbors) {
		for(it = labels.begin(); it != labels.end(); it++) {
			CellCont& currentCell = (*it).second;
			int curLabel = (*it).first;
			std::set<int> currentNeighbors = currentCell.getNeighbors();

			if((int) currentNeighbors.size() == numNeighbors) {
				currentLabelMask = cv::Scalar::all(BLACK);
				cv::compare(markersPic, curLabel, currentLabelMask, cv::CMP_EQ);

				for(setIt = currentNeighbors.begin();
						setIt != currentNeighbors.end(); ++setIt) {
					int neighborLabel = *setIt;
					std::cout << neighborLabel << std::endl;
					cv::Mat neighborLabelMask(currentLabelMask.size(), CV_8U);
					cv::compare(markersPic, neighborLabel, neighborLabelMask, cv::CMP_EQ);
					cv::bitwise_or(currentLabelMask, neighborLabelMask, neighborLabelMask);
					neighborLabelMask = ImageProcessor::applyMorphologyOp(neighborLabelMask, cv::MORPH_CLOSE, 3);
					CellCont newLabel = determineLabelProperties(neighborLabelMask, markersPic, neighborLabel);

					currentCell.printCellInfo();
					labels[neighborLabel].printCellInfo();
					newLabel.printCellInfo();
					/* if final score is better than both, then merge
					 * remove neighbor and current, add new to the list
					 * make sure currentLabel is removed from neighbor list
					 * finally swap labels in markersPic
					 */
					cv::imshow("merged", neighborLabelMask);
					cv::waitKey(0);
				}
			}
		}
	}

	for(it = labels.begin(); it != labels.end(); it++) {
		currentLabelMask = cv::Scalar::all(BLACK);
		CellCont& currentCell = (*it).second;
		std::cout << (*it).first << " " << currentCell.getCurLabel() << std::endl;
		cv::compare(markersPic, currentCell.getCurLabel(), currentLabelMask, cv::CMP_EQ);

		currentCell.printCellInfo();
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
	std::vector<int> markerSize(max + 2, 0); // there are pixels with value -1

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
	if(bbox.y + bbox.height + 2 * padding < imgHeight) bbox.height += 2 * padding;
	else bbox.height = imgHeight - bbox.y;
	if(bbox.x + bbox.width  + 2 * padding < imgWidth ) bbox.width  += 2 * padding;
	else bbox.width  = imgWidth  - bbox.x;

}

CellCont ImageSegmentor::determineLabelProperties(cv::Mat &currentLabelMask, cv::Mat &markersPic, int i) {
	double mi, ma;
	cv::Mat currentLabelCtour;
	std::vector< vector<cv::Point> > ctours;
	std::vector<cv::Vec4i> hrchy;
	std::vector<double> features (11);
	std::set<int> ngb;

	CellClassifier decider; //simplest classifier
	decider.setHeight(40);
	decider.setWidth(12);
	decider.setHeightSigma(100);
	decider.setWidthSigma(16);

	// get the contour of this cell label
	currentLabelMask.copyTo(currentLabelCtour);
	cv::findContours(currentLabelCtour, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	cv::RotatedRect box = cv::minAreaRect(ctours[0]);
	cv::Rect bbox = cv::boundingRect(ctours[0]);

	//save cell features
	detectHeightWidth(box, &mi, &ma);
	features[0] = mi;
	features[1] = ma;
	features[2] = cv::contourArea(ctours[0]);
	features[3] = cv::arcLength(ctours[0], true);
	// put 7 hu moments in features [4:10]
	cv::Moments mom = cv::moments(ctours[0]);
	cv::HuMoments(mom, &features[4]);

	// probability of this being a cell
	std::vector<double> probList = decider.calculateLogProbFeatures(features);

	//find cell neighbors
	expandRect(bbox, 20, markersPic.rows, markersPic.cols);
	ngb = findNearestNeigbors(bbox, markersPic, currentLabelMask, i, 5);

	CellCont newLabel;
	//save stuff in CellCont
	newLabel.setCurLabel(i);
	newLabel.setFeatures(features);
	newLabel.setNeighbors(ngb);
	newLabel.setProbs(probList);

	return newLabel;
}





