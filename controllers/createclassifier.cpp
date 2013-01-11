#include "createclassifier.h"
#include "operationscontroller.h"

CreateClassifier::CreateClassifier()
{
	mode = true;
}

void CreateClassifier::execute()
{
	std::set<int> &currentRejectedLabels = rejectedLabelsbyFrame[currentFrame];
	std::set<int> &currentDetectedLabels = detectedLabelsbyFrame[currentFrame];
	cv::Mat markers = controller->getPipelineImage(5);
	cv::Mat classifiedMarkers;
	markers.copyTo(classifiedMarkers);
	controller->is.setMarkersPic(classifiedMarkers);
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	double min, max;
	cv::minMaxLoc(markers, &min, &max);
	for(int i = 2; i < max+1; i++) {
		if(currentRejectedLabels.count(i) > 0) {
			controller->is.deleteMarker(i);
			continue;
		}
		/* decider still needs work
		 *cv::compare(markers, i, currentLabelMask, cv::CMP_EQ);
		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, i);
		std::vector<double> probs = selectedCell.getFeatures();
		if(!controller->decider->classifyCell(probs)) {
			controller->is.deleteMarker(i);
			currentDetectedLabels.insert(i);
		}*/
	}
	controller->setPipelineImage(6, classifiedMarkers);
}

void CreateClassifier::createPreview()
{
	cv::Mat original = controller->getPipelineImage(2);
	cv::Mat markers = controller->getPipelineImage(5);
	std::set<int> currentRejectedLabels = rejectedLabelsbyFrame[currentFrame];
	std::set<int> currentDetectedLabels = detectedLabelsbyFrame[currentFrame];
	cv::Mat preview = PictureVis::drawClassyMarkersOnPicture(original, markers, currentRejectedLabels, currentDetectedLabels);
	controller->setPreview(preview);
}

void CreateClassifier::frameChanged(int fr)
{
	currentFrame = fr;
}

void CreateClassifier::cellPicked(int i, int j, int bt)
{
	if(controller->getCurrentStep() < 5) return;
	cv::Mat markers = controller->getPipelineImage(5);
	int label = markers.at<int>(i, j);
	if(label <= 1) return;
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	cv::compare(markers, label, currentLabelMask, cv::CMP_EQ);
	std::cout << i << ", " << j << " : " << label << " is " << mode << std::endl;
	if(ImageProcessor::checkIfEmpty(currentLabelMask)) return;
	//creates a new set if does not exist
	std::set<int> &currentRejectedLabels = rejectedLabelsbyFrame[currentFrame];
	if(mode == 1 && bt == 1) {
		//CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, label);
		//controller->decider->addTrainingSample(selectedCell.getFeatures());
		currentRejectedLabels.erase(label);
	} else {
		currentRejectedLabels.insert(label);
	}

	this->showPreview();
}

void CreateClassifier::setGoodMode() {
	mode = true;
}

void CreateClassifier::setBadMode() {
	mode = false;
}

void CreateClassifier::applyTrainingSet()
{
	std::set<int> &currentRejectedLabels = rejectedLabelsbyFrame[currentFrame];
	cv::Mat markers = controller->getPipelineImage(5);
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	double min, max;
	cv::minMaxLoc(markers, &min, &max);
	std::vector<std::vector<double> > goodFeatures;
	for(int i = 2; i < max+1; i++) {
		if(currentRejectedLabels.count(i) > 0) continue;
		cv::compare(markers, i, currentLabelMask, cv::CMP_EQ);
		if(ImageProcessor::checkIfEmpty(currentLabelMask)) continue;
		CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, i);
		goodFeatures.push_back(selectedCell.getFeatures());
	}

	controller->decider->addTrainingSet(goodFeatures);
	perform();
}
