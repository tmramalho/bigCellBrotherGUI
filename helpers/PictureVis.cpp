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
			/*if( idx == -1 )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(WHITE, WHITE, WHITE);
			else*/ if( idx <= 0 || idx > (int)mc.contourList.size() )
				wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else
				wshed.at<cv::Vec3b>(i,j) = colorTab[idx - 1];
		}

	for(unsigned int i = 0; i < mc.contourList.size(); i++) {
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
			/*if( idx == -1 )
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(WHITE, WHITE, WHITE);
			else*/ if( idx <= 1 )
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else
				colorMarkers.at<cv::Vec3b>(i,j) = colorTab[idx - 1];
		}

	cv::Mat result = colorMarkers * 0.5 + targetColorPicture * 0.5;
	return result;
}

cv::Mat PictureVis::drawCellsOnPicture(cv::Mat& targetPicture, cv::Mat& markers,
		std::vector<std::vector<CellCont> >& allCells, int frameNum) {
	cv::Mat target(targetPicture.rows, targetPicture.cols, CV_8UC3);
	double max;
	cv::minMaxLoc(markers, NULL, &max);
	vector<cv::Vec3b> colorTab = getRandomColorTab((int)max + 1);
	cv::Mat original;
	if(targetPicture.type() == CV_8U)
		cv::cvtColor(targetPicture, original, CV_GRAY2RGB);
	else
		targetPicture.copyTo(original);

	for( int i = 0; i < markers.rows; i++ )
		for( int j = 0; j < markers.cols; j++ )
		{
			int idx = markers.at<int>(i,j);
			if( idx == 1 )
				target.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else
				target.at<cv::Vec3b>(i,j) = colorTab[idx - 1];
		}

	std::vector<CellCont> cellVector = allCells[frameNum];
	std::vector<CellCont> prevCellVector;
	if(frameNum > 0) prevCellVector = allCells[frameNum-1];
	int i = 0;
	//char buf[512];

	for(std::vector<CellCont>::iterator it = cellVector.begin();
		it != cellVector.end(); it++) {
		/*cv::Scalar colorScalar = cv::Scalar(colorTab[i - 1]);
		cv::Rect bbox = it->getBoundBox();
		sprintf(buf, "%d", it->getCurLabel());
		cv::putText(target, buf, cv::Point(bbox.x, bbox.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, colorScalar);
		double fluorescence = it->getFluorescence();
		if(fluorescence > 0) {
			sprintf(buf, "%f", fluorescence);
			cv::putText(target, buf, cv::Point(bbox.x, bbox.y + bbox.width), cv::FONT_HERSHEY_SIMPLEX, 0.5, colorScalar);
		}*/
		int parentLabel = it->getPrevLabel();
		if(parentLabel > 0 && frameNum > 0) {
			for(std::vector<CellCont>::iterator jt = prevCellVector.begin();
				jt != prevCellVector.end(); jt++) {
				if(jt->getCurLabel() == parentLabel) {
					cv::line(target, jt->getCenter(), it->getCenter(), cv::Scalar(cv::Vec3b(255,255,255)));
					break;
				}
			}
		}
		i++;
	}

	cv::Mat result = original * 0.5 + target * 0.5;
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
	for(unsigned int i = 0; i < box.size(); i++) {
		cv::Point2f rect_points[4];
		box[i].points( rect_points );
		cv::Point2i rect_ipts[4];
		for(unsigned int j=0; j<4; j++) rect_ipts[j] = rect_points[j];
		cv::fillConvexPoly(mask, rect_ipts, 4, cv::Scalar(WHITE));
	}
}

void PictureVis::drawRotatedRect(cv::Mat& mask, cv::RotatedRect& box) {
	cv::Point2f rect_points[4];
	box.points( rect_points );
	cv::Point2i rect_ipts[4];
	for(unsigned int j=0; j<4; j++) rect_ipts[j] = rect_points[j];
	cv::fillConvexPoly(mask, rect_ipts, 4, cv::Scalar(WHITE));

}

cv::Mat PictureVis::drawClassyMarkersOnPicture(cv::Mat& targetPicture,
		cv::Mat& markers, std::set<int> markedBadLabels, std::set<int> markedGoodLabels, std::set<int> foundBadLabels,
		std::set<int> foundGoodLabels) {
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
			if( idx == 1 )
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, BLACK);
			else if(markedBadLabels.count(idx) > 0) //red
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, BLACK, WHITE);
			else if(markedGoodLabels.count(idx) > 0) //green
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, WHITE, BLACK);
			else if(foundBadLabels.count(idx) > 0) //yellow
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(BLACK, WHITE, WHITE);
			else if(foundGoodLabels.count(idx) > 0) //cyan
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(WHITE, WHITE, BLACK);
			else
				colorMarkers.at<cv::Vec3b>(i,j) = cv::Vec3b(  160,   160,   160);
		}

	cv::Mat result = colorMarkers * 0.5 + targetColorPicture * 0.5;
	return result;
}

vector<cv::Vec3b> PictureVis::getRandomColorTab(int numComp) {
	vector<cv::Vec3b> colorTab;
	for( int i = 0; i < numComp; i++ ) {
		//start at 100 to make sure colors are bright
		int b = cv::theRNG().uniform(100, WHITE);
		int g = cv::theRNG().uniform(100, WHITE);
		int r = cv::theRNG().uniform(100, WHITE);

		colorTab.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	return colorTab;
}
