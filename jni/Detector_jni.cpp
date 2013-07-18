#include <jni.h>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>

#include <opencv2/objdetect/objdetect.hpp>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/highgui/highgui_c.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

extern "C" {
string face_cascade_name =
		"/mnt/sdcard/external_sd/hyraxv2/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
std::vector<Mat> faces;
std::vector<String> fileNames;
static void detectFaceFromVec(Mat images) {

	Mat frame_gray;
	std::vector<Rect> foundFace;
	cvtColor(images, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, foundFace, 1.1, 2,
			0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (int j = 0; j < foundFace.size(); j++) {
		Mat tempFace = images(foundFace[j]);
		cvtColor(tempFace, tempFace, COLOR_BGR2GRAY);
		faces.push_back(tempFace);
	}
}

static void read_csv(const string& filename, vector<Mat>& images,
		vector<int>& labels, char separator = '\n') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message =
				"No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		if (!path.empty()) {
			images.push_back(imread(path, CV_LOAD_IMAGE_COLOR));
			fileNames.push_back(path);

		}
	}
}

JNIEXPORT void JNICALL Java_MainActivity_detector(JNIEnv *env, jobject obj,
		jint subjectLabel) {
	// Get the path to your CSV.
	string fn_csv = string("/mnt/sdcard/external_sd/hyraxv2/testImages.csv");
	// These vectors hold the images and corresponding labels.
	//vector<Mat> nimages;
	vector<Mat> images;
	vector<int> labels;
	vector<int> results;
	// Read in the data. This can fail if no valid
	// input filename is given.
	try {
		read_csv(fn_csv, images, labels);
		printf("Finished reading CSV file...\n");
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg
				<< endl;
		// nothing more we can do
		exit(1);
	}

	printf("Loading Cascade...\n");
	if (!face_cascade.load(face_cascade_name)) {
		printf("--(!)Error loading\n");
		return;
	};
	printf("Cascade loaded successfully...\n");

	//
	// Now create a new Eigenfaces Recognizer
	//
	Ptr<FaceRecognizer> model1 = createEigenFaceRecognizer(0, 3.0);
	model1->load("eigenfaces_at.yml");
	CvSize size = cvSize(128, 128);
	printf("Detecting faces....\n");
	Mat tempFace;
	Mat tempFace2;
	for (int i = 0; i < images.size(); i++) {
		tempFace = images[i];
		detectFaceFromVec(tempFace);
		printf("Detected %lu Faces successfully from image %d...\n",
				faces.size(), i);

		for (int j = 0; j < faces.size(); j++) {
			tempFace2 = faces[j];
			cv::resize(tempFace2, tempFace2, size, 0, 0, CV_INTER_LINEAR);

			int predictedLabel = -1;
			double confidence = 0.0;
			// model1->predict(testImage, predictedLabel, confidence);
			predictedLabel = model1->predict(tempFace2);
			if (predictedLabel == subjectLabel) {
				results.push_back(i);
				break;
			}
		}
		faces.clear();
	}
	ofstream myfile("/mnt/sdcard/external_sd/hyraxv2/results.csv");
	for (int n = 0; n < results.size(); n++)
		myfile << fileNames[results[n]] << '\n';
	fileNames.clear();
}
}
