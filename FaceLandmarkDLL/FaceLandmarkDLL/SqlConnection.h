#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\calib3d.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <mysql.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "Point2D.h"

///@class 
///Class used to connect to MySQL database
/// Acqiure images and sending calculations to database
class SqlConnection
{
public:
	//Constructor establish connection to database
	SqlConnection(sql::SQLString host, sql::SQLString root, sql::SQLString  pass, sql::SQLString Schema);
	~SqlConnection();

	// Connect to database specified in constructor
	void Connect();
	cv::Mat GetImageFromDatabase(sql::SQLString imageID);//, sql::SQLString kolumnName);
														 //Get Profile image from database (TODO zrobic tak zeby byla jedna funkcji do roznych kolumn a nie 2 odzielne funkcje)
	cv::Mat GetProfileImageFromDatabase(sql::SQLString imageID);//, sql::SQLString kolumnName);

	// Update Current record in database with foto
	void CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, std::istream *imageStream, int ID);
	// Update Current record ind database without updatign foto
	void CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, int ID);
	
	void CustomerInsertStatement();
	// Insert new set of coordinates to database
	void CoordinatesInsertStatement(int ID, Point2D leftTemple, Point2D rightTemple, Point2D Nose, Point2D leftEye, Point2D rightEye, Point2D leftCheek,
		Point2D rightCheek, double scaleFactor, int customerID);
	void CoordinatesInsertStatement(int ID, Point2D Nose, Point2D Ear, double profilescaleFactor, int customerID);
	//Check if specyfied record with specyfied ID exists in profile points table table xd
	bool CheckIfKlientExists(int ID, std::string tableName);
	///@brief Get table row count
	int GetRowCount(sql::SQLString tableName);


private:

	sql::SQLString hostName;
	sql::SQLString user;
	sql::SQLString password;
	sql::SQLString databaseName;

	//SQL driver object to generate connection do database
	sql::Driver *driver;
	//Acqire connection
	sql::Connection *con;
	//Performing sql statements
	sql::Statement *stmt;
	//Containing results from querry
	sql::ResultSet *res;
	//Object to generate prepared statements
	sql::PreparedStatement *pstmt;
};

