/*
 * CellCont.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: tiago
 */

#include "CellCont.h"

CellCont::CellCont() {


}

CellCont::~CellCont() {

}

std::vector<double> CellCont::getFeatures() const {
    return features;
}

std::set<int> CellCont::getNeighbors() const {
    return neighbors;
}

std::vector<double> CellCont::getProbs() const {
    return probs;
}

void CellCont::setFeatures(std::vector<double> features) {
    this->features = features;
}

void CellCont::setNeighbors(std::set<int> neighbors) {
    this->neighbors = neighbors;
}

void CellCont::setProbs(std::vector<double> probs) {
    this->probs = probs;
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
		std::cout << *vecIt << " ";
	std::cout << std::endl << "Compactness: " << this->getCompactness();
	std::cout << std::endl << "Neighbors: ";
	for(setIt = this->neighbors.begin(); setIt != this->neighbors.end(); ++setIt)
		std::cout << *setIt << " ";
	std::cout << std::endl << "Probabilites: ";
	for(vecIt = this->probs.begin(); vecIt != this->probs.end(); ++vecIt)
		std::cout << *vecIt << " ";
	std::cout << std::endl << std::endl;
}

void CellCont::removeNeighbor(int neighbor) {
	this->neighbors.erase(neighbor);
}










