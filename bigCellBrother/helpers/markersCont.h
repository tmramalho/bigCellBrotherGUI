/*
 * markersCont.h
 *
 *  Created on: Jul 29, 2012
 *      Author: tiago
 */

#ifndef MARKERSCONT_H_
#define MARKERSCONT_H_
#include <vector>

using std::vector;

typedef struct markersCont {
	cv::Mat markers;
	int compCount;
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	vector<int> assocCompContour;
} markersCont;


#endif /* MARKERSCONT_H_ */
