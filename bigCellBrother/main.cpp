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
		if (c != 0) {
			if (!capture.read(frame)) // read next frame if any
				break;

			cv::imshow("Extracted Frame",frame);

			cv::Mat improvImage = ImageProcessor::simplifyImage(frame, 5, 100);
			improvImage = ImageProcessor::invertImage(improvImage);

			cv::Mat targetBlobs = ImageProcessor::adaptiveThreshold(improvImage, -4, 35, false);

			cv::Mat backgroundMask = ImageProcessor::floodBackground(improvImage, 1105, 86, 3);
			cv::bitwise_or(backgroundMask, ImageProcessor::invertImage(targetBlobs), backgroundMask);
			backgroundMask = ImageProcessor::applyMorphologyOp(backgroundMask, cv::MORPH_CLOSE, 7);
			backgroundMask = ImageProcessor::erode(backgroundMask, 5);

			cv::subtract(targetBlobs, backgroundMask, targetBlobs);

			cv::Mat distTrans = ImageProcessor::distanceTransform(targetBlobs);
			distTrans = ImageProcessor::threshold(distTrans, 80, false);
			distTrans = ImageProcessor::applyMorphologyOp(distTrans, cv::MORPH_OPEN, 3);

			markersCont mcBfNice = is.makeNiceMarkers(distTrans, 50, 60, 12, 30, 40);
			cv::Mat wsMarkersBfNice = is.drawMarkers(mcBfNice);
			cv::imshow("markersBf2", wsMarkersBfNice);

			cv::Mat wsMarkersFinal = is.addBackgroundMask(mcBfNice.markers, backgroundMask);

			cv::Mat watershed = is.watershed(improvImage, wsMarkersFinal);

		}
	}

	capture.release();

	return 0;
}
