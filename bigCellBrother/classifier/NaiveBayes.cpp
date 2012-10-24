/*
 * NaiveBayes.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: tiago
 */

#include "NaiveBayes.h"

NaiveBayes::NaiveBayes() {
}

NaiveBayes::~NaiveBayes() {
}

void NaiveBayes::addTrainingSample(std::vector<double>& features) {
	//TODO: implement sequential training
}

void NaiveBayes::addTrainingSet(
		std::vector<std::vector<double> >& trainingSet) {
	uint numSamples = trainingSet.size();
	numFeatures = trainingSet[0].size();
	averages.resize(numFeatures, 0);
	variances.resize(numFeatures, 0);

	for(uint i = 0; i < numSamples; i++) {
		std::vector<double> currentSample = trainingSet[i];
		for(uint j = 0; j < numFeatures; j++) {
			averages[j] += currentSample[j];
		} // this should be vectorized
	}

	for(uint j = 0; j < numFeatures; j++) {
		averages[j] /= numSamples;
	} // this should be vectorized

	for(uint i = 0; i < trainingSet.size(); i++) {
		std::vector<double> currentSample = trainingSet[i];
		for(uint j = 0; j < numFeatures; j++) {
			variances[j] += pow(currentSample[j] - averages[j], 2);
		} // this should be vectorized
	}

	for(uint j = 0; j < numFeatures; j++) {
		variances[j] /= numSamples;
	} // this should be vectorized

	std::cout << "Initialized naive bayes with " << std::endl;
	for(uint j = 0; j < numFeatures; j++) {
		std::cout << averages[j] << " (" << sqrt(variances[j]) << "), ";
	}
	std::cout << std::endl;
}

std::vector<double> NaiveBayes::calculateLogProbFeatures(
		std::vector<double>& features) {
	//only one cell type is supported for now
	std::vector<double> probList(1, 0);

	if(features.size() == numFeatures) {
		double pr = 0;
		for(uint i = 0; i < numFeatures; i++) {
			pr += pow(features[i] - averages[i], 2) / (2*variances[i]);
		}
		probList[0] = pr;
	}

	return probList;
}

bool NaiveBayes::classifyCell(std::vector<double>& probs) {
	//only one cell type!
	return probs.at(0) > this->probThreshold;
}

double NaiveBayes::getProbThreshold() const {
	return probThreshold;
}

void NaiveBayes::setProbThreshold(double probThreshold) {
	this->probThreshold = probThreshold;
}


