/*
 * CellClassifier.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: tiago
 */

#include "CellClassifier.h"

CellClassifier::CellClassifier() {


}

CellClassifier::~CellClassifier() {

}

/* Stub method until a proper classifier is implemented
 * which will inherit from this class
 */
double CellClassifier::calculateLogProbFeatures(std::vector<double>& features) {
	double pr = 0;
	if(features.size() >= 2) {
		double w = (features[1] - width )*(features[1] - width )/(2*widthSigma );
		double h = (features[0] - height)*(features[0] - height)/(2*heightSigma);
		pr = w + h;
	}

	return pr;
}

bool CellClassifier::classifyCell(std::vector<double>& probs) {
	return probs.at(0) > this->probThreshold;
}

double CellClassifier::getHeight() const {
	return height;
}

void CellClassifier::setHeight(double height) {
	if(height > 0) this->height = height;
}

double CellClassifier::getHeightSigma() const {
	return heightSigma;
}

void CellClassifier::setHeightSigma(double heightSigma) {
	if(heightSigma > 0) this->heightSigma = heightSigma;
}

double CellClassifier::getWidth() const {
	return width;
}

void CellClassifier::setWidth(double width) {
	if(width > 0) this->width = width;
}

double CellClassifier::getWidthSigma() const {
	return widthSigma;
}

void CellClassifier::setWidthSigma(double widthSigma) {
	if(widthSigma > 0) this->widthSigma = widthSigma;
}

double CellClassifier::getProbThreshold() const
{
    return probThreshold;
}

void CellClassifier::setProbThreshold(double probThreshold)
{
    this->probThreshold = probThreshold;
}





