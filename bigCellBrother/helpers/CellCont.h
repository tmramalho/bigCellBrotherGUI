/*
 * CellCont.h
 *
 *  Created on: Oct 9, 2012
 *      Author: tiago
 */

#ifndef CELLCONT_H_
#define CELLCONT_H_

#include <vector>
#include <set>
#include <opencv2/core/core.hpp>

class CellCont {
public:
	CellCont();
	virtual ~CellCont();

	std::vector<double> features;
	std::set<int> neighbors;
	std::vector<double> probs;
};

#endif /* CELLCONT_H_ */
