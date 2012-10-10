/*
 * PictureVis.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: tiago
 */

#include "PictureVis.h"

PictureVis::PictureVis() {


}

PictureVis::~PictureVis() {

}

cv::Mat PictureVis::drawMarkers(const markersCont &mc) {
	cv::Mat wshed(mc.markers.size(), CV_8UC3);
	vector<cv::Vec3b> colorTab = getRandomColorTab(mc.contourList.size());
	char buf[512];
	for( int i = 0; i < mc.markers.rows; i++ )
		for( int j = 0; j < mc.markers.cols; j++ )
		{
			int idx = mc.markers.at<int>(i,j);
			if( idx == -1 )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(WHITE, WHITE, WHITE);
			else if( idx <= 0 || idx > (int)mc.contourList.size() )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else
				wshed.at<cv::Vec3b>(i,j) = colorTab[idx - 1];
		}

	for(uint i = 0; i < mc.contourList.size(); i++) {
		int contIndex = mc.contourList[i];
		cv::Rect bbox = cv::boundingRect(mc.contours[contIndex]);
		cv::RotatedRect box = cv::minAreaRect(mc.contours[contIndex]);
		//std::cout << i << " : " << box.size.height << ", " << box.size.width << ", " << box.angle << std::endl;
		sprintf(buf, "%d", i);
		cv::putText(wshed, buf, cv::Point(bbox.x, bbox.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(colorTab[i+1]));
		drawMinAreaRect(wshed, box ,colorTab[i+1]);
	}

	return wshed;
}

cv::Mat PictureVis::drawMarkersOnPicture(cv::Mat& targetPicture, cv::Mat& markers) {
	cv::Mat targetColorPicture;
	cv::Mat colorMarkers(targetPicture.rows, targetPicture.cols, CV_8UC3);
	double max;
	cv::minMaxLoc(markers, NULL, &max);
	vector<cv::Vec3b> colorTab = getRandomColorTab((int)max + 1);

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

void PictureVis::drawMinAreaRect(cv::Mat& target, cv::RotatedRect& box,
									 cv::Vec3b& color) {
	cv::Scalar colorScalar = cv::Scalar( color );
	cv::Point2f rect_points[4];
	box.points( rect_points );
	for( int j = 0; j < 4; j++ )
		cv::line( target, rect_points[j], rect_points[(j+1)%4], colorScalar, 1, 8 );
}

void PictureVis::drawRotatedRect(cv::Mat& mask, vector<cv::RotatedRect>& box) {
	for(uint i = 0; i < box.size(); i++) {
		cv::Point2f rect_points[4];
		box[i].points( rect_points );
		cv::Point2i rect_ipts[4];
		for(uint j=0; j<4; j++) rect_ipts[j] = rect_points[j];
		cv::fillConvexPoly(mask, rect_ipts, 4, cv::Scalar(WHITE));
	}
}

void PictureVis::drawRotatedRect(cv::Mat& mask, cv::RotatedRect& box) {
	cv::Point2f rect_points[4];
	box.points( rect_points );
	cv::Point2i rect_ipts[4];
	for(uint j=0; j<4; j++) rect_ipts[j] = rect_points[j];
	cv::fillConvexPoly(mask, rect_ipts, 4, cv::Scalar(WHITE));

}

vector<cv::Vec3b> PictureVis::getRandomColorTab(int numComp) {
	vector<cv::Vec3b> colorTab;
	for( int i = 0; i < numComp; i++ ) {
		int b = cv::theRNG().uniform(BLACK, WHITE);
		int g = cv::theRNG().uniform(BLACK, WHITE);
		int r = cv::theRNG().uniform(BLACK, WHITE);

		colorTab.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	return colorTab;
}
