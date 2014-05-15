#include "executesequence.h"

#include <QMessageBox>

ExecuteSequence::ExecuteSequence(OperationsController *opCtr)
{
	ops = opCtr;
	fs = NULL;
	dotFilename = "";
	csvFilename = "";
	haveFluorescence = false;
    executed = false;
    saveFrames = false;
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
    cv::Mat target = PictureVis::drawMarkersOnPicture(original, markers);

    ops->showSelectedPreview(target);
}

void ExecuteSequence::setFileSource(FileContainer *source)
{
    fs = source;
    ffs.clear();
    haveFluorescence = false;
    emit brightfieldAdded(fs->getFilepath());
}

void ExecuteSequence::setFluorFileSource(FileContainer *source)
{
     ffs.push_back(source);
     haveFluorescence = true;
     emit fluorescenceAdded(source->getFilepath());
}

void ExecuteSequence::analyseFrame(FileContainer *bfPic, std::vector<FileContainer *> &flPics, int i,
                                   std::fstream &filestrDOT, std::fstream &filestrCSV) {
    cv::Mat nextFrame = bfPic->grabFrameNumber(i);
    ops->resetPipeline(nextFrame);
    cv::Mat currentMarkers = ops->runFullPipeline();

    lab.setMarkersPic(currentMarkers);
    if(haveFluorescence) {
        for (unsigned int j=0; j < flPics.size(); j++) {
            cv::Mat fluor = flPics[j]->grabFrameNumber(i);
            cv::Mat croppedFluor = ops->transformImage(fluor);
            lab.addFluorescencePic(croppedFluor, j);
        }
    }
    lab.processLabels(i);
    lab.setPreviousMarkersPic(currentMarkers);

    lab.updateDotFile(filestrDOT, i);
    lab.updateCsvFile(filestrCSV, i);

    if (saveFrames) {
        std::vector<CellCont> cellArray = lab.getCurrentCells();
        std::vector<CellCont> prevCellArray;
        if (i > 0) prevCellArray = lab.getPreviousCells();
        cv::Mat original = ops->getPipelineImage(2);
        cv::Mat target = PictureVis::drawCellsOnPicture(original, currentMarkers, cellArray, prevCellArray, i);
        size_t index = 0;
        index = aviFilename.find(".avi", index);
        std::string fn = aviFilename;
        std::ostringstream sstr;
        sstr << "_frame" << i << ".jpg";
        fn.replace(index, 4, sstr.str());
        cv::imwrite(fn, target);
    }
}

void ExecuteSequence::run()
{
    if(!ops->pipelineReady || !fs->isLoaded()) return;
    allowRun = 1;
    int maxFrames = fs->getNumFrames();
    if(haveFluorescence) lab.setUseFluor(true);
    std::cout << ffs.size() << std::endl;
    std::fstream filestrCSV(csvFilename.c_str(), std::fstream::trunc | std::fstream::out);
    std::fstream filestrDOT(dotFilename.c_str(), std::fstream::trunc | std::fstream::out);
    lab.createDotFile(filestrDOT);
    lab.createCsvFile(filestrCSV, ffs.size());
    lab.setFirstFrame(true);

	for(int i=0; i<maxFrames; i++) {
        //hackish way to cancel thread running
        if(allowRun == 0) return;
        clock_t startTime = clock();
        analyseFrame(fs, ffs, i, filestrDOT, filestrCSV);
        std::cout << double( clock() - startTime ) / (double)CLOCKS_PER_SEC << ". " << std::endl;

        float elapsed = ( clock() - startTime ) / (double)CLOCKS_PER_SEC;
        float progressEstimate = (i+1)/(double)maxFrames*100;
        emit incrementProgress(elapsed, progressEstimate);

    }

    lab.finishCsvFile(filestrCSV);
    lab.finishDotFile(filestrDOT);
    lab.reset();
	executed = true;
    emit sequenceDone();
}

FileContainer *ExecuteSequence::openFile(QFileInfo &fi) {
    QString ext = fi.suffix();
    FileContainer *videoBox;
    if(ext == "avi")
        videoBox = new VideoContainer();
    else if(ext == "tif" || ext == "tiff")
        videoBox = new TiffContainer();
    else {
       throw 1;
    }

    const std::string filenameString(fi.absoluteFilePath().toLocal8Bit().constData());
    videoBox->openFile(filenameString);
    std::cout << filenameString << std::endl;

    if (!videoBox->isLoaded()) {
        throw 2;
    }

    return videoBox;
}

void ExecuteSequence::batchRun(std::vector<QFileInfoList> &stacks)
{
    if(!ops->pipelineReady) return;
    allowRun = 1;
    int numStacks = stacks.at(0).count();
    for(int i=0; i < numStacks; i++) {
        std::cout << i << "th stack" << std::endl;
        FileContainer *bfFile;
        try {
            QFileInfo file = stacks.at(0).at(i);
            bfFile = openFile(file);
        } catch(int ei) {
            std::cerr << ei << std::endl;
            continue;
        }
        std::vector<FileContainer *> flFileVector;
        for(int j = 1; j < stacks.size(); j++) {
            if(i >= stacks.at(j).count()) continue;
            try {
                QFileInfo file = stacks.at(j).at(i);
                FileContainer *flFile = openFile(file);
                flFileVector.push_back(flFile);
            } catch(int ei) {
                std::cerr << ei << std::endl;
                continue;
            }
        }
        int maxFrames = bfFile->getNumFrames();
        if(flFileVector.size() > 0) {
            haveFluorescence = true;
            lab.setUseFluor(true);
        } else {
            haveFluorescence = false;
            lab.setUseFluor(false);
        }
        QString path = stacks.at(0).at(i).absolutePath().append("/");
        QString name = stacks.at(0).at(i).baseName();
        QString csvNameString = path + name + ".csv";
        QString dotNameString = path + name + ".dot";
        std::fstream filestrCSV(csvNameString.toLocal8Bit().constData(),
                                std::fstream::trunc | std::fstream::out);
        std::fstream filestrDOT(dotNameString.toLocal8Bit().constData(),
                                std::fstream::trunc | std::fstream::out);
        lab.createDotFile(filestrDOT);
        lab.createCsvFile(filestrCSV, flFileVector.size());
        lab.setFirstFrame(true);

        QString avi = path + name + ".avi";
        aviFilename = avi.toLocal8Bit().constData();
        float e_est;

        for(int n=0; n<maxFrames; n++) {
            //hackish way to cancel thread running
            if(allowRun == 0) return;
            std::cout << n << "th frame" << std::endl;
            //actual work
            clock_t startTime = clock();
            analyseFrame(bfFile, flFileVector, n, filestrDOT, filestrCSV);
            float elapsed = ( clock() - startTime ) / (double)CLOCKS_PER_SEC;
            float progressEstimate = (i*maxFrames+n)/((float) maxFrames*numStacks)*100;
            //moving average estimate
            if (n == 0) e_est = elapsed;
            else e_est = 0.01 * elapsed + 0.99 * e_est;
            float remaining = e_est * (maxFrames*numStacks - (i*maxFrames+n));
            emit incrementProgress(remaining, progressEstimate);
        }

        lab.finishCsvFile(filestrCSV);
        lab.finishDotFile(filestrDOT);
        lab.reset();

        delete bfFile;
        flFileVector.clear();
    }

    emit sequenceDone();
}

void ExecuteSequence::cancelRun()
{
    allowRun = 0;
}
