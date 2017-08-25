// FaceLandmarkDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FaceLandmarksDll.h"

namespace FaceLandmarks

{
	//Connections Strings local host
	/*sql::SQLString host = "localhost";
	sql::SQLString root = "root";
	sql::SQLString pass = "gulki1";
	sql::SQLString databaseName = "lista_klientow";*/
	//tested domain

	sql::SQLString host = "gulczy.ayz.pl";
	sql::SQLString root = "gulczy_root";
	sql::SQLString pass = "gulki1";
	sql::SQLString databaseName = "gulczy_listaKlientow";

	/** @class DataBuf
	* @brief Class converting char vector to istream buffor
	*/
	class DataBuf : public streambuf
	{
	public:
		DataBuf(char * d, size_t s)
		{
			setg(d, d, d + s);
		}
	};

	///@brief Round passed value to 2 first digits
	///[in] value to round
	double RoundToTwoDigits(double value)
	{
		return (round(value * 100) / 100);

	}

	///@brief Close all open cv calculation windows
	void CloseAllImageWindows()
	{
		cv::destroyAllWindows();
	}

	///@brief Adjust points coordianates based on resolution scaling
	///[in] resolution scaling
	double AdjustResizeFactor(double resizeFactor)
	{
		double pointScalefactor = 0.0;;

		if (resizeFactor == 0.5)
		{
			pointScalefactor = 2;
		}
		else if (resizeFactor == 0.25)
		{
			pointScalefactor = 4;
		}
		else if (resizeFactor == 0.125)
		{
			pointScalefactor = 8;
		}
		else 
		{
			pointScalefactor = 1.0;
		}

		return pointScalefactor;

	}

	//Obydwie funkcjie dzia³aj¹ TODO: trzeba dodaæ napewno jakieœ komunikaty którê bêd¹ mówiæ o tym co sie kiedy dzieje -> zrobione dlatego funkcje sa bool
	//wszystkie napisy które dostajemy ida do okienka output.

	/// @brief Start calculation of front feature points and sending data to database
	/// [in] customer ID
	/// [in] resolution resize factor
	/// [in] decision on performing additional pupil detection algorythm
	bool CalculateFrontFeaturePoints(int ID, bool resizeImage, double resizeFactor, bool useHoughTransformDetection)
	{
	
		try 
		{
#pragma region  Pobranie zdjêcia z bazy danych

			sql::SQLString userIDstring = std::to_string(ID).c_str();
			SqlConnection * Con = new SqlConnection(host, root, pass, databaseName);
			Con->Connect();
			cv::Mat imageFromDatabase = Con->GetImageFromDatabase(userIDstring);			
			//cv::imshow("Image", imageFromDatabase); //show the image
			//cv::waitKey();
			delete Con;

#pragma endregion


#pragma region  Kalibracja obrazu dla zdjecia o pe³nej rozdzielczosci w bazie
			//Calibration of the image with full original resolution
			float calibrationSquareDimensionMaxResolution = 8;// 8 mm distance for glassed between 2 nearest squares
			const cv::Size boardDomensionsMaxResolution = cv::Size(3, 3);
			std::vector<cv::Point2f> pointBufMax;
			Calibrate calibMaxResolutionImage(calibrationSquareDimensionMaxResolution, pointBufMax, boardDomensionsMaxResolution);
			//calib.LoadImageToCalibration(filename + ".jpg"); //LOADING FROM FILE

			calibMaxResolutionImage.LoadImageToCalibration(imageFromDatabase);    //LOAD IMAGE FROM DATABASE
			//calib.ShowLoadedImage();

			if (calibMaxResolutionImage.FindCorrenrsOnMarker("Kalibracja_.jpg") != true)
			{
				cout << "Nie znaleziono markera na obrazie program sie wylaczy" << endl;
				//cv::waitKey();
				return false;
			}

			double mmMaxScaleFactor = calibMaxResolutionImage.CalculateScaleFactor();

			//Chacging resolution of the image
			if (resizeImage == true)
			{
				cv::resize(imageFromDatabase, imageFromDatabase, cv::Size(0, 0), resizeFactor, resizeFactor, CV_INTER_AREA);
			}


#pragma endregion
			
			
			
#pragma region Kalibracja Obrazu
			
			//Calibration for diffrent resolution of the image
			float calibrationSquareDimension = 8; // 8 mm distance for glassed between 2 nearest squares
			const cv::Size boardDomensions = cv::Size(3, 3);
			std::vector<cv::Point2f> pointBuf;

			Calibrate calib(calibrationSquareDimension, pointBuf, boardDomensions);
			//calib.LoadImageToCalibration(filename + ".jpg"); //LOADING FROM FILE
			calib.LoadImageToCalibration(imageFromDatabase);    //LOAD IMAGE FROM DATABASE
			//calib.ShowLoadedImage();
			if (calib.FindCorrenrsOnMarker("Kalibracja_.jpg") != true)
				{
					cout << "Nie znaleziono markera na obrazie program sie wylaczy" << endl;					
					return false;
				}

			double mmScaleFactor = calib.CalculateScaleFactor();	
			
#pragma endregion

#pragma region Feature Detection

			// DETECT LANDMARKS -----------
			//FaceLandmark detectFace("Kalibracja_" + filename);
			FaceLandmark detectFace(calib.GetKalibratedImage());	
			auto isDetected = detectFace.detect_face_and_features();
			//Face not detected
			if(isDetected == false)
			{
				return false;
			}
					
			//Calculate pupil coordinates
			const dlib::point leftEye = detectFace.get_lefteye_point();
			const dlib::point rightEye = detectFace.get_righteye_point();
			// Calculate distances
			double eyeDistance = length(leftEye - rightEye);
			double cheekToCheekDistance = detectFace.calculate_face_width();
			double templeDistance = detectFace.calculate_temple_distance();
			std::vector<double> noseToEyesDistances = detectFace.calculate_eye_nose_distance();

			//Calculating points
			dlib::point rightCheek = detectFace.extract_specified_point_from_detected_landmarks(0);
			dlib::point leftCheek = detectFace.extract_specified_point_from_detected_landmarks(16);
			dlib::point rightTemple = detectFace.extract_specified_point_from_detected_landmarks(17);
			dlib::point leftTemple = detectFace.extract_specified_point_from_detected_landmarks(26);
			dlib::point middleNose = detectFace.extract_specified_point_from_detected_landmarks(27);

			//Alternative hough transform algorythm
			bool houghTransformSucces = false;
			Point2D houghtRightEye(0, 0);
			Point2D houghLeftEye(0, 0);

			if (useHoughTransformDetection == true)
			{
			
				cv::Point2d rightCheekPoint(detectFace.extract_specified_point_from_detected_landmarks(1).x(), detectFace.extract_specified_point_from_detected_landmarks(1).y());
				cv::Point2d leftTemplePoint(leftTemple.x(), leftTemple.y());
				PupilDetector detectEyePupils;

				detectEyePupils.GetImageToProcess(calib.GetKalibratedImage());
				detectEyePupils.GetCropPoints(rightCheekPoint, leftTemplePoint);
				houghTransformSucces = detectEyePupils.LookForPupilPoints();

				if (houghTransformSucces == true)
				{
					houghtRightEye.X = detectEyePupils.GetrightEyePoint().X;
					houghtRightEye.Y = detectEyePupils.GetrightEyePoint().Y;

					houghLeftEye.X = detectEyePupils.GetleftEyePoint().X;
					houghLeftEye.Y = detectEyePupils.GetleftEyePoint().Y;
				}


			}

			//Drawing lines on result image (optional)
			detectFace.DrawLineOnImage(leftEye, rightEye, dlib::rgb_pixel(255, 0, 0));	
			detectFace.DrawLineOnImage(rightCheek, leftCheek, dlib::rgb_pixel(0, 255, 0));
			detectFace.DrawLineOnImage(leftTemple, rightTemple, dlib::rgb_pixel(0, 0, 255));
			detectFace.DrawLineOnImage(middleNose, rightEye, dlib::rgb_pixel(255, 0, 255));
			detectFace.DrawLineOnImage(middleNose, leftEye, dlib::rgb_pixel(204, 153, 0));

			detectFace.save_processed_file("Result");

			
			//Odczytywanie wspolrzednych punktow			
			Point2D leftEyeP(leftEye.x() *AdjustResizeFactor(resizeFactor), leftEye.y()*AdjustResizeFactor(resizeFactor));
			Point2D rightEyeP(rightEye.x()*AdjustResizeFactor(resizeFactor), rightEye.y()*AdjustResizeFactor(resizeFactor));

			if (houghTransformSucces == true)
			{
				leftEyeP.X = houghLeftEye.X*AdjustResizeFactor(resizeFactor);
				leftEyeP.Y = houghLeftEye.Y*AdjustResizeFactor(resizeFactor);

				rightEyeP.X = houghtRightEye.X*AdjustResizeFactor(resizeFactor);
				rightEyeP.Y = houghtRightEye.Y*AdjustResizeFactor(resizeFactor);

			}

			Point2D rightCheekP(rightCheek.x()*AdjustResizeFactor(resizeFactor), rightCheek.y()*AdjustResizeFactor(resizeFactor));
			Point2D leftCheekP(leftCheek.x()*AdjustResizeFactor(resizeFactor), leftCheek.y()*AdjustResizeFactor(resizeFactor));
			
			Point2D rightTempleP(rightTemple.x()*AdjustResizeFactor(resizeFactor), rightTemple.y()*AdjustResizeFactor(resizeFactor));
			Point2D leftTempleP(leftTemple.x()*AdjustResizeFactor(resizeFactor), leftTemple.y()*AdjustResizeFactor(resizeFactor));
			
			Point2D noseP(middleNose.x()*AdjustResizeFactor(resizeFactor), middleNose.y()*AdjustResizeFactor(resizeFactor));



#pragma region Przygotowanie zdjêcia do wys³ania do bazy

			//Load calculated image
			cv::Mat imageToDatabase;
			cv::Mat processedImage = dlib::toMat(detectFace.processedImage);
			cv::cvtColor(processedImage, imageToDatabase, CV_BGR2RGB);

			//Wyswietlanie wynikowego zdjecia
			//cv::namedWindow("Wyniki", cv::WINDOW_NORMAL);
			//cv::imshow("Wyniki", imageToDatabase); //show the image
			//cv::waitKey(0);

		
			std::vector<unsigned char> dataImageBuffer;
			// KODOWANIE OBRAZU DO BUFFORA
			cv::imencode(".jpg", imageToDatabase, dataImageBuffer);
			// DZIALAJACA KONVERSA
			DataBuf buffer((char*)(&dataImageBuffer[0]), dataImageBuffer.size());
			std::istream image(&buffer);
			std::istream *pimage;
			pimage = &image;

#pragma endregion
#pragma endregion

#pragma region Wysy³anie danych do bazy
			
			SqlConnection *sendData = new SqlConnection(host, root, pass, databaseName);
			sendData->Connect();
			sendData->CustomerUpdateStatement
			(
				RoundToTwoDigits(eyeDistance*mmScaleFactor),
				RoundToTwoDigits(cheekToCheekDistance*mmScaleFactor),
				RoundToTwoDigits(templeDistance*mmScaleFactor),
				RoundToTwoDigits(noseToEyesDistances[0] * mmScaleFactor),
				RoundToTwoDigits(noseToEyesDistances[1] * mmScaleFactor),							
				ID // client ID
			);

			int currentIdCount = sendData->GetRowCount("Punkty");
			// Insert to coordinates table
			//If rekord withid klienta exists
			bool recordExists = sendData->CheckIfKlientExists(ID, "Punkty");

			if (recordExists == false)
			{
				sendData->CoordinatesInsertStatement
				(
					currentIdCount + 1,
					leftTempleP,
					rightTempleP,
					noseP,
					leftEyeP,
					rightEyeP,
					leftCheekP,
					rightCheekP,
					mmScaleFactor,
					mmMaxScaleFactor,
					ID		//id klienta
				);
			}
			
			else 
			{
				sendData->CoordinatesUpdateStatement
				(					
					leftTempleP,
					rightTempleP,
					noseP,
					leftEyeP,
					rightEyeP,
					leftCheekP,
					rightCheekP,
					mmScaleFactor,
					mmMaxScaleFactor,
					ID		//id klienta
				);
			}

			delete sendData;
			// Calculation succesful
			return true;

#pragma endregion


		}
		catch (exception& ex)
		{
			cout << "\nexception thrown!" << endl;
			cout << ex.what() << endl;
			return false;
			cin.get();
		}

	}

	/// @brief Start calculation of profile feature points and sending data to database
	/// [in] customer ID
	bool CalculateProfileFeaturePoints(int ID)
	{
		try 
		{

			sql::SQLString userIDstring = std::to_string(ID).c_str();

#pragma region Przetwarzanie zdjecia z profilu
			//Przetwarzanie zdjecia z profilu			
				SqlConnection *ConProfile = new SqlConnection(host,root,pass,databaseName);
				ConProfile->Connect();
				cv::Mat profileImageFromDatabase = ConProfile->GetProfileImageFromDatabase(userIDstring);//, "Klienci");

						//cv::imshow("Profile Image", profileImageFromDatabase); //show the image
						//cv::waitKey();
				//Zakonczenie polaczenia
				delete ConProfile;

				//Image Calibration ------------------------------------------------------- mozna wykorzystac funkcje cv_image to zmiany Mat na dlibowy format obrazu
				float calibrationSquareDimension = 8.0f; //mm 14,5 dla tego standardowego 
				const cv::Size boardDomensions = cv::Size(3, 3);
				std::vector<cv::Point2f> pointBuffor;
				

				Calibrate profileCalib(calibrationSquareDimension, pointBuffor, boardDomensions);
				//calib.LoadImageToCalibration(filename + ".jpg"); //LOADING FROM FILE
				profileCalib.LoadImageToCalibration(profileImageFromDatabase);    //LOAD IMAGE FROM DATABASE
						//profileCalib.ShowLoadedImage();
				//cv::waitKey();
				if (profileCalib.FindCorrenrsOnMarker("KalibracjaProfilowe_.jpg") != true)
				{
					cout << "Nie znaleziono markera na obrazie." << endl;
					//cv::waitKey();

					return false;
				}
				double mmProfileScaleFactor = profileCalib.CalculateScaleFactor();
				
				//-- end Profile Image Calibration -------------------------------------------------

				//Lokalizacja punktu ucha i nosa wykorzystuj¹c skalibrowan¹ szachwonicê i znany rozmiar okularów
				//TODO: Odleg³oœci wpisywane dla okularów mark8 bez kleju beda rózne w zale¿noœci od okularow
				std::vector<cv::Point2f> calibratedPoints = profileCalib.GetCalibratedPointsOnChessboard();
				//TODO: lepiej rozwi¹zaæ te magic numbers

				//Dodatnia wartosc na lewo od szachownicy ujemna na prawo
				auto earPoint = profileCalib.GetFeaturePointByDistance(61, 20, calibratedPoints[4]);
				auto nosePoint = profileCalib.GetFeaturePointByDistance(-46, 8, calibratedPoints[4]);
				auto eyePoint = profileCalib.GetFeaturePointByDistance(-33, 18, calibratedPoints[4]);
				
				//Wyslanie informacji do bazy
				SqlConnection *sendData = new SqlConnection(host, root, pass, databaseName);
				sendData->Connect();
				//Sprawdzenie czy dany rekord jest ju¿ w tabeli
				bool recordExists = sendData->CheckIfKlientExists(ID, "Punkty_Profil");
				//jezeli nie ma to dodadajemy do tabeli
				if (recordExists == false)
				{
					int currentIdCount = sendData->GetRowCount("Punkty_Profil");
					// Insert to coordinates table
					sendData->CoordinatesInsertStatement //Dodatnie wspolczynika skalowania do tabeli 
					(
						currentIdCount + 1,
						Point2D(earPoint.x, earPoint.y), 
						Point2D(nosePoint.x, nosePoint.y),
						Point2D(eyePoint.x, eyePoint.y),
						mmProfileScaleFactor,
						RoundToTwoDigits(cv::norm(earPoint - nosePoint)*mmProfileScaleFactor),
						RoundToTwoDigits(cv::norm(eyePoint - nosePoint)*mmProfileScaleFactor),
						ID		//id klienta
					);
					
				}
				else
				{
					sendData->CoordinatesUpdateStatement //Dodatnie wspolczynika skalowania do tabeli 
					(
						
						Point2D(earPoint.x, earPoint.y),
						Point2D(nosePoint.x, nosePoint.y),
						Point2D(eyePoint.x, eyePoint.y),
						mmProfileScaleFactor,
						RoundToTwoDigits(cv::norm(earPoint - nosePoint)*mmProfileScaleFactor),
						RoundToTwoDigits(cv::norm(eyePoint - nosePoint)*mmProfileScaleFactor),
						ID		//id klienta
					);

				}

				//Update odleg³oœci w glownej tabeli
				sendData->CustomerUpdateStatement
				(
					RoundToTwoDigits(cv::norm(earPoint - nosePoint)*mmProfileScaleFactor),
					RoundToTwoDigits(cv::norm(eyePoint - nosePoint)*mmProfileScaleFactor),
					ID
				);



				delete sendData;

				return true;
		}
		catch (exception& ex)
		{
			cout << "\nexception thrown!" << endl;
			cout << ex.what() << endl;
			cin.get();
			return false;
		}
#pragma endregion

	}
	
}