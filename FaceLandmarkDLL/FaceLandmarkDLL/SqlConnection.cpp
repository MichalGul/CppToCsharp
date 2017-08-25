#include "SqlConnection.h"



SqlConnection::SqlConnection(sql::SQLString host, sql::SQLString root, sql::SQLString pass, sql::SQLString schema)
{

	hostName = host;
	user = root;
	databaseName = schema;
	password = pass;

	pstmt = NULL;
	stmt = NULL;
	res = NULL;

}

SqlConnection::~SqlConnection()
{
	//Cleaning up
	delete con;
	delete stmt;
	delete res;
	delete pstmt;


}
// Connect to database specified in constructor
void SqlConnection::Connect()
{
	try
	{
		//Acqire connection
		driver = get_driver_instance();
		if (con = driver->connect(hostName, user, password))
		{
			std::cout << "Nawiazano polaczenie" << std::endl;
		};
		con->setSchema(databaseName);


	}

	//Error description
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}

}



cv::Mat SqlConnection::GetImageFromDatabase(sql::SQLString imageID)
{
	//image from database
	cv::Mat acquiredImage;
	// data lenght
	int dataLength = 0;

	try {
				
		sql::SQLString querry = "SELECT LENGTH(zdjecie), zdjecie FROM Klienci WHERE id = ";
		sql::SQLString querrymMid = querry.append(imageID);// WHERE id = "; 	
													   //creating statement and executing querry 
		stmt = con->createStatement();
		res = stmt->executeQuery(querrymMid);

		//Handling data		
		res->next();
		dataLength = res->getInt(1);
		//Alocating memory buffer
		std::vector<char> imageBuffor(dataLength);
		// result stream from databesa captured as blob
		std::istream * resultBlob = res->getBlob(2);
		//Reading blob stream into memory buffor
		resultBlob->read(&imageBuffor[0], dataLength);
		//Image decode
		acquiredImage = cv::imdecode(imageBuffor, -1);

		delete resultBlob;
		return acquiredImage;
	}

	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}




}

cv::Mat SqlConnection::GetProfileImageFromDatabase(sql::SQLString imageID)
{
	//image from database
	cv::Mat acquiredImage;
	// data lenght
	int dataLength = 0;

	try {
				
		sql::SQLString querry = "SELECT LENGTH(zdjecie_profil), zdjecie_profil FROM Klienci WHERE id = ";
		sql::SQLString querrymMid = querry.append(imageID);// WHERE id = "; 	

														   //creating statement and executing querry 
		stmt = con->createStatement();
		res = stmt->executeQuery(querrymMid);

		//Handling data		
		res->next();
		dataLength = res->getInt(1);
		//Alocating memory buffer
		std::vector<char> imageBuffor(dataLength);
		// result stream from databesa captured as blob
		std::istream * resultBlob = res->getBlob(2);
		//Reading blob stream into memory buffor
		resultBlob->read(&imageBuffor[0], dataLength);
		//Image decode
		acquiredImage = cv::imdecode(imageBuffor, -1);

		delete resultBlob;
		return acquiredImage;

	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

void SqlConnection::CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, std::istream *imageStream, int ID)
{

	try
	{
		pstmt = con->prepareStatement("UPDATE Klienci SET Rozstaw_Zrenic = ?, Szerokosc_Twarzy = ?, Szerokosc_Skroni = ?, PraweOko_Nos = ?, LeweOko_Nos = ?, zdjecie = ? WHERE id = ?");
		pstmt->setDouble(1, eyeDistance);
		pstmt->setDouble(2, faceWidth);
		pstmt->setDouble(3, templeWidth);
		pstmt->setDouble(4, rightEyeNoseDist);
		pstmt->setDouble(5, leftEyeNoseDist);
		pstmt->setBlob(6, imageStream);
		pstmt->setInt(7, ID);
		pstmt->execute();



	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}




}

void SqlConnection::CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, int ID)
{

	try
	{
		pstmt = con->prepareStatement("UPDATE Klienci SET Rozstaw_Zrenic = ?, Szerokosc_Twarzy = ?, Szerokosc_Skroni = ?, PraweOko_Nos = ?, LeweOko_Nos = ? WHERE id = ?");
		pstmt->setDouble(1, eyeDistance);
		pstmt->setDouble(2, faceWidth);
		pstmt->setDouble(3, templeWidth);
		pstmt->setDouble(4, rightEyeNoseDist);
		pstmt->setDouble(5, leftEyeNoseDist);		
		pstmt->setInt(6, ID);
		pstmt->execute();



	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}




}

void SqlConnection::CustomerUpdateStatement(double earNoseDistance, double eyeNoseDistance, int ID)
{

	try
	{
		pstmt = con->prepareStatement("UPDATE Klienci SET Ucho_Nos = ?, Oko_Nos = ? WHERE id = ?");
		pstmt->setDouble(1, earNoseDistance);
		pstmt->setDouble(2, eyeNoseDistance);
		pstmt->setInt(3, ID);
		pstmt->execute();


	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}




}






void SqlConnection::CoordinatesInsertStatement(int ID, Point2D leftTemple, Point2D rightTemple, Point2D Nose, Point2D leftEye, Point2D rightEye, Point2D leftCheek, Point2D rightCheek, double scaleFactor, double scaleFactorMax, int customerID)
{
	try
	{

		pstmt = con->prepareStatement("INSERT INTO Punkty VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		pstmt->setInt(1, ID);
		pstmt->setInt(2, leftTemple.X);
		pstmt->setInt(3, leftTemple.Y);
		pstmt->setInt(4, rightTemple.X);
		pstmt->setInt(5, rightTemple.Y);
		pstmt->setInt(6, Nose.X);
		pstmt->setInt(7, Nose.Y);
		pstmt->setInt(8, leftEye.X);
		pstmt->setInt(9, leftEye.Y);
		pstmt->setInt(10, rightEye.X);
		pstmt->setInt(11, rightEye.Y);
		pstmt->setInt(12, leftCheek.X);
		pstmt->setInt(13, leftCheek.Y);
		pstmt->setInt(14, rightCheek.X);
		pstmt->setInt(15, rightCheek.Y);
		pstmt->setDouble(16, scaleFactor);
		pstmt->setDouble(17, scaleFactorMax);
		pstmt->setInt(18, customerID);

		if (pstmt->execute())
		{
			std::cout << "Wykonano zapytanie" << std::endl;
		}


	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}




}

void SqlConnection::CoordinatesInsertStatement(int ID, Point2D Ear, Point2D Nose, Point2D Eye , double profilescaleFactor, double earNoseDistance, double eyeNoseDistance, int customerID)
{
	try
	{

		pstmt = con->prepareStatement("INSERT INTO Punkty_Profil VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		pstmt->setInt(1, ID);
		pstmt->setInt(2, Ear.X);
		pstmt->setInt(3, Ear.Y);
		pstmt->setInt(4, Nose.X);
		pstmt->setInt(5, Nose.Y);
		pstmt->setInt(6, Eye.X);
		pstmt->setInt(7, Eye.Y);
		pstmt->setDouble(8, earNoseDistance); 
		pstmt->setDouble(9, eyeNoseDistance);
		pstmt->setDouble(10, profilescaleFactor);
		pstmt->setInt(11, customerID);

		if (pstmt->execute())
		{
			std::cout << "Wykonano zapytanie" << std::endl;
		}


	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}


}

void SqlConnection::CoordinatesUpdateStatement(Point2D leftTemple, Point2D rightTemple, Point2D Nose, Point2D leftEye, Point2D rightEye, Point2D leftCheek, Point2D rightCheek, double scaleFactor, double scaleFactorMax, int customerID)
{

	try
	{

		pstmt = con->prepareStatement("UPDATE Punkty SET LSkron_X = ?, LSkron_Y = ?, PSkron_X = ?, PSkron_Y= ?, Nos_X =?, Nos_Y=?, LOko_X=?, LOko_Y=?, POko_X = ?, POko_Y = ?, LPol_X =?, LPol_Y =?, PPol_X = ?, PPol_Y =?, WspSkalowania = ?, WspSkalowaniaMax = ? WHERE Id_klienta = ?");
		
		pstmt->setInt(1, leftTemple.X);
		pstmt->setInt(2, leftTemple.Y);
		pstmt->setInt(3, rightTemple.X);
		pstmt->setInt(4, rightTemple.Y);
		pstmt->setInt(5, Nose.X);
		pstmt->setInt(6, Nose.Y);
		pstmt->setInt(7, leftEye.X);
		pstmt->setInt(8, leftEye.Y);
		pstmt->setInt(9, rightEye.X);
		pstmt->setInt(10, rightEye.Y);
		pstmt->setInt(11, leftCheek.X);
		pstmt->setInt(12, leftCheek.Y);
		pstmt->setInt(13, rightCheek.X);
		pstmt->setInt(14, rightCheek.Y);
		pstmt->setDouble(15, scaleFactor);
		pstmt->setDouble(16, scaleFactorMax);
		pstmt->setInt(17, customerID);

		if (pstmt->execute())
		{
			std::cout << "Wykonano zapytanie" << std::endl;
		}


	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}




}

void SqlConnection::CoordinatesUpdateStatement(Point2D Ear, Point2D Nose, Point2D Eye, double profilescaleFactor, double earNoseDistance, double eyeNoseDistance, int customerID)
{

	try
	{

		pstmt = con->prepareStatement("UPDATE Punkty_Profil SET Ucho_X = ?, Ucho_Y = ?, Nos_X = ?, Nos_Y = ?, Oko_X = ?, Oko_Y = ?, Ucho_Nos = ?, Oko_Nos = ?, WspSkalowania = ? WHERE Id_klienta = ?");		
		pstmt->setInt(1, Ear.X);
		pstmt->setInt(2, Ear.Y);
		pstmt->setInt(3, Nose.X);
		pstmt->setInt(4, Nose.Y);
		pstmt->setInt(5, Eye.X);
		pstmt->setInt(6, Eye.Y);
		pstmt->setDouble(7, earNoseDistance);
		pstmt->setDouble(8, eyeNoseDistance);
		pstmt->setDouble(9, profilescaleFactor);
		pstmt->setInt(10, customerID);

		if (pstmt->execute())
		{
			std::cout << "Wykonano zapytanie" << std::endl;
		}


	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}


}

 
bool SqlConnection::CheckIfKlientExists(int ID, std::string tableName)
{
	sql::SQLString customerID = std::to_string(ID).c_str();
	sql::SQLString tableNameSql = tableName.c_str();
	try {
		// Nazwa kolumny bazy musi byc znana		
		sql::SQLString querry = "SELECT COUNT(Id_klienta) FROM ";// Punkty_profil where Id_klienta = ";
		sql::SQLString querryNext = querry.append(tableNameSql);
		std::string wherePart = " where Id_klienta = ";
		sql::SQLString wherePartSql = wherePart.c_str();
		sql::SQLString querryLast = querryNext.append(wherePartSql);
		sql::SQLString querryComplete = querryLast.append(customerID);

															 //creating statement and executing querry 
		stmt = con->createStatement();
		res = stmt->executeQuery(querryComplete);
		res->next();
		int result = res->getInt(1);
		if (result >=1)
		{
			return true;
		}
		else if(result == 0)
		{
			return false;
		}
		
	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}

	return false;
}

int SqlConnection::GetRowCount(sql::SQLString tableName)
{
	
	try {
				
		sql::SQLString querry = "SELECT MAX(Id) FROM ";
		sql::SQLString querrymMid = querry.append(tableName);	

															 //creating statement and executing querry 
		stmt = con->createStatement();
		res = stmt->executeQuery(querrymMid);
		res->next();
		int rowCount = res->getInt(1);
		return rowCount;
	}
	catch (sql::SQLException &e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}


