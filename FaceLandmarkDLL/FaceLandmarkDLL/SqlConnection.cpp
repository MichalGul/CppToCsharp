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
	//cleaning up
	delete con;
	delete stmt;
	delete res;
	delete pstmt;


}
// Connect to database specified in constructor
void SqlConnection::Connect()
{
	try
	{//Acqire connection

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


//Nazwa tabeli jest wpisana na sztywno
cv::Mat SqlConnection::GetImageFromDatabase(sql::SQLString imageID)//, sql::SQLString tableName)
{
	//image from database
	cv::Mat acquiredImage;
	// data lenght
	int dataLength = 0;

	try {
		// Nazwa kolumny bazy musi byc znana		
		sql::SQLString querry = "SELECT LENGTH(zdjecie), zdjecie FROM klienci WHERE id = ";
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
//prawie to samo co metoda pobierana poprzedniego zdjecia 
cv::Mat SqlConnection::GetProfileImageFromDatabase(sql::SQLString imageID)
{
	//image from database
	cv::Mat acquiredImage;
	// data lenght
	int dataLength = 0;

	try {
		// Nazwa kolumny bazy musi byc znana		
		sql::SQLString querry = "SELECT LENGTH(zdjecie_profil), zdjecie_profil FROM klienci WHERE id = ";
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
// 
void SqlConnection::CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, std::istream *imageStream, int ID)
{

	try
	{
		pstmt = con->prepareStatement("UPDATE klienci SET Rozstaw_Zrenic = ?, Szerokosc_Twarzy = ?, Szerokosc_Skroni = ?, PraweOko_Nos = ?, LeweOko_Nos = ?, zdjecie = ? WHERE id = ?");
		pstmt->setDouble(1, eyeDistance);
		pstmt->setDouble(2, faceWidth);
		pstmt->setDouble(3, templeWidth);
		pstmt->setDouble(4, rightEyeNoseDist);
		pstmt->setDouble(5, leftEyeNoseDist);
		pstmt->setBlob(6, imageStream); // wysyla sie ale trzeba zwiekszyc max_allowed_packet
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
//Update klients table bez wstawiania zdjecia
void SqlConnection::CustomerUpdateStatement(double eyeDistance, double faceWidth, double templeWidth, double rightEyeNoseDist, double leftEyeNoseDist, int ID)
{

	try
	{
		pstmt = con->prepareStatement("UPDATE klienci SET Rozstaw_Zrenic = ?, Szerokosc_Twarzy = ?, Szerokosc_Skroni = ?, PraweOko_Nos = ?, LeweOko_Nos = ? WHERE id = ?");
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

// TBD zastanowic sie czy to bedzie tu potrzebne -> update wystarczy
void SqlConnection::CustomerInsertStatement()
{


}




void SqlConnection::CoordinatesInsertStatement(int ID, Point2D leftTemple, Point2D rightTemple, Point2D Nose, Point2D leftEye, Point2D rightEye, Point2D leftCheek, Point2D rightCheek, double scaleFactor, int customerID)
{
	try
	{

		pstmt = con->prepareStatement("INSERT INTO Punkty VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
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

void SqlConnection::CoordinatesInsertStatement(int ID, Point2D Nose, Point2D Ear, double profilescaleFactor, int customerID)
{
	try
	{

		pstmt = con->prepareStatement("INSERT INTO Punkty_Profil VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
		pstmt->setInt(1, ID);
		pstmt->setInt(2, Nose.X);
		pstmt->setInt(3, Nose.Y);
		pstmt->setInt(4, Ear.X);
		pstmt->setInt(5, Ear.Y);
		pstmt->setDouble(6, 0); // TODO: zastanowic sie co tu wstawiac poczatkowa odleg³oœæ 
		pstmt->setDouble(7, profilescaleFactor);
		pstmt->setInt(8, customerID);

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

//Sprawdzenie czy w tabeli jest ju¿ rekors z danym currentID w tabeli Punkty_profil w kolumnie klient ID -> bo jesli jest i bedzie chcia³o siê
//wstawiæ to bêdzie 
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
		// Nazwa kolumny bazy musi byc znana		
		sql::SQLString querry = "SELECT COUNT(Id) FROM ";
		sql::SQLString querrymMid = querry.append(tableName);// WHERE id = "; 	

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


