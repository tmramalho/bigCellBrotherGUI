#include "createclassifier.h"

CreateClassifier::CreateClassifier()
{
}

void CreateClassifier::execute()
{
	//change this so that the user opens a csv
	/*std::vector<std::vector<double> >values = CSVReader::readValues(
				"/home/tiago/Downloads/ImageStacksFluorescenceGoodCells.csv");
	decider = new NaiveBayes();
	decider->addTrainingSet(values);
	decider->setProbThreshold(1600);*/
}

void CreateClassifier::createPreview()
{
}

void CreateClassifier::cellPicked(int i, int j, int bt)
{
	cv::Mat markers = controller->getPipelineImage(5);
	int label = markers.at<int>(i, j);
	if(label <= 1) return;
	cv::Mat currentLabelMask(markers.size(), CV_8U);
	cv::compare(markers, label, currentLabelMask, cv::CMP_EQ);
	std::cout << i << ", " << j << " : " << label << " is " << bt << std::endl;
	if(ImageProcessor::checkIfEmpty(currentLabelMask)) return;
	CellCont selectedCell = CellCont::determineLabelProperties(currentLabelMask, markers, label);

	emit cellFeaturesFound(selectedCell);
}
