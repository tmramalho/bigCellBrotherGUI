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

vector<cv::Vec3b> ImageSegmentor::getRandomColorTab(int numComp) {
	vector<cv::Vec3b> colorTab;
	for( int i = 0; i < numComp; i++ ) {
		int b = cv::theRNG().uniform(BLACK, WHITE);
		int g = cv::theRNG().uniform(BLACK, WHITE);
		int r = cv::theRNG().uniform(BLACK, WHITE);

		colorTab.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	return colorTab;
}



cv::Mat ImageSegmentor::drawMarkers(const markersCont &mc) {
	cv::Mat wshed(mc.markers.size(), CV_8UC3);
	vector<cv::Vec3b> colorTab = getRandomColorTab(mc.compCount);
	char buf[512];
	for( int i = 0; i < mc.markers.rows; i++ )
		for( int j = 0; j < mc.markers.cols; j++ )
		{
			int idx = mc.markers.at<int>(i,j);
			if( idx == -1 )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(WHITE, WHITE, WHITE);
			else if( idx <= 0 || idx > mc.compCount )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else
				wshed.at<cv::Vec3b>(i,j) = colorTab[idx - 1];
		}

	for(uint i = 0; i < mc.assocCompContour.size(); i++) {
		int contIndex = mc.assocCompContour[i];
		cv::Rect bbox = cv::boundingRect(mc.contours[contIndex]);
		cv::RotatedRect box = cv::minAreaRect(mc.contours[contIndex]);
		//std::cout << i << " : " << box.size.height << ", " << box.size.width << ", " << box.angle << std::endl;
		sprintf(buf, "%d", i);
		cv::putText(wshed, buf, cv::Point(bbox.x, bbox.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(colorTab[i+1]));
		drawMinAreaRect(wshed, box ,colorTab[i+1]);
	}

	return wshed;
}

cv::Mat ImageSegmentor::drawMarkersOnPicture(cv::Mat& targetPicture, cv::Mat& markers) {
	cv::Mat targetColorPicture;
	cv::Mat colorMarkers(targetPicture.rows, targetPicture.cols, CV_8UC3);
	vector<cv::Vec3b> colorTab = getRandomColorTab(500);

	cv::cvtColor(targetPicture, targetColorPicture, CV_GRAY2BGR);

	for( int i = 0; i < markers.rows; i++ )
		for( int j = 0; j < markers.cols; j++ )
		{
			int idx = markers.at<int>(i,j);
			if( idx == -1 )
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(WHITE, WHITE, WHITE);
			else if( idx == 1 )
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else
				colorMarkers.at<cv::Vec3b>(i,j) = colorTab[idx - 1];
		}

	cv::Mat result = colorMarkers * 0.5 + targetColorPicture * 0.5;
	return result;
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

markersCont ImageSegmentor::makeNiceMarkers(cv::Mat& origImage, int maxHeight, int maxWidth) {
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

void ImageSegmentor::drawMinAreaRect(cv::Mat& target, cv::RotatedRect& box,
									 cv::Vec3b& color) {
	cv::Scalar colorScalar = cv::Scalar( color );
	cv::Point2f rect_points[4];
	box.points( rect_points );
	for( int j = 0; j < 4; j++ )
		cv::line( target, rect_points[j], rect_points[(j+1)%4], colorScalar, 1, 8 );
}

vector<double> ImageSegmentor::calcRatios(const cv::Mat& cellMask,
		vector<cv::RotatedRect>& boxes, int cellPixels) {
	vector<double> results;
	cv::Mat rectMask(cellMask.rows, cellMask.cols, CV_8U, cv::Scalar::all(BLACK));

	double area = 0;
	for (uint i = 0; i<boxes.size() ; i++) {
		drawRotatedRect(rectMask, boxes[i]);
		area += cv::countNonZero(rectMask);
		rectMask = cv::Scalar::all(BLACK);
	}

	drawRotatedRect(rectMask, boxes);

	int boxAreaPixels = cv::countNonZero(rectMask);//area of the boxes
	double overlap = area - boxAreaPixels;

	cv::bitwise_and(rectMask, cellMask, rectMask);
	int coveredPixels = cv::countNonZero(rectMask);//area of cells covered by boxes

	results.push_back( coveredPixels / (double) cellPixels );
	results.push_back( coveredPixels / (double) boxAreaPixels );
	results.push_back( overlap );
	return results;
}

void ImageSegmentor::drawRotatedRect(cv::Mat& mask, vector<cv::RotatedRect>& box) {
	for(uint i = 0; i < box.size(); i++) {
		cv::Point2f rect_points[4];
		box[i].points( rect_points );
		cv::Point2i rect_ipts[4];
		for(uint j=0; j<4; j++) rect_ipts[j] = rect_points[j];
		cv::fillConvexPoly(mask, rect_ipts, 4, cv::Scalar(WHITE));
	}
}

void ImageSegmentor::drawRotatedRect(cv::Mat& mask, cv::RotatedRect& box) {
	cv::Point2f rect_points[4];
	box.points( rect_points );
	cv::Point2i rect_ipts[4];
	for(uint j=0; j<4; j++) rect_ipts[j] = rect_points[j];
	cv::fillConvexPoly(mask, rect_ipts, 4, cv::Scalar(WHITE));

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

cv::Mat ImageSegmentor::processLabels(cv::Mat &markersPic) {
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

