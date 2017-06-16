#include "Calibrate.h"


double Calibrate::CalculatePointsDistance(cv::Point2f a, cv::Point2f b)
{
	return cv::norm(b - a);
}

Calibrate::Calibrate(float squareDimension, std::vector<cv::Point2f> bufPotins, cv::Size boardSize)
{
	calibrationSquareDimension = squareDimension;
	points = bufPotins;
	boardDimension = boardSize;
}
///Loading image form mat object
void Calibrate::LoadImageToCalibration(cv::Mat imageLoaded)
{
	image = imageLoaded;

}
///Loading image froma file
void Calibrate::LoadImageToCalibration(std::string fileName)
{
	image = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);

}
void Calibrate::ShowLoadedImage()
{
	cv::imshow("Loaded Image", image);
}
///Locating Chessboard marker on image writiing and displaying it
bool Calibrate::FindCorrenrsOnMarker(std::string fileNameToSave)
{

	bool found = cv::findChessboardCorners(image, boardDimension, points, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
	if (found && points.size() <= 9)
	{
		//drowing points on image
		cv::drawChessboardCorners(image, boardDimension, points, found);		
		cv::namedWindow("Looking for Corners", cv::WINDOW_NORMAL);
		imshow("Looking for Corners", image);
		imwrite(fileNameToSave, image);
		//cv::waitKey();

	}
	else
	{
		std::cout << "Nie znaleziono modelu" << std::endl;

	}
	//if chessboard was found on image
	return found;
}

double Calibrate::CalculateScaleFactor()
{
	std::vector<double> distances;

	distances.push_back(CalculatePointsDistance(points[1], points[0]));
	distances.push_back(CalculatePointsDistance(points[3], points[0]));
	distances.push_back(CalculatePointsDistance(points[2], points[1]));
	distances.push_back(CalculatePointsDistance(points[4], points[1]));
	distances.push_back(CalculatePointsDistance(points[5], points[2]));
	distances.push_back(CalculatePointsDistance(points[5], points[4]));
	distances.push_back(CalculatePointsDistance(points[5], points[8]));
	distances.push_back(CalculatePointsDistance(points[4], points[3]));
	distances.push_back(CalculatePointsDistance(points[6], points[3]));
	distances.push_back(CalculatePointsDistance(points[8], points[7]));
	distances.push_back(CalculatePointsDistance(points[7], points[4]));
	distances.push_back(CalculatePointsDistance(points[7], points[6]));

	double distanceSum = 0;

	for (auto iter = distances.begin(); iter != distances.end(); iter++)
	{
		distanceSum = distanceSum + *iter;
	}
	double averagePixelDistanceOnMarker = distanceSum / distances.size();


	return calibrationSquareDimension / averagePixelDistanceOnMarker;
}

cv::Mat Calibrate::GetKalibratedImage()
{
	return image;
}
