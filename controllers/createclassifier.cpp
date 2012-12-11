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
	cv::Mat preview = PictureVis::drawMarkersOnPicture(original, markers);
	controller->setPreview(preview);
}

void CreateClassifier::cellPicked(int i, int j, int bt)
{
	if(controller->getCurrentStep() < 5) return;
	cv::Mat markers = controller->getPipelineImage(5);
	int label = markers.at<int>(i, j);
	if(label <= 1) return;
	if(mode == 1 && bt == 1) {
		cv::Mat currentLabelMask(markers.size(), CV_8U);
		cv::compare(markers, label, currentLabelMask, cv::CMP_EQ);
		std::cout << i << ", " << j << " : " << label << " is " << mode << std::endl;
		if(ImageProcessor::checkIfEmpty(currentLabelMask)) return;
		CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, label);
		controller->decider->addTrainingSample(selectedCell.getFeatures());

		emit cellFeaturesFound(selectedCell);
	}
}

void CreateClassifier::setGoodMode() {
	mode = true;
}

void CreateClassifier::setBadMode() {
	mode = false;
}
