/*
 * markersCont.h
 *
 *  Created on: Jul 29, 2012
 *      Author: tiago
 */

#ifndef MARKERSCONT_H_
#define MARKERSCONT_H_
#include <vector>
#include <opencv2/core/core.hpp>

using std::vector;

typedef struct markersCont {
	cv::Mat markers;
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	vector<int> contourList;
} markersCont;


#endif /* MARKERSCONT_H_ */
