/*
 * ScientificProcessor.h
 *
 *  Created on: Oct 4, 2012
 *      Author: tiago
 */

#ifndef SCIENTIFICPROCESSOR_H_
#define SCIENTIFICPROCESSOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ImageProcessor.h"

class ScientificProcessor {
public:
	ScientificProcessor();
	virtual ~ScientificProcessor();
	cv::Mat processLabels(cv::Mat &markersPic);
};

#endif /* SCIENTIFICPROCESSOR_H_ */
