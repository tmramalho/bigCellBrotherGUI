#include "createclassifier.h"
#include "operationscontroller.h"

CreateClassifier::CreateClassifier(OperationsController *_controller)
{
	controller = _controller;
	decider = new SVMachine(CellCont::getNumFeatures());
	int debug[] = {41, 98,346,482,503,597,605,705,735,889,936,951,965,1073,1122,1207,1283,1331,1340,1349,1384,1570,1666,1874,1982,2058,2090,2125,2294,2298};
	std::set<int> test(debug, debug+30);
	badLabelsbyFrame[0] = test;
}

void CreateClassifier::updateParameters() {

}

void CreateClassifier::execute()
{
	cv::Mat markers = controller->getPipelineImage(5);
	if(decider->isTrained()) {
		std::set<int> &currentbadLabels = badLabelsbyFrame[currentFrame];
		std::set<int> &currentgoodLabels = goodLabelsbyFrame[currentFrame];
		std::set<int> &svmBadLabels = svmBadLabelsbyFrame[currentFrame];
		std::set<int> &svmGoodLabels = svmGoodLabelsbyFrame[currentFrame];
		cv::Mat classifiedMarkers;
		markers.copyTo(classifiedMarkers);
		controller->is.setMarkersPic(classifiedMarkers);
		cv::Mat currentLabelMask(markers.size(), CV_8U);
		double min, max;
		cv::minMaxLoc(markers, &min, &max);
		for(int i = 2; i < max+1; i++) {
			if(currentbadLabels.count(i) > 0) {
				controller->is.deleteMarker(i);
				continue;
			} else if (currentgoodLabels.count(i) > 0) {
				continue;
			}
			cv::compare(markers, i, currentLabelMask, cv::CMP_EQ);
			if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
			CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, i);
			std::vector<double> probs = selectedCell.getFeatures();
			bool isCell = decider->classifyCell(probs);
			if(!isCell) {
				controller->is.deleteMarker(i);
				svmBadLabels.insert(i);
			} else {
				svmGoodLabels.insert(i);
			}
		}
		controller->setPipelineImage(6, classifiedMarkers);
	} else { //do nothing
		controller->setPipelineImage(6, markers);
	}
}

void CreateClassifier::createPreview()
{
	cv::Mat original = controller->getPipelineImage(2);
	cv::Mat markers = controller->getPipelineImage(5);
	std::set<int> &currentgoodLabels = goodLabelsbyFrame[currentFrame];
	std::set<int> currentbadLabels = badLabelsbyFrame[currentFrame];
	std::set<int> &svmBadLabels = svmBadLabelsbyFrame[currentFrame];
	std::set<int> &svmGoodLabels = svmGoodLabelsbyFrame[currentFrame];
	cv::Mat preview = PictureVis::drawClassyMarkersOnPicture(original, markers,
															 currentbadLabels, currentgoodLabels,
															 svmBadLabels, svmGoodLabels);
	controller->setPreview(preview);
}

void CreateClassifier::frameChanged(int fr)
{
	currentFrame = fr;
}

void CreateClassifier::cellPicked(int i, int j, int bt)
{
	if(controller->getCurrentStep() != "Classifier") return;
	cv::Mat markers = controller->getPipelineImage(5);
	int label = markers.at<int>(i, j);
	if(label <= 1) return;
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	cv::compare(markers, label, currentLabelMask, cv::CMP_EQ);
	std::cout << i << ", " << j << " : " << label << std::endl;
	if(ImageProcessor::checkIfEmpty(currentLabelMask)) return;
	std::set<int> &currentbadLabels = badLabelsbyFrame[currentFrame];
	std::set<int> &currentgoodLabels = goodLabelsbyFrame[currentFrame];
	if(bt == 1) {
		currentgoodLabels.insert(label);
		currentbadLabels.erase(label);
	} else {
		currentbadLabels.insert(label);
		currentgoodLabels.erase(label);
	}

	this->showPreview();
}

void CreateClassifier::saveTrainingSamples()
{
	std::set<int> &currentbadLabels = badLabelsbyFrame[currentFrame];
	std::set<int> &currentgoodLabels = goodLabelsbyFrame[currentFrame];
	cv::Mat markers = controller->getPipelineImage(5);
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	double min, max;
	cv::minMaxLoc(markers, &min, &max);
	std::vector<std::vector<double> > goodFeatures;
	std::vector<std::vector<double> > badFeatures;
	for(int i = 2; i < max+1; i++) {
		bool isBadCell = currentbadLabels.count(i) > 0;
		bool isGoodCell = currentgoodLabels.count(i) > 0;
		if(!isBadCell && !isGoodCell) continue;
		cv::compare(markers, i, currentLabelMask, cv::CMP_EQ);
		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, i);
		if(isBadCell) {
			badFeatures.push_back(selectedCell.getFeatures());
		} else {
			goodFeatures.push_back(selectedCell.getFeatures());
		}
	}

	decider->addTrainingSet(goodFeatures, badFeatures);
}

void CreateClassifier::applyTrainingSet()
{
	decider->createSVM();
	perform();
}

void CreateClassifier::markallGood()
{
	cv::Mat markers = controller->getPipelineImage(5);
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	double min, max;
	cv::minMaxLoc(markers, &min, &max);
	std::set<int> &currentgoodLabels = goodLabelsbyFrame[currentFrame];
	for(int i = 2; i < max+1; i++) {
		cv::compare(markers, i, currentLabelMask, cv::CMP_EQ);
		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		currentgoodLabels.insert(i);
	}
}

void CreateClassifier::markallBad()
{
	cv::Mat markers = controller->getPipelineImage(5);
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	double min, max;
	cv::minMaxLoc(markers, &min, &max);
	std::set<int> &currentbadLabels = badLabelsbyFrame[currentFrame];
	for(int i = 2; i < max+1; i++) {
		cv::compare(markers, i, currentLabelMask, cv::CMP_EQ);
		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		currentbadLabels.insert(i);
	}
}
