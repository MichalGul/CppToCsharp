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
	/// @brief Constructor establish database connection parameters
	/// [in] hostname
	/// [in] user name
	/// [in] password
	/// [in] database name
	SqlConnection(sql::SQLString host, sql::SQLString root, sql::SQLString  pass, sql::SQLString Schema);

	/// @brief Default destructor
	~SqlConnection();

	/// @brief Connect to database specified in constructor
	void Connect();

	/// @brief Get image and convert it to cv Mat format
	/// [in] id of the image
	/// [out] Image in openCV format
	cv::Mat GetImageFromDatabase(sql::SQLString imageID);

	/// @brief Get profile image and convert it to cv Mat format
	/// [in] id of the image
	/// [out] Image in openCV format												 
	cv::Mat GetProfileImageFromDatabase(sql::SQLString imageID);	

	/// @brief Update Current record in database with image on specified ID
	/// [in] eye Distance parameter
	/// [in] face Width 
	/// [in] temple width
	/// [in] first eye nose distance (right)
	/// [in] secon eye nose distance (left)
	/// [in] image memory stream
	/// [in] database customer ID
	void CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, std::istream *imageStream, int ID);
	
	/// @brief Update Current record inddatabase without image on specified ID
	/// [in] eye Distance parameter
	/// [in] face Width 
	/// [in] temple width
	/// [in] first eye nose distance (right)
	/// [in] secon eye nose distance (left)
	/// [in] database customer ID
	void CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, int ID);

	/// @brief Update profile distances after calculation
	/// [in] ear nose distance parameter
	/// [in] eye nose distance
	/// [in] customer ID
	void CustomerUpdateStatement(double earNoseDistance, double eyeNoseDistance, int ID);
		
	/// @brief Insert points coordinates after front feature calculations 
	/// [in] Table row ID
	/// [in] left temple point on image
	/// [in] righ temple point on image
	/// [in] nose point on image
	/// [in] left eye point on image
	/// [in] right eye point on image
	/// [in] left cheek point on image
	/// [in] right cheek point on image
	/// [in] scale factor of the image- pixel to mm ratio base on image scaling
	/// [in] absolute image scale factor of original image resolution
	/// [in] customer ID
	void CoordinatesInsertStatement(int ID, Point2D leftTemple, Point2D rightTemple, Point2D Nose, Point2D leftEye, Point2D rightEye, Point2D leftCheek,
		Point2D rightCheek, double scaleFactor, double scaleFactorMax, int customerID);

	/// @brief Insert points coordinates after profile feature calculations 
	/// [in] Table row ID
	/// [in] ear point
	/// [in] nose point
	/// [in] eye point
	/// [in] profile image scale factor - pixel to mm ratio
	/// [in] distance from nose to ear
	/// [in] distance from eye to nose
	/// [in] customer ID
	void CoordinatesInsertStatement(int ID, Point2D Ear, Point2D Nose, Point2D Eye, double profilescaleFactor, double earNoseDistance, double eyeNoseDistance, int customerID);

	/// @brief Update points coordinates after consecutive front feature calculations 
	/// [in] Table row ID
	/// [in] left temple point on image
	/// [in] righ temple point on image
	/// [in] nose point on image
	/// [in] left eye point on image
	/// [in] right eye point on image
	/// [in] left cheek point on image
	/// [in] right cheek point on image
	/// [in] scale factor of the image- pixel to mm ratio base on image scaling
	/// [in] absolute image scale factor of original image resolution
	/// [in] customer ID
	void CoordinatesUpdateStatement(Point2D leftTemple, Point2D rightTemple, Point2D Nose, Point2D leftEye, Point2D rightEye, Point2D leftCheek,
		Point2D rightCheek, double scaleFactor, double scaleFactorMax, int customerID);
	
	/// @brief Update points coordinates after consecutive profile feature calculations 
	/// [in] Table row ID
	/// [in] ear point
	/// [in] nose point
	/// [in] eye point
	/// [in] profile image scale factor - pixel to mm ratio
	/// [in] distance from nose to ear
	/// [in] distance from eye to nose
	/// [in] customer ID
	void CoordinatesUpdateStatement(Point2D Ear, Point2D Nose, Point2D Eye, double profilescaleFactor, double earNoseDistance, double eyeNoseDistance, int customerID);

	/// @brief Check if specified record with gived Id exists in named table
	/// [in] Id of examined record
	/// [in] name of the checked table
	/// [out] results of searching 
	bool CheckIfKlientExists(int ID, std::string tableName);

	/// @brief Calculate number of rows in table
	/// [in] Name of the table
	int GetRowCount(sql::SQLString tableName);


private:

	sql::SQLString hostName;
	sql::SQLString user;
	sql::SQLString password;
	sql::SQLString databaseName;

	///SQL driver object to generate connection do database
	sql::Driver *driver;
	///Acqire connection
	sql::Connection *con;
	///Performing sql statements
	sql::Statement *stmt;
	///Containing results from querry
	sql::ResultSet *res;
	///Object to generate prepared statements
	sql::PreparedStatement *pstmt;
};

