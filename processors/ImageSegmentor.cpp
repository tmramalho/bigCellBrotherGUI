/*
 * ImageSegmentor.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: tiago
 */

#include "ImageSegmentor.h"

ImageSegmentor::ImageSegmentor() {
	boxPadding = 20;
}

ImageSegmentor::~ImageSegmentor() {

}

void ImageSegmentor::watershed() {
	cv::Mat wsTargetImage;
	cv::cvtColor(boostedImage, wsTargetImage, CV_GRAY2BGR);

	addBackgroundMask();
	cv::watershed( wsTargetImage, markersPic );
}

void ImageSegmentor::addBackgroundMask() {
	int nl = markersPic.rows;
	int nc = markersPic.cols; //only valid for single channel pic!
	for (int j=0; j<nl; j++) {
		char* bgdata = backgroundMask.ptr<char>(j);
		int * mkdata = markersPic.ptr<int>(j);
		for (int i=0; i<nc; i++) {
			if(bgdata[i] != 0) mkdata[i] = 1;
		}
	}
}

void ImageSegmentor::createMarkersIterative(cv::Mat& origImage, int maxHeight,
		int maxWidth, int window) {
	std::vector< vector<cv::Point> > ctours;
	std::vector<cv::Vec4i> hrchy;
	double hDim, wDim;
	cv::Mat targets(origImage.size(), CV_8U, cv::Scalar::all(BLACK));
	cv::Mat topHat = ImageProcessor::applyMorphologyOp(origImage, cv::MORPH_TOPHAT, window);
	cv::Mat laplace = ImageProcessor::laplacian(origImage, 27);

	cv::Mat blobs = ImageProcessor::adaptiveThreshold(origImage, 0, window, true);
	blobs = blobs - backgroundMask;
	blobs = ImageProcessor::applyMorphologyOp(blobs, cv::MORPH_CLOSE, 3);
	blobs = ImageProcessor::applyMorphologyOp(blobs, cv::MORPH_OPEN, 3);
	cv::Mat distTrans = ImageProcessor::distanceTransform(blobs);
	cv::Mat landscape = distTrans*0.5 + laplace*0.5 - topHat - backgroundMask;

	for (int th = 20; th < 250; th += 10) { //increase threshold for distance transf.
		cv::Mat threshResult = ImageProcessor::threshold(landscape, th, false);
		cv::Mat newContour; threshResult.copyTo(newContour);
		//find connected components in the thresholded pic
		cv::findContours(newContour, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
		int cc = ctours.size();
		for (int j = 0; j < cc; j++) {
			cv::RotatedRect boxTemp = cv::minAreaRect(ctours[j]);
			CellCont::detectHeightWidth(boxTemp, &hDim, &wDim);
			//draw the connected components which obey the criterion
			if (hDim < maxHeight && wDim < maxWidth) {
				cv::drawContours(targets, ctours, j, cv::Scalar::all(WHITE), -1, 8, hrchy, INT_MAX);
			}
		}
	}

	cv::Mat markers(origImage.size(), CV_32S, cv::Scalar::all(0));
	cv::findContours(targets, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	int nc = 0;
	for( uint i = 0; i< ctours.size(); i++ ) {
		if(hrchy[i][3] != -1) continue; //it's a hole!
		nc++;
		cv::drawContours(markers, ctours, i, cv::Scalar::all(nc+1), -1, 8, hrchy, INT_MAX);
	}

	markersPic = markers;

}

void ImageSegmentor::createMarkers(cv::Mat& targetBlobs, int maxHeight, int maxWidth) {
	std::vector< vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<int> contourList;
	cv::Mat contourSourceImage;
	cv::Mat finalStorage(targetBlobs.size(), CV_8U, cv::Scalar::all(BLACK));
	cv::Mat tempStorage(targetBlobs.size(), CV_8U, cv::Scalar::all(BLACK));
	cv::Mat markers(targetBlobs.size(), CV_32S, cv::Scalar::all(0));

	targetBlobs.copyTo(contourSourceImage);

	cv::findContours(contourSourceImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	cv::Mat laplace = ImageProcessor::laplacian(originalImage, 27);

	for( uint i = 0; i< contours.size(); i++ ) {
		if(hierarchy[i][3] != -1) continue; //it's a hole!
		this->breakLargeContours(tempStorage, laplace, contours, hierarchy, i, maxHeight, maxWidth);
		cv::bitwise_or(finalStorage, tempStorage, finalStorage);
	}

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

	markersPic = markers;
}

void ImageSegmentor::breakLargeContours(cv::Mat &contourStorage, cv::Mat laplace, vector< vector<cv::Point> > &contours, vector<cv::Vec4i> &hierarchy,
										int i, int maxHeight, int maxWidth) {
	cv::Rect boundBox = cv::boundingRect(contours[i]);
	cv::RotatedRect box = cv::minAreaRect(contours[i]);
	std::vector< vector<cv::Point> > ctours;
	std::vector<cv::Vec4i> hrchy;
	double hDim, wDim;
	int hits = 0;

	CellCont::detectHeightWidth(box, &hDim, &wDim);
	contourStorage = cv::Scalar::all(BLACK);
	cv::drawContours(contourStorage, contours, i, cv::Scalar::all(WHITE), -1, 8, hierarchy, INT_MAX);

	if(hDim > maxHeight || wDim > maxWidth) { //contour too big, break it

		cv::Mat contourRoi = contourStorage(boundBox); //select only region of interest
		cv::Mat splitLabels(contourRoi.size(), CV_8U, cv::Scalar::all(BLACK));
		cv::Mat distTrans = ImageProcessor::distanceTransform(contourRoi);
		cv::Mat laplaceRoi = laplace(boundBox);
		cv::Mat maskedLaplace;
		laplaceRoi.copyTo(maskedLaplace, contourRoi);//set bg to zero on laplace
		cv::Mat landscape = 0.5*distTrans + 0.5*maskedLaplace;

		for (int th = 20; th < 250; th += 10) { //increase threshold for distance transf.
			cv::Mat threshResult = ImageProcessor::threshold(landscape, th, false);
			cv::Mat newContour; threshResult.copyTo(newContour);
			//find connected components in the thresholded pic
			cv::findContours(newContour, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
			int cc = ctours.size();
			for (int j = 0; j < cc; j++) {
				cv::RotatedRect boxTemp = cv::minAreaRect(ctours[j]);
				CellCont::detectHeightWidth(boxTemp, &hDim, &wDim);
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

void ImageSegmentor::findCellMarkers(CellClassifier *deciderPtr) {
	double min, max;
	cv::minMaxLoc(markersPic,&min,&max);
	cv::Mat currentLabelMask(markersPic.size(), CV_8U);
	std::vector<CellCont> cellVector;
	std::map<int, CellCont> cellsByLabel;
	std::stack<int> labelStack;
	decider = deciderPtr;

	for(int i = 2; i < max + 1; i++) { //iterate all cell labels
		currentLabelMask = cv::Scalar::all(BLACK);
		cv::compare(markersPic, i, currentLabelMask, cv::CMP_EQ);

		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		CellCont newCell = CellCont::determineLabelProperties(currentLabelMask, markersPic, i, decider);

		cellsByLabel.insert( std::pair<int, CellCont> (i, newCell) );
		cellVector.push_back(newCell);
	}

	std::sort(cellVector.begin(), cellVector.end(), CellCont::compareByNumNeighbors);

	for(std::vector<CellCont>::reverse_iterator it = cellVector.rbegin(); it != cellVector.rend(); it++) {
		labelStack.push(it->getCurLabel());
	}

	while(!labelStack.empty()) {
		int currentLabel = labelStack.top();
		std::cout << "Processing "<< currentLabel << std::endl;
		labelStack.pop();
		if(cellsByLabel.count(currentLabel) == 0) {
			//std::cout << "Skipping " << currentLabel << std::endl;
			continue;
		}
		CellCont& currentCell = cellsByLabel[currentLabel];

		int numNeighbors = currentCell.getNeighbors().size();
		cv::compare(markersPic, currentLabel, currentLabelMask, cv::CMP_EQ);
		std::vector<double> probs = currentCell.getProbs();

		if(numNeighbors == 0 && decider->classifyCell(probs)) {
			//std::cout << "was garbage." << std::endl;
			removeLabel(currentLabelMask);
			cellsByLabel.erase(currentLabel);
		} else if (numNeighbors > 0) {
			std::set<int> currentNeighbors = currentCell.getNeighbors();
			double bestScore = 1e100;
			double bestLabel = 0;

			for(std::set<int>::iterator setIt = currentNeighbors.begin();
				setIt != currentNeighbors.end(); ++setIt) {
				int neighborLabel = *setIt;
				if(cellsByLabel.count(neighborLabel) == 0) continue;
				double mergedScore = calcMergedScore(currentLabelMask, neighborLabel);
				if(mergedScore < currentCell.getProbs().at(0)
						&& mergedScore < cellsByLabel[neighborLabel].getProbs().at(0)
						&& mergedScore < bestScore) {
					bestScore = mergedScore;
					bestLabel = neighborLabel;
				}
			}
			if(bestScore < 1e100) {
				//std::cout << "merged with " << bestLabel << std::endl;
				cellsByLabel[bestLabel] = mergeLabels(currentLabelMask, bestLabel, currentLabel);
				labelStack.push(bestLabel); // process the merged cell
				cellsByLabel.erase(currentLabel);
			} else if (decider->classifyCell(probs)) {
				//std::cout << "had neighbors but was garbage." << std::endl;
				removeLabel(currentLabelMask);
				cellsByLabel.erase(currentLabel);
			} else {// else was good cell, do nothing
				//std::cout << "was good aleady" << std::endl;
			}
		}
	}

	/*for(std::map<int, CellCont>::iterator it = cellsByLabel.begin(); it != cellsByLabel.end(); it++) {
		currentLabelMask = cv::Scalar::all(BLACK);
		CellCont& currentCell = (*it).second;
		std::cout << currentCell.getCurLabel() << std::endl;
		cv::compare(markersPic, currentCell.getCurLabel(), currentLabelMask, cv::CMP_EQ);
		cv::Mat displayCell = currentLabelMask(currentCell.getBoundBox());

		currentCell.printCellInfo();
		cv::imshow("curr label", displayCell); //DEBUG
		cv::waitKey(0);
	}*/
}

double ImageSegmentor::calcMergedScore(cv::Mat &currentLabelMask, int neighborLabel) {
	cv::Mat neighborLabelMask(currentLabelMask.size(), CV_8U);
	cv::compare(markersPic, neighborLabel, neighborLabelMask, cv::CMP_EQ);
	cv::bitwise_or(currentLabelMask, neighborLabelMask, neighborLabelMask);
	neighborLabelMask = ImageProcessor::applyMorphologyOp(neighborLabelMask, cv::MORPH_CLOSE, mergeSmoothRadius);
	CellCont newCell = CellCont::determineLabelProperties(neighborLabelMask, markersPic, neighborLabel, decider);

	return newCell.getProbs().at(0);
}

CellCont ImageSegmentor::mergeLabels(cv::Mat &currentLabelMask, int neighborLabel, int currentLabel) {
	std::vector< vector<cv::Point> > ctours;
	std::vector<cv::Vec4i> hrchy;

	cv::Mat neighborLabelMask(currentLabelMask.size(), CV_8U);
	cv::compare(markersPic, neighborLabel, neighborLabelMask, cv::CMP_EQ);
	cv::bitwise_or(currentLabelMask, neighborLabelMask, neighborLabelMask);
	neighborLabelMask = ImageProcessor::applyMorphologyOp(neighborLabelMask, cv::MORPH_CLOSE, mergeSmoothRadius);
	CellCont newCell = CellCont::determineLabelProperties(neighborLabelMask, markersPic, neighborLabel, decider);

	cv::findContours(neighborLabelMask, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	cv::drawContours(markersPic, ctours, 0, cv::Scalar::all(neighborLabel), -1, 8, hrchy, INT_MAX);

	newCell.removeNeighbor(currentLabel);
	newCell.removeNeighbor(neighborLabel);

	return newCell;
}

void ImageSegmentor::removeLabel(cv::Mat &currentLabelMask) {
	std::vector< vector<cv::Point> > ctours;
	std::vector<cv::Vec4i> hrchy;
	cv::findContours(currentLabelMask, ctours, hrchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	cv::drawContours(markersPic, ctours, 0, cv::Scalar::all(0), -1, 8, hrchy, INT_MAX);
}

void ImageSegmentor::removeSmallMarkers(int th) {
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

cv::Mat ImageSegmentor::getBoostedImage() const
{
    return boostedImage;
}

cv::Mat ImageSegmentor::getMarkersPic() const
{
    return markersPic;
}

cv::Mat ImageSegmentor::getOriginalImage() const
{
    return originalImage;
}

markersCont ImageSegmentor::getWatershedMarkers() const
{
    return watershedMarkers;
}

void ImageSegmentor::setBoostedImage(cv::Mat &boostedImage)
{
    this->boostedImage = boostedImage;
}

void ImageSegmentor::setOriginalImage(cv::Mat &originalImage)
{
    this->originalImage = originalImage;
}

void ImageSegmentor::setWatershedMarkers(markersCont watershedMarkers)
{
    this->watershedMarkers = watershedMarkers;
}

cv::Mat ImageSegmentor::getBackgroundMask() const
{
    return backgroundMask;
}

void ImageSegmentor::setBackgroundMask(cv::Mat &backgroundMask)
{
    this->backgroundMask = backgroundMask;
}

void ImageSegmentor::smoothLabels(int kernelSize) {
	double min, max;
	cv::minMaxLoc(markersPic,&min,&max);
	cv::Mat newMarkers(markersPic.size(), CV_32S, cv::Scalar::all(0));
	cv::Mat currentLabelMask(markersPic.size(), CV_8U);

	for(int i = 2; i < max + 1; i++) { //iterate all cell labels
		currentLabelMask = cv::Scalar::all(BLACK);
		cv::compare(markersPic, i, currentLabelMask, cv::CMP_EQ);

		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		currentLabelMask = ImageProcessor::applyMorphologyOp(currentLabelMask, cv::MORPH_OPEN, kernelSize);
		currentLabelMask = ImageProcessor::applyMorphologyOp(currentLabelMask, cv::MORPH_CLOSE, kernelSize);
		newMarkers.setTo(i, currentLabelMask);
	}

	newMarkers.copyTo(markersPic);
}

void ImageSegmentor::setMarkersPic(cv::Mat& markersPic) {
	this->markersPic = markersPic;
}

void ImageSegmentor::clearBorderValues() {
	int nl = markersPic.rows;
	int nc = markersPic.cols; //only valid for single channel pic!
	for (int j=0; j<nl; j++) {
		int* data = markersPic.ptr<int>(j);
		for (int i=0; i<nc; i++) {
			if(data[i] < 0) data[i] = 0;
		}
	}
}

