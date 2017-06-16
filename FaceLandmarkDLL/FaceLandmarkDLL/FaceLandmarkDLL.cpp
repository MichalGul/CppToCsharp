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
	//Obydwie funkcjie dzia³aj¹ TODO: trzeba dodaæ napewno jakieœ komunikaty którê bêd¹ mówiæ o tym co sie kiedy dzieje
	//wszystkie napisy które dostajemy ida do okienka output.
	void CalculateFrontFeaturePoints(int ID)
	{
		//Kod funkcji main z porgramu FaceLandmark
		try 
		{
#pragma region  Pobranie zdjêcia z bazy danych
			//konwersja int przez string  na SQL string
			sql::SQLString userIDstring = std::to_string(ID).c_str();

			//Pobranie zdjêcia z bazy danych
			SqlConnection * Con = new SqlConnection(host, root, pass, databaseName);
			Con->Connect();
			cv::Mat imageFromDatabase = Con->GetImageFromDatabase(userIDstring);//, "Klienci");
					//cv::imshow("Image", imageFromDatabase); //show the image
					//cv::waitKey();
			//Zakonczenie polaczenia
			delete Con;

#pragma endregion

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
					cv::waitKey();

					return;
				}
			double mmScaleFactor = calib.CalculateScaleFactor();	
			
#pragma endregion

#pragma region Feature Detection

			// DETECT LANDMARKS -------------------------------------------------------------------------------------------
			//FaceLandmark detectFace("Kalibracja_" + filename);
			FaceLandmark detectFace(calib.GetKalibratedImage());			
			detectFace.detect_face_and_features();
			//Show detected faces on window
			detectFace.show_detected_faces();
					//detectFace.show_face_chips();
			//Wyliczenie pozycji Ÿrenic
			const dlib::point leftEye = detectFace.get_lefteye_point();
			const dlib::point rightEye = detectFace.get_righteye_point();
			// Wyliczanie odleg³osci
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

			//Rysowanie odleglosci na obrazie
			draw_line(detectFace.image, leftEye, rightEye, dlib::rgb_pixel(255, 0, 0));
			draw_line(detectFace.image, rightCheek, leftCheek, dlib::rgb_pixel(0, 255, 0));
			draw_line(detectFace.image, leftTemple, rightTemple, dlib::rgb_pixel(0, 0, 255));
			draw_line(detectFace.image, middleNose, rightEye, dlib::rgb_pixel(255, 0, 255));
			draw_line(detectFace.image, middleNose, leftEye, dlib::rgb_pixel(204, 153, 0));

			detectFace.save_processed_file("Result");


			//Odczytywanie wspolrzednych punktow			
			Point2D leftEyeP(leftEye.x(), leftEye.y());
			Point2D rightEyeP(rightEye.x(), rightEye.y());
			
			Point2D rightCheekP(rightCheek.x(), rightCheek.y());
			Point2D leftCheekP(leftCheek.x(), leftCheek.y());
			
			Point2D rightTempleP(rightTemple.x(), rightTemple.y());
			Point2D leftTempleP(leftTemple.x(), leftTemple.y());
			
			Point2D noseP(middleNose.x(), middleNose.y());


			//Wczytanie zdjecia zapisanego przez FaceLandmark 
			cv::Mat imageToDatabase;
			cv::Mat processedImage = dlib::toMat(detectFace.image);
			cv::cvtColor(processedImage, imageToDatabase, CV_BGR2RGB);

			cv::imshow("ImageToDatabase", imageToDatabase); //show the image
			cv::waitKey();

			// TODO: Kodowanie zdjecia do wyslania do bazy danych -> Tylko do testów
			// w ostatecznej wersji bez wysy³ania zdjêcia tylko punkty

			std::vector<unsigned char> dataImageBuffer;
			// KODOWANIE OBRAZU DO BUFFORA
			cv::imencode(".jpg", imageToDatabase, dataImageBuffer);
			// DZIALAJACA KONVERSA
			DataBuf buffer((char*)(&dataImageBuffer[0]), dataImageBuffer.size());
			std::istream image(&buffer);
			std::istream *pimage;
			pimage = &image;

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
				pimage, //TODO: Ostatecznie nie bedzie wysylc przetworzonego zdjecia do bazy
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
					ID		//id klienta
				);
			}
			else
			{
				cout << "Rekord ju¿ jest w tabeli" << endl;
			}

			delete sendData;


#pragma endregion


		}
		catch (exception& ex)
		{
			cout << "\nexception thrown!" << endl;
			cout << ex.what() << endl;
			cin.get();
		}

	}


	void CalculateProfileFeaturePoints(int ID)
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
				profileCalib.ShowLoadedImage();
				//cv::waitKey();
				if (profileCalib.FindCorrenrsOnMarker("KalibracjaProfilowe_.jpg") != true)
				{
					cout << "Nie znaleziono markera na obrazie program sie wylaczy" << endl;
					cv::waitKey();

					return; //TODO: Mo¿e jakoœ lepiej to rozwi¹zaæ?
				}
				double mmProfileScaleFactor = profileCalib.CalculateScaleFactor();
				
				//-- end Profile Image Calibration -------------------------------------------------

				



				//Wyslanie informacji do bazy

				SqlConnection *sendData = new SqlConnection(host, root, pass, databaseName);
				sendData->Connect();
				//TODO: Trzeba to jeszcze przetestowaæ!!!!!!!!!!!!!!!!!!!!!!!!!
				bool recordExists = sendData->CheckIfKlientExists(ID, "Punkty_profil");
				//jezeli nie ma to dodadajemy do tabeli
				if (recordExists == false)
				{
					int currentIdCount = sendData->GetRowCount("Punkty_Profil");
					// Insert to coordinates table
					sendData->CoordinatesInsertStatement //Dodatnie wspolczynika skalowania do tabeli 
					(
						currentIdCount + 1,
						Point2D(0, 0),
						Point2D(0, 0),
						mmProfileScaleFactor,
						ID		//id klienta
					);
					//zakonczenie po³¹czenia
				}
				else
				{ // jesli jest
					// TODO: Wymyœleæ jakiœ lepszy komunikat
					cout << "Dany rekord juz byl w tabeli" << endl;

				}
				delete sendData;

		}
		catch (exception& ex)
		{
			cout << "\nexception thrown!" << endl;
			cout << ex.what() << endl;
			cin.get();
		}
#pragma endregion

	}
}