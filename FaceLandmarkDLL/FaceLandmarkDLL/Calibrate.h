#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\calib3d.hpp>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <stdio.h>




/** @class Calibrate
*  @brief Class to calibrate image to measure distance in mm
*/
class Calibrate
{

private:
	///Image to process
	cv::Mat image;
	///Phisical distance between squares on marker
	float calibrationSquareDimension;
	///Dimensions of used marker
	cv::Size boardDimension;
	/// Buffor for detected marker points
	std::vector<cv::Point2f> points;
	/// Calulate points distance
	double CalculatePointsDistance(cv::Point2f a, cv::Point2f b);
	/// Image mm to pixel scale factor
	double imageScaleFactor;

public:
	/// @brief  Constructor
	/// [in] distance between squares on marker in mm
	/// [in] buffor for calibration points
	/// [in] size of the marker
	Calibrate(float squareDimension, std::vector<cv::Point2f> bufPotins, cv::Size boardSize);
	/// @brief Load image from Mat file
	/// [in] Mat object to load
	void LoadImageToCalibration(cv::Mat imageLoaded);
	/// @brief Load image from file to calibrate
	///  [in] name of the file to load wth extention
	void LoadImageToCalibration(std::string fileName);
	///@brief Display image on the screen
	void ShowLoadedImage();
	/// @ Detect marker on loaded image
	/// [in] name of the file to save on disk
	/// [out] flag if marker was found on image
	bool FindCorrenrsOnMarker(std::string fileNameToSave);
	///@ brief Calculate real distance on image in mm
	double CalculateScaleFactor();
	///@brief Return image after calibration
	cv::Mat GetKalibratedImage();
	///@brief Locate points specyfied by distance from reference point
	///[in] X distance
	///[in] Y dostance
	///[in] Reference point
	cv::Point2f GetFeaturePointByDistance(float xDistance, float yDistance, cv::Point2f refPoint);
	///@brief Get array of calibration points 
	std::vector<cv::Point2f> Calibrate::GetCalibratedPointsOnChessboard();

};