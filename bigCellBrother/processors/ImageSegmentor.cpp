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

	// paint the watershed image
	cv::Mat wshed(wsTargetImage.rows, wsTargetImage.cols, CV_8UC3);
	vector<cv::Vec3b> colorTab = getRandomColorTab(500);
	for( int i = 0; i < markers.rows; i++ )
		for( int j = 0; j < markers.cols; j++ )
		{
			int idx = markers.at<int>(i,j);
			if( idx == -1 )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(WHITE, WHITE, WHITE);
			else if( idx <= 1 )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else
				wshed.at<cv::Vec3b>(i,j) = colorTab[idx - 1];
		}

	cv::Mat result = wshed*0.5 + wsTargetImage*0.5;
	cv::imshow("wshed2", result);

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

cv::Mat ImageSegmentor::addBackgroundMask(cv::Mat& markersPic, cv::Mat& backgroundMask) {
	cv::Mat merged;
	markersPic.copyTo(merged);

	for( int i = 0; i < markersPic.rows; i++ )
		for( int j = 0; j < markersPic.cols; j++ )
			if(backgroundMask.at<char>(i,j) != 0) merged.at<int>(i,j) = 1;

	return merged;
}

markersCont ImageSegmentor::makeNiceMarkers(cv::Mat& origImage, int defectSize, int maxHeight, int maxWidth, int minArea, int minPerimeter) {
	int compCount = 0, ni = 0, np = 100, nc = 0;
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	vector<vector<cv::Point> > contoursTemp;
	vector<cv::Vec4i> hierarchyTemp;
	vector<int> assocCompContour;
	cv::Mat wsTargetImage;
	cv::Mat contourSourceImage;
	cv::Mat fixedImage(origImage.size(), CV_8U, cv::Scalar::all(BLACK));
	cv::Mat tempStorage(origImage.size(), CV_8U, cv::Scalar::all(BLACK));
	cv::Mat markers(origImage.size(), CV_32S, cv::Scalar::all(0));
	cv::Mat debugImage(origImage.size(), CV_8U, cv::Scalar::all(BLACK));//debug

	origImage.copyTo(contourSourceImage);

	while((np - nc) != 0){
		np = nc; nc = 0;
		fixedImage = cv::Scalar::all(BLACK);

		/*find the contours of the image with the coarse objects*/
		cv::findContours(contourSourceImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

		if( contours.empty() ) {
			std::cerr << "No contours in watershed" << std::endl;
			throw new std::exception;
		}

		for( uint i = 0; i< contours.size(); i++ ) {
			if(hierarchy[i][3] != -1) continue; //it's a hole!

			cv::Rect boundBox = cv::boundingRect(contours[i]);
			cv::RotatedRect box = cv::minAreaRect(contours[i]);
			double hDim, wDim;

			if(box.size.width > box.size.height) {
				hDim = box.size.width;
				wDim = box.size.height;
			} else {
				hDim = box.size.height;
				wDim = box.size.width;
			}

			if(hDim > maxHeight || wDim > maxWidth) { //contour too big, break it
				cv::drawContours(tempStorage, contours, i, cv::Scalar::all(WHITE), -1, 8, hierarchy, INT_MAX);
				cv::Mat contourRoi = tempStorage(boundBox);
				if(ni > 0) contourRoi = ImageProcessor::dilate(contourRoi, 3);
				cv::Mat distTrans = ImageProcessor::distanceTransform(contourRoi);
				cv::Mat labeledContours(contourRoi.size(), CV_8U);
				cv::Mat newContour;
				bool broken = false;

				for (int th = 20; th < 250; th += 5) { //try to break contour
					newContour = ImageProcessor::threshold(distTrans, th, false);
					cv::Mat newContourSource;
					newContour.copyTo(newContourSource);
					cv::findContours(newContourSource, contoursTemp, hierarchyTemp, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
					int cc = contoursTemp.size();
					if(cc > 2) { broken = true; break; }
				}

				if(broken){
					nc++;
					newContour.copyTo(contourRoi);
					cv::bitwise_or(tempStorage, fixedImage, fixedImage);
				} else {
					cv::drawContours(fixedImage, contours, i, cv::Scalar::all(WHITE), -1, 8, hierarchy, INT_MAX);
				}

				tempStorage = cv::Scalar::all(BLACK);

			} else {
				cv::drawContours(fixedImage, contours, i, cv::Scalar::all(WHITE), -1, 8, hierarchy, INT_MAX);
			}

		}

		fixedImage.copyTo(contourSourceImage);
		tempStorage = cv::Scalar::all(BLACK);
		ni++;
	}

	fixedImage = cv::Scalar::all(BLACK);

	/*find the contours of the image with the coarse objects*/
	cv::findContours(contourSourceImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	for( uint i = 0; i< contours.size(); i++ ) {
		if(hierarchy[i][3] != -1) continue; //it's a hole!

		double perimeter = cv::arcLength(contours[i], 1);
		double area = cv::contourArea(contours[i]);
		if(perimeter < defectSize || area < defectSize) {
			cv::drawContours(tempStorage, contours, i, cv::Scalar::all(255), -1, 8, hierarchy, INT_MAX);
			nc++;
		} else {
			cv::drawContours(fixedImage, contours, i, cv::Scalar::all(255), -1, 8, hierarchy, INT_MAX);
		}
	}

	tempStorage = ImageProcessor::dilate(tempStorage, 5);
	tempStorage = ImageProcessor::applyMorphologyOp(tempStorage, cv::MORPH_CLOSE, 5);

	cv::bitwise_or(fixedImage, tempStorage, fixedImage);

	/*find the contours of the image with the coarse objects*/
	cv::findContours(fixedImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	for( uint i = 0; i< contours.size(); i++ ) {
		if(hierarchy[i][3] != -1) continue; //it's a hole!

		double perimeter = cv::arcLength(contours[i], 1);
		double area = cv::contourArea(contours[i]);
		if(perimeter > minPerimeter && area > minArea) {
			assocCompContour.push_back(i);
			compCount++;
			cv::drawContours(markers, contours, i, cv::Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
		}
	}

	markersCont mc;
	mc.markers = markers;
	mc.compCount = compCount;
	mc.contours = contours;
	mc.hierarchy = hierarchy;
	mc.assocCompContour = assocCompContour;

	return mc;
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

	//cv::imshow("The mask", rectMask);
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
	//cv::imshow("The masked cells", rectMask);
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


