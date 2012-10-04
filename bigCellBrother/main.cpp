#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageSegmentor.h"
#include "processors/ImageProcessor.h"
#include "helpers/PictureVis.h"

int main(int argc, char *argv[]) {
	srand(time(NULL));
	ImageSegmentor is;
	//cv::Mat bfOriginalImage = cv::imread("data/cells_bf.png");

	cv::VideoCapture capture("/media/sf_tiago/Downloads/ImageStacks_Brightfield_Fluorescence/compressed_10nm_wf_BF.avi");
	if (!capture.isOpened()) {
		std::cerr << "blooo" << std::endl;
		return 1;
	}

	double rate = capture.get(CV_CAP_PROP_FPS);
	double fourcc = capture.get(CV_CAP_PROP_FOURCC);
	cv::Size frameSize;
	frameSize.height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	frameSize.width  = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	cv::Mat frame; // current video frame

	cv::VideoWriter output("/media/sf_tiago/Downloads/ImageStacks_Brightfield_Fluorescence/compressed_10nm_wf_BF_Labeled.avi",
			fourcc, rate, frameSize);
	cv::VideoWriter debug("/media/sf_tiago/Downloads/ImageStacks_Brightfield_Fluorescence/compressed_10nm_wf_BF_Markers.avi",
			fourcc, rate, frameSize);

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

		cv::Mat thresholdBlobs = ImageProcessor::adaptiveThreshold(improvImage, -4, 35, false);
		thresholdBlobs = ImageProcessor::applyMorphologyOp(thresholdBlobs, cv::MORPH_OPEN, 7);
		thresholdBlobs = ImageProcessor::applyMorphologyOp(thresholdBlobs, cv::MORPH_CLOSE, 7);

		cv::Mat topHatBlobs = ImageProcessor::applyMorphologyOp(improvImage, cv::MORPH_TOPHAT, 21);
		topHatBlobs = ImageProcessor::threshold(topHatBlobs, 25, false);

		cv::Mat finalBlobs;
		cv::bitwise_and(thresholdBlobs, topHatBlobs, finalBlobs);
		finalBlobs = ImageProcessor::applyMorphologyOp(finalBlobs, cv::MORPH_OPEN, 7);
		finalBlobs = ImageProcessor::erode(finalBlobs, 3);

		cv::Mat backgroundMask = ImageProcessor::invertImage(finalBlobs);
		backgroundMask = ImageProcessor::erode(backgroundMask, 7);

		markersCont mcBfNice = is.createMarkers(finalBlobs, 35, 15);
		cv::Mat wsMarkersBfNice = PictureVis::drawMarkers(mcBfNice);
		debug.write(wsMarkersBfNice);

		cv::Mat wsMarkersFinal = is.addBackgroundMask(mcBfNice.markers, backgroundMask);

		cv::Mat watershed = is.watershed(boostedImage, wsMarkersFinal);

		for (int n = 10; n < 100; n += 20) {
			is.removeSmallMarkers(watershed, n);
			watershed = is.addBackgroundMask(watershed, backgroundMask);
			watershed = is.watershed(boostedImage, watershed);
		}

		cv::Mat paintedWatershed = PictureVis::drawMarkersOnPicture(improvImage, watershed);
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
