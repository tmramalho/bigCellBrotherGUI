/*
 * PictureVis.h
 *
 *  Created on: Oct 4, 2012
 *      Author: tiago
 */

#ifndef PICTUREVIS_H_
#define PICTUREVIS_H_
#include "markersCont.h"

#include <vector>
#include <cstdio>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "../helpers/CellCont.h"

#define WHITE 255
#define BLACK 0

class PictureVis {
public:
	PictureVis();
	virtual ~PictureVis();
	static cv::Mat drawMarkers(const markersCont &mc);
	static cv::Mat drawMarkersOnPicture(cv::Mat& targetPicture, cv::Mat& markers);
    static cv::Mat highlightMarker(cv::Mat& targetPicture, cv::Mat& markers, int parentLabel);
    static cv::Mat highlightMarkerVector(cv::Mat& targetPicture, cv::Mat& markers, std::vector<int> &childLabels);
	static cv::Mat drawClassyMarkersOnPicture(cv::Mat& targetPicture, cv::Mat& markers,
			std::set<int> markedBadLabels, std::set<int> markedGoodLabels, std::set<int> foundBadLabels,
			std::set<int> foundGoodLabels);
    static cv::Mat drawParentMarkersOnPicture(cv::Mat& targetPicture, cv::Mat& markers,
            std::set<int> &parentCells);
    static cv::Mat drawChildMarkersOnPicture(cv::Mat& targetPicture, cv::Mat& markers,
            std::map<int, std::vector<double> > &childCells);
    static cv::Mat drawCellsOnPicture(cv::Mat& targetPicture, cv::Mat& markers,
            std::vector<CellCont>& cellVector, std::vector<CellCont>& prevCellVector, int frameNum);
	static void drawRotatedRect(cv::Mat &mask, vector<cv::RotatedRect> &boxes);
	static void drawRotatedRect(cv::Mat &mask, cv::RotatedRect &boxes);

private:
	static void drawMinAreaRect(cv::Mat &target, cv::RotatedRect &box, cv::Vec3b &color);
	static vector<cv::Vec3b> getRandomColorTab(int numComp);
};

#endif /* PICTUREVIS_H_ */
