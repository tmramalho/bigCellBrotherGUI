/*
 * NaiveBayes.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: tiago
 */

#include "NaiveBayes.h"

NaiveBayes::NaiveBayes() {
	initialized = false;
}

NaiveBayes::~NaiveBayes() {
}

void NaiveBayes::addTrainingSample(std::vector<double> features) {
	numFeatures = features.size();
	if(initialized == false) {
		averages = features;
		invVar.resize(numFeatures, 0);
		for(uint j = 0; j < numFeatures; j++) {
			invVar[j] = 1/pow(0.5*averages[j],2); //50% initial error
		}
		initialized = true;
		numUsedSamples = 1;
	} else {
		double pr = calculateProbFeatures(features);
		double eta = 1/(double)numUsedSamples;
		for(uint j = 0; j < numFeatures; j++) {
			averages[j] += eta*pr*(features[j] - averages[j]);
			invVar[j] += eta*pr*(invVar[j] - pow((features[j] - averages[j])*invVar[j], 2));
		}
		numUsedSamples ++;
	}

	std::cout << "UNB ";
	for(uint j = 0; j < numFeatures; j++) {
		std::cout << averages[j] << " (" << invVar[j] << ", " << sqrt(1/invVar[j]) << "), ";
	}
	std::cout << std::endl;
}

void NaiveBayes::addTrainingSet(
		std::vector<std::vector<double> >& trainingSet) {
	uint numSamples = trainingSet.size();
	numFeatures = trainingSet[0].size();
	averages.resize(numFeatures, 0);
	invVar.resize(numFeatures, 0);

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
			invVar[j] += pow(currentSample[j] - averages[j], 2);
		} // this should be vectorized
	}

	for(uint j = 0; j < numFeatures; j++) {
		invVar[j] = numSamples / invVar[j];
	} // this should be vectorized

	std::cout << "Initialized naive bayes with " << std::endl;
	for(uint j = 0; j < numFeatures; j++) {
		std::cout << averages[j] << " (" << sqrt(invVar[j]) << "), ";
	}
	std::cout << std::endl;

	initialized = true;
}

double NaiveBayes::calculateProbFeatures(
		std::vector<double>& features) {
	double pr = 0;

	if(features.size() == numFeatures) {
		for(uint i = 0; i < numFeatures; i++) {
			pr *= exp( - pow(features[i] - averages[i], 2) * (0.5*invVar[i])) / sqrt(2*PI/invVar[i]);
		}
	}

	return pr;
}

double NaiveBayes::calculateLogProbFeatures(
		std::vector<double>& features) {
	double pr = 0;

	if(features.size() == numFeatures) {
		for(uint i = 0; i < numFeatures; i++) {
			pr += pow(features[i] - averages[i], 2) * (0.5*invVar[i]);
		}
	}

	return pr;
}

bool NaiveBayes::classifyCell(std::vector<double>& probs) {
	//only one cell type!
	return probs.at(0) < this->probThreshold;
}

double NaiveBayes::getProbThreshold() const {
	return probThreshold;
}

void NaiveBayes::setProbThreshold(double probThreshold) {
	this->probThreshold = probThreshold;
}


