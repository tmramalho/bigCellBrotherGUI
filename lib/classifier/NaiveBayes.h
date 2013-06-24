/*
 * NaiveBayes.h
 *
 *  Created on: Oct 18, 2012
 *      Author: tiago
 */

#ifndef NAIVEBAYES_H_
#define NAIVEBAYES_H_
#include <cstdlib>
#include <cmath>

#include "CellClassifier.h"

#define PI 3.14159265

class NaiveBayes: public CellClassifier {
public:
	NaiveBayes();
	~NaiveBayes();

	void addTrainingSample(std::vector<double> features);
	void addTrainingSet(std::vector< std::vector<double> >& trainingSet);
	double calculateProbFeatures(std::vector<double>& features);
	double calculateLogProbFeatures(std::vector<double>& features);
	bool classifyCell(std::vector<double>& probs);
	double getProbThreshold() const;
	void setProbThreshold(double probThreshold);

private:
	void initializeSamples(std::vector< std::vector<double> >& trainingSet);
	std::vector<double> averages;
	std::vector<double> invVar;
	double probThreshold;
	unsigned int numFeatures;
	double initialized;
	unsigned int numUsedSamples;
};

#endif /* NAIVEBAYES_H_ */
