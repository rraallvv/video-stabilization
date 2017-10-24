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

	// set input video
	std::string video = argv[1];
	VideoCapture inputVideo(video);

	Mat frame;

	// get bounding box
	inputVideo >> frame;

	const string name = "output.mp4";
	const uchar threshold = 200;
	const int colors = 3;

	Size size = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

	//VideoWriter outputVideo;
	//outputVideo.open(name, CV_FOURCC('m', 'p', '4', '2'), inputVideo.get(CV_CAP_PROP_FPS), size, true);

	//if (!outputVideo.isOpened()){
	//	std::cout << "!!! Output video could not be opened" << std::endl;
	//	return 0;
	//}

	// do the tracking
	Mat T(2,3,CV_64F);
	int k=1;
	int max_frames = inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
	
	int total_rows = frame.rows;
	int total_cols = frame.cols;

	int top = 0;
	int bottom = total_rows - 1;
	int left = 0;
	int right = total_cols - 1;

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
		Mat preview = Mat::zeros(frame.rows, frame.cols, frame.type());
		frame.copyTo(preview(Range::all(), Range::all()));
				
		int hist[total_rows];
		
		bool top_end = false; 
		bool bottom_end = false; 
		
		for (int i = 0; i < total_rows; i++) {
  			uchar* data = frame.ptr<uchar>(i);	//the pointer to each row in the image
  			if (!top_end) {
  				int black_count = 0;
				for(int j = 0; j < total_cols; j++) {
					if (data[j * colors + 0] <= threshold
							&& data[j * colors + 1] <= threshold
							&& data[j * colors + 2] <= threshold) {
						black_count++;
					}
				}
	  			if (black_count == total_cols) {
	  				if (i > top) {
  						top = i;
  					}
  				} else {
  					top_end = true;
  				}
  			}
  			int k = total_rows - i - 1;
  			data = frame.ptr<uchar>(k);	//the pointer to each row in the image
  			if (!bottom_end) {
  				int black_count = 0;
				for(int j = 0; j < total_cols; j++) {
					if (data[j * colors + 0] <= threshold
							&& data[j * colors + 1] <= threshold
							&& data[j * colors + 2] <= threshold) {
						black_count++;
					}
				}
	  			if (black_count == total_cols) {
	  				if (k < bottom) {
  						bottom = k;
  					}
  				} else {
  					bottom_end = true;
  				}
  			}
		}
		
		bool left_end = false; 
		bool right_end = false; 

		for (int i = 0; i < total_cols; i++) {
  			if (!left_end) {
  			  	int black_count = 0;
				for(int j = 0; j < total_rows; j++) {
			  		uchar* data = frame.ptr<uchar>(j);	//the pointer to each row in the image
					if (data[i * colors + 0] <= threshold
							&& data[i * colors + 1] <= threshold
							&& data[i * colors + 2] <= threshold) {
						black_count++;
					}
				}
	  			if (black_count == total_cols) {
	  				if (i > left) {
  						left = i;
  					}
  				} else {
  					left_end = true;
  				}
  			}
  			int k = total_cols - i - 1;
  			if (!right_end) {
  			  	int black_count = 0;
				for(int j = 0; j < total_rows; j++) {
			  		uchar* data = frame.ptr<uchar>(j);	//the pointer to each row in the image
					if (data[k * colors + 0] <= threshold
							&& data[k * colors + 1] <= threshold
							&& data[k * colors + 2] <= threshold) {
						black_count++;
					}
				}
	  			if (black_count == total_cols) {
	  				if (k < right) {
  						right = k;
  					}
  				} else {
  					right_end = true;
  				}
  			}
		}
		
		rectangle(preview, Rect(left, top, right - left, bottom - top), Scalar(0, 0, 255), 2, 1);

		T.at<double>(0,0) = 1;
		T.at<double>(0,1) = 0;
		T.at<double>(1,0) = 0;
		T.at<double>(1,1) = 1;
				
		T.at<double>(0,2) = 0;
		T.at<double>(1,2) = 0;

		warpAffine(preview, preview, T, frame.size());

		// show image with the tracked object
		imshow("tracker", preview);

		//quit on ESC button
		if (waitKey(1) == 27) {
			break;
		}

		k++;
	}
	
	left++;
	top++;
	bottom--;
	right--;
	
	int center_x = (left + right) / 2;
	int center_y = (top + bottom) / 2;
	int width, height;
	width = height = MIN(right - left, bottom - top);	
	
	left = center_x - width / 2;
	top = center_y - height / 2;
	
	cout << width << ":" << height << ":" << left << ":" << top << endl;
}
