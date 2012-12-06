#include "createclassifier.h"

CreateClassifier::CreateClassifier()
{
	//this should not be initialized here
	//controller->decider = new NaiveBayes();
	//controller->decider->setProbThreshold(1600);
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
	if(bt == 1) {
		cv::Mat currentLabelMask(markers.size(), CV_8U);
		cv::compare(markers, label, currentLabelMask, cv::CMP_EQ);
		std::cout << i << ", " << j << " : " << label << " is " << bt << std::endl;
		if(ImageProcessor::checkIfEmpty(currentLabelMask)) return;
		CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, label);
		featureList.push_back(selectedCell.getFeatures());
		//controller->decider->addTrainingSet(featureList);

		emit cellFeaturesFound(selectedCell);
	}
}
