#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\calib3d.hpp>

#include <dlib/opencv/cv_image.h>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>



#include <dlib/image_io.h>
#include <iostream>
#include <stdio.h>

using namespace dlib;

/** @class FaceLandmark
* @brief Class detecting only one face and its features on image
*/
class FaceLandmark
{
private:


	/// We need a face detector.  We will use this to get bounding boxes for each face in an image.
	dlib::frontal_face_detector faceDetector;
	///This is the tool that will predict face landmark positions given an image and face bounding box.
	dlib::shape_predictor shapePredictor;
	///Container for the detected face
	std::vector<full_object_detection> faces;
	
public:
	///Processed image
	dlib::array2d<dlib::rgb_pixel> processedImage;
	/// Tymczasowe rozwiazanie TBD funkcja ktora bedzie rysowala po obrazku
	void DrawLineOnImage(dlib::point firstPoint, dlib::point secondPoint, dlib::rgb_pixel color);	
	/// @brief  Constructor - pass image file name to process
	///[in] name of the file to process
	FaceLandmark(std::string imageName);
	/// @brief Constructor - pass openCV mat object
	///[in] OpenCv mat to process
	FaceLandmark(cv::Mat imageMat);
	~FaceLandmark();
	/// @brief Methood to detect faces on image
	bool detect_face_and_features();
	///  @brief Saving the file
	/// [in] name of the file to save
	void save_processed_file(std::string fileName);
	/// @brief method for extracting specified points on detected landmarks
	/// [in] number og the extracted point
	dlib::point extract_specified_point_from_detected_landmarks(int pointNumber);
	///@bried method for extracting eye point to detect pupil
	std::vector <dlib::point> GetLeftEyePoints();
	///@bried method for extracting rigjh eye point to detect pupil
	std::vector <dlib::point> GetRightEyePoints();

	//TODO: zrobic funkcje getdlibimage ktora po prostu bedzie wczytywac obraz do pola image

	///Calculations
	dlib::point get_righteye_point();
	dlib::point get_lefteye_point();
	double calculate_face_width();
	double calculate_temple_distance();
	std::vector<double> calculate_eye_nose_distance();


};

