/*
 * CellCont.h
 *
 *  Created on: Oct 9, 2012
 *	  Author: tiago
 */

#ifndef CELLCONT_H_
#define CELLCONT_H_

#include <iostream>
#include <vector>
#include <set>
#include <opencv2/core/core.hpp>

#define PI 3.14159265

class CellCont {
public:
	CellCont();
	virtual ~CellCont();

	std::vector<double> getFeatures() const;
	std::set<int> getNeighbors() const;
	std::vector<double> getProbs() const;
	void setFeatures(std::vector<double> features);
	void setNeighbors(std::set<int> neighbors);
	void setProbs(std::vector<double> probs);
	double getCompactness();
	void removeNeighbor(int neighbor);

	void printCellInfo();

	static bool compareByNumNeighbors(CellCont a, CellCont b);
    int getCurLabel() const;
    void setCurLabel(int curLabel);

private:
	std::vector<double> features;
	std::set<int> neighbors;
	std::vector<double> probs;
	int curLabel;
};

#endif /* CELLCONT_H_ */
