/*
 * connectedcomponents.h
 *
 *  Created on: Aug 9, 2012
 *      Source: http://code.opencv.org/issues/1236
 */

#ifndef CONNECTEDCOMPONENTS_H_
#define CONNECTEDCOMPONENTS_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace cv;

//! computes the connected components labeled image of boolean image I with 4 or 8 way connectivity - returns N, the total
//number of labels [0, N-1] where 0 represents the background label.
int connectedComponents(Mat &L, const Mat &I, int connectivity = 8);

#endif /* CONNECTEDCOMPONENTS_H_ */
