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

class NaiveBayes: public CellClassifier {
public:
	NaiveBayes();
	~NaiveBayes();

	void addTrainingSample(std::vector<double>& features);
	void addTrainingSet(std::vector< std::vector<double> >& trainingSet);
	std::vector<double> calculateLogProbFeatures(std::vector<double>& features);
	bool classifyCell(std::vector<double>& probs);
	double getProbThreshold() const;
	void setProbThreshold(double probThreshold);

private:
	std::vector<double> averages;
	std::vector<double> variances;
	double probThreshold;
	uint numFeatures;
};

#endif /* NAIVEBAYES_H_ */
