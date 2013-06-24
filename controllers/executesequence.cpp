#include "executesequence.h"

ExecuteSequence::ExecuteSequence(OperationsController *opCtr)
{
	ops = opCtr;
	fs = NULL;
	dotFilename = "";
	csvFilename = "";
	haveFluorescence = false;
    executed = false;
}

ExecuteSequence::~ExecuteSequence()
{
    ffs.erase(ffs.begin(), ffs.end());
}

void ExecuteSequence::debugSequence(int frameNum)
{
	cv::Mat nextFrame = fs->grabFrameNumber(frameNum);
	ops->resetPipeline(nextFrame);
	cv::Mat markers = ops->runFullPipeline();
	cv::Mat original = ops->getPipelineImage(2);
	cv::Mat target = PictureVis::drawCellsOnPicture(original, markers, lab.getAllCells(), frameNum);

    ops->showSelectedPreview(target);
}

void ExecuteSequence::setFluorFileSource(FileContainer *source)
{
     ffs.push_back(source);
     haveFluorescence = true;
}

void ExecuteSequence::run()
{
    if(!ops->pipelineReady || !fs->isLoaded()) return;
    int maxFrames = fs->getNumFrames();
    if(haveFluorescence) lab.setUseFluor(true);
	cv::Size finalSize;
    std::cout << ffs.size() << std::endl;

	for(int i=0; i<maxFrames; i++) {
        cv::Mat nextFrame = fs->grabFrameNumber(i);
        ops->resetPipeline(nextFrame);
        cv::Mat currentMarkers = ops->runFullPipeline();
		finalSize = currentMarkers.size();
        lab.setMarkersPic(currentMarkers);
        if(haveFluorescence) {
            for (unsigned int j=0; j < ffs.size(); j++) {
                cv::Mat fluor = ffs[j]->grabFrameNumber(i);
                cv::Mat croppedFluor = ops->cropImage(fluor);
                lab.addFluorescencePic(croppedFluor, j);
            }
        }
        lab.processLabels(i);
        lab.setPreviousMarkersPic(currentMarkers);
		std::cout << "FRAME" << i << std::endl;
		emit incrementProgress((i+1)*100/maxFrames);
	}

	double fourcc = CV_FOURCC('M', 'J', 'P', 'G');
    cv::VideoWriter output(aviFilename, fourcc, 5, finalSize);
	for(int i=0; i<maxFrames; i++) {
        cv::Mat nextFrame = fs->grabFrameNumber(i);
        ops->resetPipeline(nextFrame);
        cv::Mat result = ops->runFullPipeline();
        cv::Mat original = ops->getPipelineImage(2);
        cv::Mat target = PictureVis::drawCellsOnPicture(original, result, lab.getAllCells(), i);
		output << target;
		std::cout << "saved frame " << i << " : " << target.size().height << ", " << target.size().width << std::endl;
	}
    lab.createDotFile(dotFilename);
    lab.createCsvFile(csvFilename);
	executed = true;
	emit sequenceDone();
}
