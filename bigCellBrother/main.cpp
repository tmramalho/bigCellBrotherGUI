#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "processors/ImageSegmentor.h"
#include "processors/ImageProcessor.h"

void thresh_callback(int t, void* c) {
	/*cv::Mat *target = (cv::Mat *)c;
	cv::Mat targetBlobs = is.adaptiveThreshold(*target, t-40, 31, false);

	cv::namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	cv::imshow( "Contours", targetBlobs );
	cv::namedWindow( "Source", CV_WINDOW_AUTOSIZE );
	cv::imshow( "Source", targetBlobs );

	int thresh = 0;
	cv::createTrackbar( "Adaptive thresh:", "Source", &thresh, 80, thresh_callback, &improvImage );
	thresh_callback( thresh, &improvImage );*/
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	ImageSegmentor is;
	//cv::Mat bfOriginalImage = cv::imread("data/cells_bf.png");

	cv::VideoCapture capture("/media/sf_tiago/Downloads/ImageStacks_Brightfield_Fluorescence/compressed_10nm_wf_BF.avi");
	if (!capture.isOpened()) {
		std::cerr << "blooo" << std::endl;
		return 1;
	}

	//double rate= capture.get(CV_CAP_PROP_FPS);
	cv::Mat frame; // current video frame
	cv::namedWindow("Extracted Frame");

	// for all frames in video
	while (true) {
		char c = cv::waitKey(0);
		if (c == 27) break;
		if (c == 32) {
			if (!capture.read(frame)) // read next frame if any
				break;
			cv::Mat improvImage = ImageProcessor::simplifyImage(frame, 3, 100);
			improvImage = ImageProcessor::invertImage(improvImage);
			cv::imshow("Extracted Frame",improvImage);

			cv::Mat thresholdBlobs = ImageProcessor::adaptiveThreshold(improvImage, -4, 35, false);
			thresholdBlobs = ImageProcessor::applyMorphologyOp(thresholdBlobs, cv::MORPH_OPEN, 7);
			thresholdBlobs = ImageProcessor::applyMorphologyOp(thresholdBlobs, cv::MORPH_CLOSE, 7);

			cv::Mat topHatBlobs = ImageProcessor::applyMorphologyOp(improvImage, cv::MORPH_TOPHAT, 21);
			topHatBlobs = ImageProcessor::threshold(topHatBlobs, 25, false);

			cv::Mat finalBlobs;
			cv::bitwise_and(thresholdBlobs, topHatBlobs, finalBlobs);
			finalBlobs = ImageProcessor::applyMorphologyOp(finalBlobs, cv::MORPH_OPEN, 7);
			cv::imshow("blobs", finalBlobs);

			cv::Mat backgroundMask = ImageProcessor::invertImage(finalBlobs);
			backgroundMask = ImageProcessor::erode(backgroundMask, 7);

			markersCont mcBfNice = is.makeNiceMarkers(finalBlobs, 50, 60, 12, 30, 40);
			cv::Mat wsMarkersBfNice = is.drawMarkers(mcBfNice);
			cv::imshow("markersBf2", wsMarkersBfNice);

			cv::Mat wsMarkersFinal = is.addBackgroundMask(mcBfNice.markers, backgroundMask);

			cv::Mat watershed = is.watershed(improvImage, wsMarkersFinal);

			cv::Mat paintedWatershed = is.drawMarkersOnPicture(improvImage, watershed);

			cv::imshow("Watershed result", paintedWatershed);

		}
	}

	capture.release();

	return 0;
}
