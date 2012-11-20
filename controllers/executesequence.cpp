#include "executesequence.h"

ExecuteSequence::ExecuteSequence(OperationsController *opCtr)
{
	ops = opCtr;
	distanceCutoff = 0;
	fs = NULL;
	ffs = NULL;
	dotFilename = "";
	csvFilename = "";
	haveFluorescence = false;
	executed = false;
}

void ExecuteSequence::debugSequence(int frameNum)
{
	cv::Mat nextFrame = fs->grabFrameNumber(frameNum);
	ops->resetPipeline(nextFrame);
	cv::Mat markers = ops->runFullPipeline();
	cv::Mat preview = ops->getPreviewForOperation(1);
	cv::Mat target;
	char buf[512];
	if(preview.type() == CV_8U)
		cv::cvtColor(preview, target, CV_GRAY2RGB);
	else
		preview.copyTo(target);
	std::vector<std::vector<CellCont> > allCells = lab.getAllCells();
	std::vector<CellCont> cellVector = allCells[frameNum];
	std::vector<CellCont> prevCellVector;
	if(frameNum > 0) prevCellVector = allCells[frameNum-1];

	for(std::vector<CellCont>::iterator it = cellVector.begin();
		it != cellVector.end(); it++) {
		cv::Scalar colorScalar = cv::Scalar(cv::Vec3b(255,255,255));
		cv::Rect bbox = it->getBoundBox();
		sprintf(buf, "%d", it->getCurLabel());
		cv::putText(target, buf, cv::Point(bbox.x, bbox.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, colorScalar);
		double fluorescence = it->getFluorescence();
		if(fluorescence > 0) {
			sprintf(buf, "%f", fluorescence);
			cv::putText(target, buf, cv::Point(bbox.x, bbox.y + bbox.width), cv::FONT_HERSHEY_SIMPLEX, 0.5, colorScalar);
		}
		cv::rectangle(target, bbox, colorScalar);
		int parentLabel = it->getPrevLabel();
		if(parentLabel > 0 && frameNum > 0) {
			for(std::vector<CellCont>::iterator jt = prevCellVector.begin();
				jt != prevCellVector.end(); jt++) {
				if(jt->getCurLabel() == parentLabel) {
					cv::line(target, jt->getCenter(), it->getCenter(), colorScalar);
					break;
				}
			}
		}
	}

	ops->showSelectedPreview(target);
}

void ExecuteSequence::run()
{
	if(!ops->pipelineReady || !fs->isLoaded()) return;
	int maxFrames = 10; //fs->numFrames;
	if(haveFluorescence) lab.setUseFluor(true);
	for(int i=0; i<maxFrames; i++) {
		cv::Mat nextFrame = fs->grabFrameNumber(i);
		ops->resetPipeline(nextFrame);
		cv::Mat result = ops->runFullPipeline();
		lab.setMarkersPic(result);
		if(haveFluorescence) {
			cv::Mat fluor = ffs->grabFrameNumber(i);
			lab.setFluorescencePic(fluor);
		}
		lab.processLabels(i, ops->decider, distanceCutoff);
		emit incrementProgress((i+1)*100/maxFrames);
	}
	lab.createDotFile(dotFilename);
	lab.createCsvFile(csvFilename);
	executed = true;
	emit sequenceDone();
}
