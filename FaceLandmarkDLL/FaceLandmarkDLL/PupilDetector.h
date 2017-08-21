#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\calib3d.hpp>

#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <mysql.h>

#include "Calibrate.h"
#include"FaceLandmark.h"
#include"SqlConnection.h"
#include"Point2D.h"


/** @class FaceLandmark
* @brief Class detecting eye pupils on front image
*/
class PupilDetector
{
public:

	PupilDetector();
	~PupilDetector();

	/// @brief  Set coordinates of region of interest on image
	///[in] first point of ROI rectangle
	///[in] second point of ROI rectangle
	void GetCropPoints(cv::Point2d firstRectanglePoint, cv::Point2d secondRectanglePoint);
	/// @brief Get image to locate pupils
	///[in] CV image from calibration
	void GetImageToProcess(cv::Mat image);
	/// @brief  Locate pupils on face image
	bool LookForPupilPoints();
	///@brief Get located right eye point
	Point2D GetrightEyePoint();
	///@brief Get located left eye point
	Point2D GetleftEyePoint();

private:

	const std::string windowName = "Wykywanie èrenic";

	/// Image to performs calculations
	cv::Mat imageToProcess;
	cv::Mat pupilMask;
	cv::Mat maskedImage;
	cv::Mat processedImage;

	/// Rectangle points of region on interest
	cv::Point2d ROIFirstPoint;
	cv::Point2d ROISecondPoint;
	/// Calculated pupil points
	Point2D rightEyePoint;
	Point2D leftEyePoint;

	///@brief crop image to region of interest
	void cropImageToEyes();

	///@brief Used OpenCV Hough Transform to detect circles on pupils
	///[in] @param grayscale image to perform
	///[in] image to display
	///[in] canny filter threshold value
	///[in] circle accumulator threshold
	std::vector<cv::Vec3f> HoughDetection(const cv::Mat& src_gray, const cv::Mat& src_display, int cannyThreshold, int accumulatorThreshold);


};

