/*
 * CellCont.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: tiago
 */

#include "CellCont.h"

CellCont::CellCont() {
	this->prevLabel = -1;
	this->fluorescence = -1;
}

CellCont::~CellCont() {

}

std::vector<double> CellCont::getFeatures() const {
    return features;
}

std::set<int> CellCont::getNeighbors() const {
    return neighbors;
}

void CellCont::setFeatures(std::vector<double> features) {
    this->features = features;
}

void CellCont::setNeighbors(std::set<int> neighbors) {
    this->neighbors = neighbors;
}

bool CellCont::compareByNumNeighbors(CellCont a, CellCont b) {
	return a.getNeighbors().size() < b.getNeighbors().size();
}

double CellCont::getCompactness() {
	double p = this->features[3];
	double a = this->features[2];
	return p*p/(4*PI*a);
}

int CellCont::getCurLabel() const
{
    return curLabel;
}

void CellCont::setCurLabel(int curLabel)
{
    this->curLabel = curLabel;
}

void CellCont::printCellInfo() {
	std::vector<double>::iterator vecIt;
	std::set<int>::iterator setIt;
	std::cout << "Pretty info for cell with label " << this->curLabel << std::endl;
	std::cout << "Features: ";
	for(vecIt = this->features.begin(); vecIt != this->features.end(); ++vecIt)
		std::cout << *vecIt << ", ";
	std::cout << std::endl << "Compactness: " << this->getCompactness();
	std::cout << std::endl << "Neighbors: ";
	for(setIt = this->neighbors.begin(); setIt != this->neighbors.end(); ++setIt)
		std::cout << *setIt << ", ";
	std::cout << std::endl << std::endl;
}

void CellCont::removeNeighbor(int neighbor) {
	this->neighbors.erase(neighbor);
}

cv::Rect CellCont::getBoundBox() const {
	return boundBox;
}

void CellCont::setBoundBox(cv::Rect boundBox) {
	this->boundBox = boundBox;
}

cv::Point2f CellCont::getCenter() const {
	return center;
}

void CellCont::setCenter(double x, double y) {
	this->center.x = x;
	this->center.y = y;
}

int CellCont::getPrevLabel() const {
	return prevLabel;
}

void CellCont::setPrevLabel(int prevLabel) {
	this->prevLabel = prevLabel;
}

double CellCont::getTime() const {
	return time;
}

void CellCont::setTime(double time) {
	this->time = time;
}

CellCont CellCont::determineLabelProperties(cv::Mat &currentLabelMask, cv::Mat &markersPic,
											int label, int smRadius) {
	int boxPadding = 20;
	double mi, ma;
	cv::Mat currentLabelCtour;
	std::vector< std::vector<cv::Point> > ctours;
	std::vector<cv::Vec4i> hrchy;
	std::vector<double> features (11);
	std::set<int> ngb;

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

	for(uint n = 4; n < 11; n++)
		features[n] = features[n]*1e6;

	bool isCell = true;

	//find cell neighbors
	expandRect(bbox, boxPadding, markersPic.rows, markersPic.cols);
	ngb = findNearestNeigbors(bbox, currentLabelMask, markersPic, label, smRadius);

	CellCont newCell;
	//save stuff in CellCont
	newCell.setCenter(mom.m10/mom.m00, mom.m01/mom.m00);
	newCell.setCurLabel(label);
	newCell.setFeatures(features);
	newCell.setNeighbors(ngb);
	newCell.setBoundBox(bbox);
	newCell.setIsCell(isCell);

	return newCell;
}

inline void CellCont::expandRect(cv::Rect& bbox, int padding, int imgHeight, int imgWidth) {
	if(bbox.x - padding >= 0) bbox.x -= padding;
	else bbox.x = 0;
	if(bbox.y - padding >= 0) bbox.y -= padding;
	else bbox.y = 0;
	if(bbox.y + bbox.height + 2 * padding < imgHeight) bbox.height += 2 * padding;
	else bbox.height = imgHeight - bbox.y;
	if(bbox.x + bbox.width  + 2 * padding < imgWidth ) bbox.width  += 2 * padding;
	else bbox.width  = imgWidth  - bbox.x;

}

double CellCont::calcDistance(CellCont& first, CellCont& second) {
	cv::Point2f centerFirst = first.getCenter();
	cv::Point2f centerSecond = second.getCenter();
	double dx = centerSecond.x - centerFirst.x;
	double dy = centerSecond.y - centerFirst.y;

	return dx*dx + dy*dy;
}

void CellCont::calcFluorescence(CellCont& target, cv::Mat &currentLabelMask,
								cv::Mat &fluorescencePic) {
	//look only at roi
	cv::Rect roi = target.getBoundBox();
	cv::Mat trimFlPic = fluorescencePic(roi);
	cv::Mat trimLabelMask = currentLabelMask(roi);
	//mask
	cv::Mat maskFlPic;
	trimFlPic.copyTo(maskFlPic, trimLabelMask);
	cv::Mat result;
	cv::integral(maskFlPic,result);
	double integral = result.at<double>(result.rows-1, result.cols-1);
	target.setFluorescence(integral);


}

std::set<int> CellCont::findNearestNeigbors(cv::Rect &bbox,
		cv::Mat &currentLabelMask, cv::Mat &markersPic, int self, int distance) {
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

void CellCont::detectHeightWidth(cv::RotatedRect &box, double *hDim, double *wDim) {
	if(box.size.width > box.size.height) {
		*hDim = box.size.width;
		*wDim = box.size.height;
	} else {
		*hDim = box.size.height;
		*wDim = box.size.width;
	}
}

bool CellCont::getIsCell() const {
	return isCell;
}

void CellCont::setIsCell(bool isCell) {
	this->isCell = isCell;
}

double CellCont::getFluorescence() const {
	return fluorescence;
}

void CellCont::setFluorescence(double fluorescence) {
	this->fluorescence = fluorescence;
}







