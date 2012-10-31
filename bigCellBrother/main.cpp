#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageSegmentor.h"
#include "processors/ImageProcessor.h"
#include "helpers/PictureVis.h"
#include <cstdio>
#include "helpers/CSVReader.h"
#include "classifier/NaiveBayes.h"

int main(int argc, char *argv[]) {
	srand(time(NULL));
	ImageSegmentor is;
	/*CellClassifier decider; //simplest classifier
	decider.setHeight(55);
	decider.setWidth(12);
	decider.setHeightSigma(200);
	decider.setWidthSigma(16);
	decider.setProbThreshold(2);*/
	std::vector<std::vector<double> >values = CSVReader::readValues("data/ImageStacksFluorescenceGoodCells.csv");
	NaiveBayes decider;
	decider.addTrainingSet(values);
	decider.setProbThreshold(16);

	cv::VideoCapture capture("/media/sf_tiago/Downloads/ImageStacks_Brightfield_Fluorescence/compressed_10nm_wf_BF.avi");
	if (!capture.isOpened()) {
		std::cerr << "blooo" << std::endl;
		return 1;
	}

	cv::Size frameSize;
	frameSize.height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	frameSize.width  = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	cv::Mat frame; // current video frame

	double rate = capture.get(CV_CAP_PROP_FPS);
	int fourcc = capture.get(CV_CAP_PROP_FOURCC);
	cv::VideoWriter output("/media/sf_tiago/Downloads/ImageStacks_Brightfield_Fluorescence/compressed_10nm_wf_BF_Labeled.avi",
			fourcc, rate, frameSize);
	//cv::VideoWriter debug("/media/sf_tiago/Downloads/ImageStacks_Brightfield_Fluorescence/compressed_10nm_wf_BF_Markers.avi",
		//	fourcc, rate, frameSize);

	double duration;

	// for all frames in video
	while (true) {
		if (!capture.read(frame)) // read next frame if any
			break;

		duration = static_cast<double>(cv::getTickCount());

		cv::Mat improvImage = ImageProcessor::simplifyImage(frame, 3, 100);
		improvImage = ImageProcessor::invertImage(improvImage);
		cv::Mat boostedImage = ImageProcessor::simplifyImage(frame, 3, 100, true);
		boostedImage = ImageProcessor::invertImage(boostedImage);
		is.setOriginalImage(improvImage);
		is.setBoostedImage(boostedImage);

		cv::Mat thresholdBlobs = ImageProcessor::adaptiveThreshold(improvImage, -4, 35, false);
		thresholdBlobs = ImageProcessor::applyMorphologyOp(thresholdBlobs, cv::MORPH_OPEN, 7);
		thresholdBlobs = ImageProcessor::applyMorphologyOp(thresholdBlobs, cv::MORPH_CLOSE, 7);

		cv::Mat topHatBlobs = ImageProcessor::applyMorphologyOp(improvImage, cv::MORPH_TOPHAT, 21);
		topHatBlobs = ImageProcessor::threshold(topHatBlobs, 25, false);

		cv::Mat targetBlobs;
		cv::bitwise_and(thresholdBlobs, topHatBlobs, targetBlobs);
		targetBlobs = ImageProcessor::applyMorphologyOp(targetBlobs, cv::MORPH_OPEN, 7);
		targetBlobs = ImageProcessor::erode(targetBlobs, 3);

		cv::Mat backgroundMask = ImageProcessor::invertImage(targetBlobs);
		backgroundMask = ImageProcessor::erode(backgroundMask, 7);
		is.setBackgroundMask(backgroundMask);

		is.createMarkers(targetBlobs, 35, 15);
		cv::Mat wsMarkersBfNice = PictureVis::drawMarkers(is.getWatershedMarkers());
		//debug.write(wsMarkersBfNice);

		is.watershed();

		for (int n = 10; n < 200; n += 20) {
			is.removeSmallMarkers(n);
			is.watershed();
		}

		is.smoothLabels(9);

		cv::Mat markers = is.getMarkersPic();
		cv::Mat paintedWatershed = PictureVis::drawMarkersOnPicture(improvImage, markers);

		/*bool removedMarkers;
		for(double th = 100; th > 19; th -= 10) {
			do {
				decider.setProbThreshold(th);
				removedMarkers = is.removeSingleMarker();
				is.watershed();
				markers = is.getMarkersPic();
				paintedWatershed = PictureVis::drawMarkersOnPicture(improvImage, markers);
				cv::imshow("wsAfter", paintedWatershed);
				cv::waitKey(0);
			} while(removedMarkers);
		}*/

		is.findCellMarkers(&decider);
		markers = is.getMarkersPic();
		paintedWatershed = PictureVis::drawMarkersOnPicture(improvImage, markers);
		cv::imshow("wsAfter", paintedWatershed);
		cv::waitKey(0);

		output.write(paintedWatershed);

		duration = static_cast<double>(cv::getTickCount())-duration;
		duration /= cv::getTickFrequency();
		std::cout << "FRAME in " << duration << " s" << std::endl;
		//cv::Mat smoothWs = is.processLabels(watershed);
		//cv::Mat paintedSmoothWatershed = is.drawMarkersOnPicture(improvImage, smoothWs);
		//cv::imshow("Smooth ws result", paintedSmoothWatershed);
	}

	capture.release();

	return 0;
}
