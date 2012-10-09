/*
 * CellClassifier.h
 *
 *  Created on: Oct 9, 2012
 *      Author: tiago
 */

#ifndef CELLCLASSIFIER_H_
#define CELLCLASSIFIER_H_
#include <vector>

class CellClassifier {
public:
	CellClassifier();
	virtual ~CellClassifier();
	double calculateLogProbFeatures(std::vector<double>& features, int phenotype);

	double getHeight() const;
	void setHeight(double height);
	double getHeightSigma() const;
	void setHeightSigma(double heightSigma);
	double getWidth() const;
	void setWidth(double width);
	double getWidthSigma() const;
	void setWidthSigma(double widthSigma);

private:
	double width;
	double widthSigma;
	double height;
	double heightSigma;
};

#endif /* CELLCLASSIFIER_H_ */
