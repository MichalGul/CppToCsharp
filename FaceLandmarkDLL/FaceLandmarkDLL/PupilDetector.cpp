#include "PupilDetector.h"



PupilDetector::PupilDetector()
{
}


PupilDetector::~PupilDetector()
{
}

void PupilDetector::GetCropPoints(cv::Point2d firstRectanglePoint, cv::Point2d secondRectanglePoint)
{
	ROIFirstPoint = firstRectanglePoint;
	ROISecondPoint = secondRectanglePoint;

	cropImageToEyes();
}

void PupilDetector::GetImageToProcess(cv::Mat image)
{
	imageToProcess = image;
}

bool PupilDetector::LookForPupilPoints()
{
	int cannyThresholdInitialValue = 100;
	int accumulatorThresholdInitialValue = 50;
	int maxAccumulatorThreshold = 100;
	int maxCannyThreshold = 155;

	const std::string cannyThresholdTrackbarName = "Krawedzie";
	const std::string accumulatorThresholdTrackbarName = "Akumulator";
	int cannyThreshold = cannyThresholdInitialValue;
	int accumulatorThreshold = accumulatorThresholdInitialValue;

	// create the main window, and attach the trackbars
	cv::namedWindow(windowName, cv::WINDOW_NORMAL);
	
	cv::createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold, maxCannyThreshold);
	cv::createTrackbar(accumulatorThresholdTrackbarName, windowName, &accumulatorThreshold, maxAccumulatorThreshold);


	std::vector<cv::Vec3f> circles;
	// infinite loop to display
	// and refresh the content of the output image
	// until the user presses q or Q
	char key = 0;
	while (key != 'q' && key != 'Q')
	{
		// those paramaters cannot be =0
		// so we must check here
		cannyThreshold = std::max(cannyThreshold, 1);
		accumulatorThreshold = std::max(accumulatorThreshold, 1);

		//runs the detection, and update the display
		circles = HoughDetection(processedImage, maskedImage, cannyThreshold, accumulatorThreshold);

		// get user key
		key = (char)cv::waitKey(10);
	}

	int numberOfCirclesOnEyesImage = circles.size();

	if (numberOfCirclesOnEyesImage == 2)
	{
		rightEyePoint.X = cvRound(circles[0][0]);
		rightEyePoint.Y = cvRound(circles[0][1]);

		leftEyePoint.X = cvRound(circles[1][0]);
		leftEyePoint.Y = cvRound(circles[1][1]);

		return true; // jezeli zwracana jest prawda to mamy dwa punkty - niekoniecznie dobre i je bedziemy pobierac z funkcji
	}
	else
	{
		rightEyePoint.X = 0;
		rightEyePoint.Y = 0;

		leftEyePoint.X = 0;
		leftEyePoint.Y = 0;

		return false;
	}
}

Point2D PupilDetector::GetrightEyePoint()
{
	return rightEyePoint;
}

Point2D PupilDetector::GetleftEyePoint()
{
	return leftEyePoint;
}

void PupilDetector::cropImageToEyes()
{
	//Stworzenie regionu wokol oczu
	cv::Rect ROI(ROIFirstPoint, ROISecondPoint);

	pupilMask = cv::Mat::zeros(imageToProcess.size(), imageToProcess.type());
	maskedImage = cv::Mat::zeros(imageToProcess.size(), imageToProcess.type());
	processedImage = cv::Mat::zeros(imageToProcess.size(), imageToProcess.type());
	//Wyciecie maski z obrazu
	cv::rectangle(pupilMask, ROI, cv::Scalar(255, 255, 255), -1, 8, 0);
	imageToProcess.copyTo(maskedImage, pupilMask);
	//Filtr Gaussa do poprawienia wykrywania okregow
	GaussianBlur(maskedImage, maskedImage, cv::Size(3, 3), 2, 2);

	processedImage = maskedImage;
	//Zmiania na szaroodcieniowy obraz
	cv::cvtColor(processedImage, processedImage, CV_BGR2GRAY);

	//cv::namedWindow("MASKA1", cv::WINDOW_NORMAL);
	//cv::imshow("MASKA1", processedImage);
	//cv::waitKey();

}

std::vector<cv::Vec3f> PupilDetector::HoughDetection(const cv::Mat & src_gray, const cv::Mat & src_display, int cannyThreshold, int accumulatorThreshold)
{
	// will hold the results of the detection
	std::vector<cv::Vec3f> circles;
	// runs the actual detection
	HoughCircles(src_gray, circles, cv::HOUGH_GRADIENT, 1, src_gray.rows / 8, cannyThreshold, accumulatorThreshold, 0, 200);

	// clone the colour, input image for displaying purposes
	cv::Mat display = src_display.clone();
	for (size_t i = 0; i < circles.size(); i++)
	{
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(display, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(display, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);
	}

	// shows the results
	//cv::namedWindow(windowName, cv::WINDOW_NORMAL);
	imshow(windowName, display);

	return circles;
}
