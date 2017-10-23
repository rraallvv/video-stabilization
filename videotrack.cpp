/*----------------------------------------------
 * Usage:
 * example_tracking_multitracker <video_name> [algorithm]
 *
 * example:
 * videotrack Bolt/img/%04d.jpg
 * videotrack faceocc2.webm KCF
 *--------------------------------------------------*/

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>
#include "samples_utility.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
	// show help
	if (argc < 2) {
		cout<<
			" Usage: example_tracking_multitracker <video_name> [algorithm]\n"
			" examples:\n"
			" example_tracking_multitracker Bolt/img/%04d.jpg\n"
			" example_tracking_multitracker faceocc2.webm MEDIANFLOW\n"
			<< endl;
		return 0;
	}

	// set the default tracking algorithm
	std::string trackingAlg = "KCF";

	// set the tracking algorithm from parameter
	if (argc > 2) {
		trackingAlg = argv[2];
	}

	// create the tracker
	MultiTracker trackers;

	// container of the tracked objects
	vector<Rect2d> objects;

	// set input video
	std::string video = argv[1];
	VideoCapture inputVideo(video);

	Mat frame;

	// get bounding box
	inputVideo >> frame;
	vector<Rect> ROIs;
	selectROIs("tracker", frame, ROIs);

	//quit when the tracked object(s) is not provided
	if (ROIs.size() < 1) {
		return 0;
	}

	// initialize the tracker
	std::vector<Ptr<Tracker> > algorithms;
	for (size_t i = 0; i < ROIs.size(); i++) {
		algorithms.push_back(createTrackerByName(trackingAlg));
		objects.push_back(ROIs[i]);
	}

	trackers.add(algorithms, frame, objects);

	const string name = "output.mp4";
	Size size = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

	VideoWriter outputVideo;
	outputVideo.open(name, CV_FOURCC('m', 'p', '4', '2'), inputVideo.get(CV_CAP_PROP_FPS), size, true);

	if (!outputVideo.isOpened()){
		std::cout << "!!! Output video could not be opened" << std::endl;
		return 0;
	}

	// do the tracking
	Mat T(2,3,CV_64F);
	int k=1;
	int max_frames = inputVideo.get(CV_CAP_PROP_FRAME_COUNT);

	printf("Start the tracking process, press ESC to quit.\n");
	while(true) {
		// get frame from the video
		inputVideo >> frame;

		if(frame.data == NULL) {
			break;
		}

		// stop the program if no more images
		if (frame.rows == 0 || frame.cols == 0) {
			break;
		}

		//update the tracking result
		trackers.update(frame);

		// get the average movement for the tracked objects
		float dx = 0;
		float dy = 0;
		for (unsigned i=0; i<trackers.getObjects().size(); i++) {
			Rect ROI = ROIs[i];
			Rect rect = trackers.getObjects()[i];
			dx += ROI.x - rect.x;
			dy += ROI.y - rect.y;
			//rectangle(frame, rect, Scalar(255, 0, 0), 2, 1);
		}
		dx /= trackers.getObjects().size();
		dy /= trackers.getObjects().size();

		T.at<double>(0,0) = 1;
		T.at<double>(0,1) = 0;
		T.at<double>(1,0) = 0;
		T.at<double>(1,1) = 1;

		T.at<double>(0,2) = dx;
		T.at<double>(1,2) = dy;

		Mat canvas = Mat::zeros(frame.rows, frame.cols, frame.type());

		warpAffine(frame, canvas, T, frame.size());

		//frame.copyTo(canvas(Range::all(), Range(0, frame.cols)));

		// show image with the tracked object
		imshow("tracker", canvas);

		// save the video
		outputVideo.write(canvas);

		// save the images
		char str[256];
		sprintf(str, "images/%04d.png", k);
		imwrite(str, canvas);

		//quit on ESC button
		if (waitKey(1) == 27) {
			break;
		}

		k++;
	}
}
