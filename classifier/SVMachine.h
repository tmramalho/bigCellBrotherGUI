/*
 * SVM.h
 *
 *  Created on: Jan 14, 2013
 *      Author: tiago
 */

#ifndef SVM_H_
#define SVM_H_

#include <string>
#include <iostream>
#include "libsvm/svm.h"

#include "CellClassifier.h"

class SVMachine: public CellClassifier {
public:
	SVMachine(int nf);
	~SVMachine();

	void addTrainingSet(std::vector<std::vector<double> >& goodSamples,
			std::vector<std::vector<double> >& badSamples);
	void createSVM();
	bool classifyCell(std::vector<double>& probs);
	void saveModel(std::string filename);
	void loadModel(std::string filename);

private:
	void addFeaturesToNodeList(std::vector<std::vector<double> >& someSamples, int label, int numFeatures);
	void rescaleFeatures();
	inline void rescaleNode(svm_node *node);
	void findBestParameters();
	svm_model *model;
	svm_problem *problem;
	svm_parameter *parameters;
	bool trained;
	std::vector<svm_node*> trainingSet;
	std::vector<double> trainingLabels;
	std::vector<double> max;
	std::vector<double> min;
	std::vector<double> a;
	std::vector<double> b;
	int numFeatures;
};

#endif /* SVM_H_ */
