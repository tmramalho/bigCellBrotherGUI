/*
 * SVM.cpp
 *
 *  Created on: Jan 14, 2013
 *      Author: tiago
 */

#include "SVMachine.h"

SVMachine::SVMachine(int nf) {
	trained = false;
	parameters = new svm_parameter;
	model = new svm_model;
	problem = new svm_problem;
	numFeatures = nf;
	accuracy = 0;
	a.resize(numFeatures, 0);
	b.resize(numFeatures, 0);
	svm_set_print_string_function(&(SVMachine::dummy));
}

SVMachine::~SVMachine() {
	svm_free_model_content(model);
	svm_destroy_param(parameters);
}

void SVMachine::addTrainingSet(std::vector<std::vector<double> >& goodSamples,
		std::vector<std::vector<double> >& badSamples) {
	if((unsigned int) numFeatures != goodSamples[0].size()) {
		std::cerr << "These are not the samples I'm looking for." << std::endl;
		return;
	}

	if(max.size() == 0) max.resize(numFeatures, -1e10);
	if(min.size() == 0) min.resize(numFeatures, 1e10);

	addFeaturesToNodeList(goodSamples, 1, numFeatures);
	addFeaturesToNodeList(badSamples, 0, numFeatures);
}

void SVMachine::createSVM() {
	if(trainingSet.size() < 10) {
		std::cerr << "I want more samples" << std::endl;
		return;
	} else if(trained) {
		svm_free_model_content(model);
	}

	rescaleFeatures();

	problem->l = trainingSet.size();
	problem->y = trainingLabels.data();
	problem->x = trainingSet.data();

	parameters->svm_type = C_SVC;
	parameters->kernel_type = RBF;
	parameters->coef0 = 1;
	parameters->degree = 1;
	parameters->gamma = 1;
	parameters->cache_size = 30;
	parameters->eps = 0.01;
	parameters->C = 1;
	parameters->nu = 1;
	parameters->p = 1;
	parameters->nr_weight = 0;
	parameters->shrinking = 0;
	parameters->probability = 0;

	const char *msg = svm_check_parameter(problem, parameters);

	if(msg) {
		std::cerr << "You messed up: " << msg << std::endl;
		return;
	}

	findBestParameters();

	model = svm_train(problem, parameters);
	calcAcc();
	trainingLabels.clear();
	trainingSet.clear();

	trained = true;
}

bool SVMachine::classifyCell(std::vector<double>& probs) {
	if(!trained) {
		std::cerr << "You must train the model before." << std::endl;
		return false;
	}
	svm_node *node = new svm_node[numFeatures+1];
	for(int i= 0; i< numFeatures; i++) {
		double val = probs.at(i);
		node[i].index = i;
		node[i].value = val;
	}
	node[numFeatures].index = -1; //end of node
	rescaleNode(node);
	double svmClass = svm_predict(model, node);
	delete node;
	if(svmClass == 1) return true;
	else return false;
}

void SVMachine::saveModel(std::string filename) {
	if(trained) {
		//TODO:must save the scaling as well
		svm_save_model(filename.c_str(), model);
	}
}

void SVMachine::loadModel(std::string filename) {
	if(trained) {
		svm_free_model_content(model);
	}
	model = svm_load_model(filename.c_str());
	trained = true;
}

void SVMachine::addFeaturesToNodeList(std::vector<std::vector<double> >& someSamples,
		int label, int numFeatures) {
	for(std::vector<std::vector<double> >::iterator it = someSamples.begin();
		it != someSamples.end(); ++it) {
		svm_node *node = new svm_node[numFeatures+1];
		for(int i= 0; i< numFeatures; i++) {
			double val = it->at(i);
			if(val > max[i]) max[i] = val;
			if(val < min[i]) min[i] = val;
			node[i].index = i;
			node[i].value = val;
		}
		node[numFeatures].index = -1; //end of node
		trainingSet.push_back(node);
		trainingLabels.push_back(label);
	}
}

void SVMachine::rescaleFeatures() {
	for(int i= 0; i< numFeatures; i++) {
		a[i] = 2.0/(max[i] - min[i]);
		b[i] = -(max[i] + min[i])/(max[i] - min[i]);
	}

	for(std::vector<svm_node*>::iterator it = trainingSet.begin();
		it != trainingSet.end(); ++it) {
		rescaleNode(*it);
	}
}

inline void SVMachine::rescaleNode(svm_node *node) {
	for(int i= 0; i< numFeatures; i++) {
		node[i].value = node[i].value*a[i]+b[i];
	}
}

void SVMachine::findBestParameters() {
	double *target = new double[problem->l];
	std::vector<double> metaBest(2, 0);
	double best = -1e100;
	for(double j = pow(2,-5); j < pow(2,15); j*=2) {
		for(double k = pow(2,-15); k < pow(2,3); k*=2) {
			parameters->C = j;
			parameters->gamma = k;
			double score = crossValidate(target);
			if(score > best) {
				best = score;
				metaBest[0] = j;
				metaBest[1] = k;
			}
		}
	}
	std::cout << "C: " << metaBest[0] << " g: " << metaBest[1] << " -> " << 100*best/problem->l << std::endl;
	parameters->C = metaBest[0];
	parameters->gamma = metaBest[1];
	delete target;
}

double SVMachine::crossValidate(double* target) {
	svm_cross_validation(problem, parameters, 5, target);
	int acc = 0;
	for(int i = 0; i < problem->l; i++) {
		if(target[i] == problem->y[i]) ++acc;
	}
	return acc;
}

void SVMachine::calcAcc() {
	int correct = 0;
	int acc = 0;
	for(unsigned int i = 0; i < trainingSet.size(); i++) {
		double svmClass = svm_predict(model, trainingSet[i]);
		if(svmClass == trainingLabels[i]) correct++;
		acc++;
	}
	accuracy = (double) correct / (double) acc;
}
