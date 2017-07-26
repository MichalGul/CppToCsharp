// FaceLandmarkDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FaceLandmarksDll.h"

namespace FaceLandmarks

{
	//Connections Strings
	sql::SQLString host = "localhost";
	sql::SQLString root = "root";
	sql::SQLString pass = "gulki1";
	sql::SQLString databaseName = "lista_klientow";

	//Pomocniczna klasa do konwersji vectora charow do istream 
	class DataBuf : public streambuf
	{
	public:
		DataBuf(char * d, size_t s)
		{
			setg(d, d, d + s);
		}
	};

	double RoundToTwoDigits(double value)
	{
		return (round(value * 100) / 100);

	}

	double Add(double a, double b)
	{
		return a + b;
	}
	double Subtract(double a, double b)
	{
		return a - b;
	}
	double Multiply(double a, double b)
	{
		return a*b;
	}
	double Divide(double a, double b)
	{
		if (b == 0)
			throw invalid_argument("b cannot be zero!");
		return a / b;
	}
	void CloseAllImageWindows()
	{
		cv::destroyAllWindows();
	}
	//Funkcja do skalowania wsp�rz�dnych punkt�w zaznaczanych na obrazie w zale�no�ci od zmniejszenie rozdzielczo�ci tego obrazu przy obliczaniu 
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

	//Obydwie funkcjie dzia�aj� TODO: trzeba doda� napewno jakie� komunikaty kt�r� b�d� m�wi� o tym co sie kiedy dzieje
	//wszystkie napisy kt�re dostajemy ida do okienka output.
	bool CalculateFrontFeaturePoints(int ID, bool resizeImage, double resizeFactor)
	{
		//Kod funkcji main z porgramu FaceLandmark
		try 
		{
#pragma region  Pobranie zdj�cia z bazy danych
			//konwersja int przez string  na SQL string
			sql::SQLString userIDstring = std::to_string(ID).c_str();

			//Pobranie zdj�cia z bazy danych
			SqlConnection * Con = new SqlConnection(host, root, pass, databaseName);
			Con->Connect();
			cv::Mat imageFromDatabase = Con->GetImageFromDatabase(userIDstring);//, "Klienci");
			
					//cv::imshow("Image", imageFromDatabase); //show the image
					//cv::waitKey();
			//Zakonczenie polaczenia
			delete Con;

#pragma endregion
#pragma region  Kalibracja obrazu dla zdjecia o pe�nej rozdzielczosci w bazie
			//Kalibracja obrazu dla zdjecia o pe�nej rozdzielczosci kt�re b�dzie siedzia�o w bazie
			//Kalibracja obrazu
			float calibrationSquareDimensionMaxResolution = 8; //mm 14,5 dla tego standardowego 8 dla tego dla okulaorw
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
#pragma endregion
			
			// W tym miejschu ewentualnie zmniejszenie rozdzielczosci zdjecia o 2 lub 4 z wykorzystaniem OPEN CV
			if (resizeImage == true)
			{
				cv::resize(imageFromDatabase, imageFromDatabase, cv::Size(0, 0), resizeFactor, resizeFactor, CV_INTER_AREA);
			}

#pragma region Kalibracja Obrazu
			
			//Kalibracja obrazu
			float calibrationSquareDimension = 8; //mm 14,5 dla tego standardowego 8 dla tego dla okulaorw
			const cv::Size boardDomensions = cv::Size(3, 3);
			std::vector<cv::Point2f> pointBuf;

			Calibrate calib(calibrationSquareDimension, pointBuf, boardDomensions);
			//calib.LoadImageToCalibration(filename + ".jpg"); //LOADING FROM FILE
			calib.LoadImageToCalibration(imageFromDatabase);    //LOAD IMAGE FROM DATABASE
					//calib.ShowLoadedImage();
			if (calib.FindCorrenrsOnMarker("Kalibracja_.jpg") != true)
				{
					cout << "Nie znaleziono markera na obrazie program sie wylaczy" << endl;
					//cv::waitKey();

					return false;
				}
			double mmScaleFactor = calib.CalculateScaleFactor();	
			
#pragma endregion

#pragma region Feature Detection

			// DETECT LANDMARKS -------------------------------------------------------------------------------------------
			//FaceLandmark detectFace("Kalibracja_" + filename);
			FaceLandmark detectFace(calib.GetKalibratedImage());	
			auto isDetected = detectFace.detect_face_and_features();
			//Jezeli nie wykryto twarzy
			if(isDetected == false)
			{
				return false;
			}
			
			//Show detected faces on window
			//detectFace.show_detected_faces();
					//detectFace.show_face_chips();
			//Wyliczenie pozycji �renic
			const dlib::point leftEye = detectFace.get_lefteye_point();
			const dlib::point rightEye = detectFace.get_righteye_point();
			// Wyliczanie odleg�osci
			double eyeDistance = length(leftEye - rightEye);
			double cheekToCheekDistance = detectFace.calculate_face_width();
			double templeDistance = detectFace.calculate_temple_distance();
			std::vector<double> noseToEyesDistances = detectFace.calculate_eye_nose_distance();

			//Wyznaczanie punktow
			dlib::point rightCheek = detectFace.extract_specified_point_from_detected_landmarks(0);
			dlib::point leftCheek = detectFace.extract_specified_point_from_detected_landmarks(16);
			dlib::point rightTemple = detectFace.extract_specified_point_from_detected_landmarks(17);
			dlib::point leftTemple = detectFace.extract_specified_point_from_detected_landmarks(26);
			dlib::point middleNose = detectFace.extract_specified_point_from_detected_landmarks(27);

			//TODO: Wyszukiwanie okr�g�w w �renicy w obr�bie maski wyznaczonej przez punkty z dliba w celu lepszego wyszukiwania �rodka oka


			//Rysowanie odleglosci na obrazie
			detectFace.DrawLineOnImage(leftEye, rightEye, dlib::rgb_pixel(255, 0, 0));	
			detectFace.DrawLineOnImage(rightCheek, leftCheek, dlib::rgb_pixel(0, 255, 0));
			detectFace.DrawLineOnImage(leftTemple, rightTemple, dlib::rgb_pixel(0, 0, 255));
			detectFace.DrawLineOnImage(middleNose, rightEye, dlib::rgb_pixel(255, 0, 255));
			detectFace.DrawLineOnImage(middleNose, leftEye, dlib::rgb_pixel(204, 153, 0));

			detectFace.save_processed_file("Result");


			//Odczytywanie wspolrzednych punktow			
			Point2D leftEyeP(leftEye.x() *AdjustResizeFactor(resizeFactor), leftEye.y()*AdjustResizeFactor(resizeFactor));
			Point2D rightEyeP(rightEye.x()*AdjustResizeFactor(resizeFactor), rightEye.y()*AdjustResizeFactor(resizeFactor));
			
			Point2D rightCheekP(rightCheek.x()*AdjustResizeFactor(resizeFactor), rightCheek.y()*AdjustResizeFactor(resizeFactor));
			Point2D leftCheekP(leftCheek.x()*AdjustResizeFactor(resizeFactor), leftCheek.y()*AdjustResizeFactor(resizeFactor));
			
			Point2D rightTempleP(rightTemple.x()*AdjustResizeFactor(resizeFactor), rightTemple.y()*AdjustResizeFactor(resizeFactor));
			Point2D leftTempleP(leftTemple.x()*AdjustResizeFactor(resizeFactor), leftTemple.y()*AdjustResizeFactor(resizeFactor));
			
			Point2D noseP(middleNose.x()*AdjustResizeFactor(resizeFactor), middleNose.y()*AdjustResizeFactor(resizeFactor));



#pragma region Przygotowanie zdj�cia do wys�ania do bazy

			//Wczytanie zdjecia zapisanego przez FaceLandmark 
			cv::Mat imageToDatabase;
			cv::Mat processedImage = dlib::toMat(detectFace.processedImage);
			cv::cvtColor(processedImage, imageToDatabase, CV_BGR2RGB);
			//Wyswietlanie wynikowego zdjecia
			//cv::namedWindow("Wyniki", cv::WINDOW_NORMAL);
			//cv::imshow("Wyniki", imageToDatabase); //show the image
			//cv::waitKey(0);

			

			// Kodowanie zdjecia do wyslania do bazy danych -> Tylko do test�w
			// w ostatecznej wersji bez wysy�ania zdj�cia tylko punkty

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

#pragma region Wysy�anie danych do bazy
			
			SqlConnection *sendData = new SqlConnection(host, root, pass, databaseName);
			sendData->Connect();
			sendData->CustomerUpdateStatement
			(
				RoundToTwoDigits(eyeDistance*mmScaleFactor),
				RoundToTwoDigits(cheekToCheekDistance*mmScaleFactor),
				RoundToTwoDigits(templeDistance*mmScaleFactor),
				RoundToTwoDigits(noseToEyesDistances[0] * mmScaleFactor),
				RoundToTwoDigits(noseToEyesDistances[1] * mmScaleFactor),
							//pimage, //TODO: Ostatecznie nie bedzie wysylc przetworzonego zdjecia do bazy
				ID // id klienta
			);

			int currentIdCount = sendData->GetRowCount("Punkty");
			// Insert to coordinates table
			//Sprawdzenie czy rekord o danym id klienta juz istnieje
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
			//TODO: jak rekord jest juz w tabeli to trzeba wykona� polecenie UPDATE po ponownym policzeniu
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
			// zwraca prawde jezeli obliczenia zakonczyly sie powodzeniem
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

				//Lokalizacja punktu ucha i nosa wykorzystuj�c skalibrowan� szachwonic� i znany rozmiar okular�w
				//TODO: Odleg�o�ci wpisywane dla okular�w mark8 bez kleju beda r�zne w zale�no�ci od okularow
				std::vector<cv::Point2f> calibratedPoints = profileCalib.GetCalibratedPointsOnChessboard();
				//TODO: lepiej rozwi�za� te magic numbers

				//Dodatnia wartosc na lewo od szachownicy ujemna na prawo
				auto earPoint = profileCalib.GetFeaturePointByDistance(61, 20, calibratedPoints[4]);
				auto nosePoint = profileCalib.GetFeaturePointByDistance(-46, 8, calibratedPoints[4]);
				auto eyePoint = profileCalib.GetFeaturePointByDistance(-33, 18, calibratedPoints[4]);
				
				//Wyslanie informacji do bazy
				SqlConnection *sendData = new SqlConnection(host, root, pass, databaseName);
				sendData->Connect();
				//Sprawdzenie czy dany rekord jest ju� w tabeli
				bool recordExists = sendData->CheckIfKlientExists(ID, "Punkty_profil");
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

				//Update odleg�o�ci w glownej tabeli
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