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
#include <algorithm>
#include <android/log.h>
using namespace cv;
using namespace std;

extern "C" {
string face_cascade_name =
		"/mnt/sdcard/external_sd/hyraxv2/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
std::vector<Mat> faces;
static void detectFaceFromVec(Mat images) {

	Mat frame_gray = images;
	std::vector<Rect> foundFace;
	//cvtColor(images, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "Detect face");
	face_cascade.detectMultiScale(frame_gray, foundFace, 1.1, 2,
			0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "found face of size %lu",
			foundFace.size());
	for (int j = 0; j < foundFace.size(); j++) {
		Mat tempFace = images(foundFace[j]);
		//cvtColor(tempFace, tempFace, COLOR_BGR2GRAY);
		faces.push_back(tempFace);
	}
}

static void read_csv(const string& filename, vector<Mat>& images,
		vector<string>& fileNames, char separator = '\n') {
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
		/*Removing quotes from path*/
		path.erase(remove(path.begin(), path.end(), '\"'), path.end());
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "Path:%s\n",
				path.c_str());
		if (!path.empty()) {
			/*Still need to check if path gives a null image*/
			images.push_back(imread(path, CV_LOAD_IMAGE_GRAYSCALE));
			fileNames.push_back(path);
		}
	}
}

JNIEXPORT void JNICALL Java_hyrax_v2_personfinder_MainActivity_detector(
		JNIEnv *env, jobject obj, jint subjectLabel) {
	// Get the path to your CSV.
	string fn_csv = string("/mnt/sdcard/external_sd/hyraxv2/testImages.csv");
	// These vectors hold the images and corresponding labels.
	//vector<Mat> nimages;
	vector<Mat> images;
	vector<int> labels;
	vector<int> results;

	std::vector<string> fileNames;
	// Read in the data. This can fail if no valid
	// input filename is given.
	try {
		read_csv(fn_csv, images, fileNames);
		printf("Finished reading CSV file...\n");
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG",
				"Finished reading the CSV");
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
	__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG",
			"Cascade Loaded Successfully");

	//
	// Now create a new Eigenfaces Recognizer
	//
	Ptr<FaceRecognizer> model1 = createEigenFaceRecognizer(200);
	model1->load("/mnt/sdcard/external_sd/hyraxv2/eigenfaces_at.yml");
	CvSize size = cvSize(80, 80);
	printf("Detecting faces....\n");


	for (int i = 0; i < images.size(); i++) {
		Mat tempFace = images[i];
		detectFaceFromVec(tempFace);
		printf("Detected %lu Faces successfully from image %d...\n",
				faces.size(), i);
		__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG",
				"Detected %lu Faces successfully from image %d...\n",
				faces.size(), i);
		for (int j = 0; j < faces.size(); j++) {
			Mat tempFace2 = faces[j];
			cv::resize(tempFace2, tempFace2, size, 0, 0, CV_INTER_LINEAR);

			int predictedLabel = -1;
			double confidence = 0.0;
			// model1->predict(testImage, predictedLabel, confidence);
			predictedLabel = model1->predict(tempFace2);
			__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG",
					"Predicted Label=%d\n", predictedLabel);
			if (predictedLabel == subjectLabel) {
				results.push_back(i);
				faces.clear();
				__android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG",
						"Match found...\n");
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
