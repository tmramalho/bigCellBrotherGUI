/*
 * NaiveBayes.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: tiago
 */

#include "NaiveBayes.h"

NaiveBayes::NaiveBayes() {
	probThreshold = 1000;
	initialized = false;
	numUsedSamples = 0;
	numFeatures = 0;
}

NaiveBayes::~NaiveBayes() {
}

void NaiveBayes::addTrainingSample(std::vector<double> features) {
	numFeatures = features.size();
	if(initialized == false) {
		averages = features;
		invVar.resize(numFeatures, 0);
		for(unsigned int j = 0; j < numFeatures; j++) {
			invVar[j] = 1/pow(averages[j],2); //100% initial error
		}
		initialized = true;
		numUsedSamples = 1;
	} else {
		double eta = (numUsedSamples < 100) ? 0.01 : 1/(double)numUsedSamples;
		for(unsigned int j = 0; j < numFeatures; j++) {
			averages[j] += eta*(features[j] - averages[j]);
			invVar[j] += eta*(invVar[j] - pow((features[j] - averages[j])*invVar[j], 2));
		}

		numUsedSamples ++;
	}
}

void NaiveBayes::addTrainingSet(
		std::vector<std::vector<double> >& trainingSet) {
	if(!initialized) {
		initializeSamples(trainingSet);
	} else {
		for(std::vector<std::vector<double> >::iterator it = trainingSet.begin();
				it != trainingSet.end(); it++) {
			addTrainingSample(*it);
		}
	}

	std::cout << "UNB ";
	for(unsigned int j = 0; j < numFeatures; j++) {
		std::cout << averages[j] << " (" << invVar[j] << ", " << sqrt(1/invVar[j]) << "), ";
	}
	std::cout << std::endl;
}

void NaiveBayes::initializeSamples(
		std::vector<std::vector<double> >& trainingSet) {
	unsigned int numSamples = trainingSet.size();
	numFeatures = trainingSet[0].size();
	averages.resize(numFeatures, 0);
	invVar.resize(numFeatures, 0);

	for(unsigned int i = 0; i < numSamples; i++) {
		std::vector<double> currentSample = trainingSet[i];
		for(unsigned int j = 0; j < numFeatures; j++) {
			averages[j] += currentSample[j];
		} // this should be vectorized
	}

	for(unsigned int j = 0; j < numFeatures; j++) {
		averages[j] /= numSamples;
	} // this should be vectorized

	for(unsigned int i = 0; i < trainingSet.size(); i++) {
		std::vector<double> currentSample = trainingSet[i];
		for(unsigned int j = 0; j < numFeatures; j++) {
			invVar[j] += pow(currentSample[j] - averages[j], 2);
		} // this should be vectorized
	}

	for(unsigned int j = 0; j < numFeatures; j++) {
		invVar[j] = numSamples / invVar[j];
	} // this should be vectorized

	numUsedSamples = numSamples;
	initialized = true;
}

double NaiveBayes::calculateProbFeatures(
		std::vector<double>& features) {
	double pr = 0;

	if(features.size() == numFeatures) {
		for(unsigned int i = 0; i < numFeatures; i++) {
			pr *= exp( - pow(features[i] - averages[i], 2) * (0.5*invVar[i])) / sqrt(2*PI/invVar[i]);
		}
	}

	return pr;
}

double NaiveBayes::calculateLogProbFeatures(
		std::vector<double>& features) {
	double pr = 0;

	if(features.size() == numFeatures) {
		for(unsigned int i = 0; i < numFeatures; i++) {
			pr += pow(features[i] - averages[i], 2) * (0.5*invVar[i]);
		}
	}

	return pr;
}

bool NaiveBayes::classifyCell(std::vector<double>& features) {
	if(!initialized) return true;
	double pr = calculateProbFeatures(features);
	//prob being cell > prob NOT being cell
	if(pr > 1-pr) return true;
	else return false;
}

double NaiveBayes::getProbThreshold() const {
	return probThreshold;
}

void NaiveBayes::setProbThreshold(double probThreshold) {
	this->probThreshold = probThreshold;
}


