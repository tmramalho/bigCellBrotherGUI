#include "createclassifier.h"

CreateClassifier::CreateClassifier()
{
	mode = true;
}

void CreateClassifier::execute()
{

}

void CreateClassifier::createPreview()
{
	cv::Mat original = controller->getPipelineImage(2);
	cv::Mat markers = controller->getPipelineImage(5);
	std::set<int> currentRejectedLabels = rejectedLabelsbyFrame[currentFrame];
	cv::Mat preview = PictureVis::drawClassyMarkersOnPicture(original, markers, currentRejectedLabels);
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
	if(mode == 1 && bt == 1) {
		//CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, label);
		//controller->decider->addTrainingSample(selectedCell.getFeatures());
	} else {
		//creates a new set if does not exist
		std::set<int> &currentRejectedLabels = rejectedLabelsbyFrame[currentFrame];
		currentRejectedLabels.insert(label);
	}

	perform();
}

void CreateClassifier::setGoodMode() {
	mode = true;
}

void CreateClassifier::setBadMode() {
	mode = false;
}
