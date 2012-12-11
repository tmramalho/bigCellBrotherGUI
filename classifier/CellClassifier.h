/*
 * CellClassifier.h
 *
 *  Created on: Oct 9, 2012
 *      Author: tiago
 */

#ifndef CELLCLASSIFIER_H_
#define CELLCLASSIFIER_H_
#include <vector>
#include <cmath>
#include <iostream>

class CellClassifier {
public:
	CellClassifier();
	virtual ~CellClassifier();
	virtual double calculateLogProbFeatures(std::vector<double>& features);
	virtual bool classifyCell(std::vector<double>& probs);

	double getHeight() const;
	void setHeight(double height);
	double getHeightSigma() const;
	void setHeightSigma(double heightSigma);
	double getWidth() const;
	void setWidth(double width);
	double getWidthSigma() const;
	void setWidthSigma(double widthSigma);
    virtual double getProbThreshold() const;
    virtual void setProbThreshold(double probThreshold);

private:
	double width;
	double widthSigma;
	double height;
	double heightSigma;
	double probThreshold;
};

#endif /* CELLCLASSIFIER_H_ */
