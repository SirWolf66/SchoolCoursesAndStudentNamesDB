#include "schooldbSQLoperations.h"
#include <string>
#include <iostream>
#include <regex>

#include "HelperFunctions.h"

//SQLite library
#include "SQL_Library/sqlite3.h"

//INclude the catch 2 library
#include"Catch2/catch.hpp"

//---- File input ----
std::string ReadCSVFile(sqlite3* db, char* zErrMsg, int& rc, const std::string& filePath)
{
	int retakeScore{};
	int firstTakeScore{};
	std::string text{};
	std::string sentence{};
	std::string courseName{};
	std::string firstName{};
	std::string lastName{};
	std::string studentId{};

	std::ifstream in{ filePath };

	//(studentId,score,score)
	const std::regex extractFileName(".*/(.*)\\\..+");
	const std::regex extractModifyDataStudentname("(.+),(.+),(.+),(.+)");
	const std::regex extractModifyDataStudentId("(.+),(.+),(.+)");

	std::smatch match;
	if (std::regex_search(filePath, match, extractFileName))
	{
		courseName = match.str(1);
	}
	else
	{
		std::cout << "File name not found";
	}

	while (in)
	{
		//Get a line of the file
		ReadSentence(in, sentence);
		if (sentence == "")
		{
			continue;
		}

		//Determine if the gotten line uses the student name or the studentId
		if (CheckFileStudentIdModify(sentence))
		{
			if (std::regex_search(sentence, match, extractModifyDataStudentId))
			{
				studentId = match.str(1);
				firstTakeScore = std::stoi(match.str(2));
				retakeScore = std::stoi(match.str(3));
			}
			ModifyStudentScores(db, zErrMsg, rc, studentId, courseName, "firsttake", firstTakeScore);
			ModifyStudentScores(db, zErrMsg, rc, studentId, courseName, "retake", retakeScore);
		}
		else if (CheckFileStudentNameModify(sentence))
		{
			if (std::regex_search(sentence, match, extractModifyDataStudentname))
			{
				firstName = match.str(1);
				lastName = match.str(2);
				firstTakeScore = std::stoi(match.str(3));
				retakeScore = std::stoi(match.str(4));
			}
			ModifyStudentScores(db, zErrMsg, rc, firstName, lastName, courseName, "firsttake", firstTakeScore);
			ModifyStudentScores(db, zErrMsg, rc, firstName, lastName, courseName, "retake", retakeScore);
		}
		else 
		{
			std::cout << "Invalid input: " << sentence;
		}

		text += sentence;
		sentence.clear();
		std::string line{};
	}

	return text;
}

//---- Data adding ----
void addDataEntry(sqlite3* db, char* zErrMsg, int& rc, const std::string& data, const std::string& dataKind, const std::string& dbNAme)
{
	std::string sqlQuery{};
	sqlQuery = "INSERT INTO " + dbNAme + ' ' + dataKind + " VALUES " + "(" + data + ");";

	const char* sql{ sqlQuery.c_str() };
	Insert(db, zErrMsg, rc, sql);
}
//Regquires "firstName,lastName" in data
void AddStudent(sqlite3* db, char* zErrMsg, int& rc, const std::string& data)
{
	std::string dataReformated{};
	std::string dataKind{"(firstName,lastName,studentId)"};

	//Adding the parentheses to the First and Last name, so sql will recognize it as a string
	const std::regex studentInputRegex("(.+),(.+)");
	std::smatch match;
	if (std::regex_search(data, match, studentInputRegex) && match.size() > 1)
	{
		dataReformated += '\'' + match.str(1) + '\'';
		dataReformated += ',';
		dataReformated += '\'' + match.str(2) + '\'';
	}
	else
	{
		std::cout << "!Student reformatting failed \n";
		return;
	}



	//Adding the student ID
	std::string studentID{ RandomIP4Adress() };
	dataReformated += ",'" + studentID + '\'';
	addDataEntry(db, zErrMsg, rc, dataReformated, dataKind, "students");

	//After a student is added, then add him to the reults and give him the standard 0 for all courses
	//First get all of the id's of all of the existing courses


}
//Regquires "courseName" in data
void AddCourse(sqlite3* db, char* zErrMsg, int& rc, const std::string& data)
{
	std::string dataReformated{};
	std::string dataKind{ "(name)" };

	dataReformated = '\'' + data + '\'';

	addDataEntry(db, zErrMsg, rc, dataReformated, dataKind, "courses");
}
//Results need to be finsihed
void AddResults(sqlite3* db, char* zErrMsg, int& rc, const std::string& data)
{
	std::string dataReformated{};
	std::string dataKind{ "(studentId,courseId,firsttake,reTake)" };

	//Adding the parentheses to the First and Last name, so sql will recognize it as a string
	const std::regex studentInputRegex("(.+),(.+),(.+),(.+)");
	const std::regex studentId("\\d*.");
	const std::regex courseId("\\d*");
	std::smatch match;
	if (std::regex_search(data, match, studentInputRegex) && match.size() > 1)
	{
		//studentId
		if (!std::regex_match(match.str(1), studentId))
		{

		}
		
		//courseId
		if (!std::regex_match(match.str(2), courseId))
		{

		}
	}
	else
	{
		std::cout << "!Student reformatting failed \n";
		return;
	}

	addDataEntry(db, zErrMsg, rc, data, dataKind, "results");
}

//---- Modifying data ----
void ModifyStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& firstName, const std::string& lastName, const std::string& course, const std::string& examPeriod, const int score)
{
	std::string studentId{};
	std::vector<std::vector<std::string>> studentIdData{};

	studentIdData = GetTableData(db, zErrMsg, rc, "students", "studentId", "firstName = '" + firstName + "' AND lastName = '" + lastName+ '\'');
	
	//If studentIdData is empty, then the student does not exist yet and we are going to need to first add the student to the db
	if (studentIdData.empty())
	{
		std::string data = firstName + ',' + lastName;
		AddStudent(db, zErrMsg, rc, data);
		studentIdData = GetTableData(db, zErrMsg, rc, "students", "studentId", "firstName = '" + firstName + "' AND lastName = '" + lastName + '\'');
	}

	if (int(studentIdData[0].size()) == 2)
	{
		studentId = studentIdData[0][1];
	}
	else
	{
		std::cout << "Multiple students with the name: \"" + firstName + " " + lastName + " have been found";
	}

	//Find the student id to match the given name

	ModifyStudentScores(db, zErrMsg, rc, studentId, course, examPeriod, score);
}
void ModifyStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& studentId, const std::string& courseName, const std::string& examPeriod, const int score)
{
	const std::string& table1{ "students" };
	const std::string& table2{ "courses" };
	const std::string& table3{ "results" };
	std::string sqlQuery{};
	std::vector<std::vector<std::string>> studentData{ GetTableData(db, zErrMsg, rc, table1, "id", "studentId = '" + studentId +'\'') };
	std::vector<std::vector<std::string>> courseData{ GetTableData(db, zErrMsg, rc, table2, "id", "name = '" + courseName + "'") };

	//If the courseData is empty, this means that the given couse does not exist yet, so add the course first
	if(courseData.empty())
	{
		AddCourse(db, zErrMsg, rc, courseName);
		courseData = GetTableData(db, zErrMsg, rc, table2, "id", "name = '" + courseName + "'");
	}

	//If the studentData is empty, this means that the given studentId does not exist yet, we are not makin a new student entry, cause no game is known of the student
	if (studentData.empty())
	{
		std::cout << "studentId: " + studentId + " Does not exist, results are not added to the db, please add the student first";
		return;
	}

	//Creat the sql query
	sqlQuery = "studentId = " + studentData[0][1] + " AND courseId = " + courseData[0][1];

	const char* whereSql = sqlQuery.c_str();
	UpdateItemInTable(db, zErrMsg, rc, table3, std::to_string(score), examPeriod, whereSql);
}
void ModifyStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& data)
{
	int score{};
	std::string examPeriod{};
	std::string firstName{};
	std::string lastName{};
	std::string studentId{};
	std::string courseName{};
	std::string courseId{};
	std::string combinedName{};

	//((First name,Last name) OR studentId,courseName OR courseId,examPeriod,score)
	const std::regex ModifyStudentScoresRegex("([\\d.]+|.+),(\\d|.+),(.+),(.+)");
	const std::regex IsNumberRegex("\\d+");
	const std::regex SeperateNameregex("(.+),(.+)");

	std::smatch match;
	std::smatch nameMatch;

	std::regex_search(data, match, ModifyStudentScoresRegex);

	if (CheckStudentInputData(match.str(1)))
	{
		combinedName = match.str(1);
		if (std::regex_search(combinedName, nameMatch, SeperateNameregex))
		{
			firstName = nameMatch.str(1);
			lastName = nameMatch.str(2);
		}
	}
	else
	{
		studentId = match.str(1);
	}

	if (!regex_match(match.str(2), IsNumberRegex))
	{
		courseName = match.str(2);
	}
	else
	{
		courseId = match.str(2);
	}

	examPeriod = match.str(3);
	score = std::stoi(match.str(4));

	//See if the student has been identified by studentId or name and act accordingly
	if (!studentId.empty())
	{
		ModifyStudentScores(db, zErrMsg, rc, studentId, courseName, examPeriod, score);
	}
	else
	{
		ModifyStudentScores(db, zErrMsg, rc, firstName, lastName, courseName, examPeriod, score);
	}
}

//---- Input data checking ----
//Manual
bool CheckStudentInputData(const std::string& data)
{
	//Need First name and Last name
	//const std::regex studentInputRegex("[[:alpha:]\\s\\\'-]+,[[:alpha:]\\s\\\'\\\-]+,(?:\\d{1,3}\\\.){3}\\d{1,3}");
	const std::regex studentInputRegex("[[:alpha:]\\\'-]+,[[:alpha:]\\\'-]+");

	if (!regex_match(data, studentInputRegex))
	{
		return false;
	}

	return true;
}
bool CheckCourseInputData(const std::string& data)
{
	//Need course name
	const std::regex studentInputRegex("[[:alnum:]]+");

	if (!regex_match(data, studentInputRegex))
	{
		return false;
	}

	return true;
}
bool CheckResultsInputData(const std::string& data)
{
	//((lastName,firstName) OR studentId, courseName OR courseId, firstTake, ReTake)
	const std::regex studentInputRegex("(?:(?:[[:alpha:]]+,[[:alpha:]]+)|(?:(?:\\d{1,3}\\\.){3}\\d{1,3})),(?:(?:[[:alpha:]]+)|(?:\\d+)),(?:(?:1?[0-9])|(?:20)),(?:(?:1?[0-9])|(?:20))");

	if (!regex_match(data, studentInputRegex))
	{
		return false;
	}

	return true;
}
bool CheckModifyScoreInputData(const std::string& data)
{
	//((First name,Last name) OR studentId,courseName OR courseId,examPeriod,score)
	const std::regex studentInputRegex("(?:(?:[[:alpha:]]+,[[:alpha:]]+)|(?:(?:\\d{1,3}\\\.){3}\\d{1,3})),(?:(?:[[:alpha:]]+)|(?:\\d+)),(?:firsttake|retake),(?:(?:1?[0-9])|(?:20))");

	if (!regex_match(data, studentInputRegex))
	{
		return false;
	}

	return true;
}
//From file
bool CheckFileStudentNameModify(const std::string& data)
{
	//(firstName, lastName, score, score)
	const std::regex studentInputRegex("[[:alpha:]\\\'-]+,[[:alpha:]\\\'-]+,(?:(?:1?[0-9])|(?:20)),(?:(?:1?[0-9])|(?:20))");

	if (!regex_match(data, studentInputRegex))
	{
		return false;
	}

	return true;
}
bool CheckFileStudentIdModify(const std::string& data)
{
	//(studentId,score,score)
	const std::regex studentInputRegex("(?:(?:\\d{1,3}\\\.){3}\\d{1,3}),(?:(?:1?[0-9])|(?:20)),(?:(?:1?[0-9])|(?:20))");

	if (!regex_match(data, studentInputRegex))
	{
		return false;
	}

	return true;
}

//---- Displaying data ----
void DisplayStudents(sqlite3* db, char* zErrMsg, int& rc)
{
	DisplayTableWhere(db, zErrMsg, rc, "students");
}
void DisplayCourses(sqlite3* db, char* zErrMsg, int& rc)
{
	DisplayTableWhere(db, zErrMsg, rc, "courses");
}
//Make sure to put a "," between the first and last name in "studentName"
void DisplayStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& studentName)
{
	const char* sql{};
	std::string sqlQuery{};
	std::string firstName{};
	std::string lastName{};

	const std::regex studentInputRegex("(.+),(.+)");
	std::smatch match;
	if (std::regex_search(studentName, match, studentInputRegex) && match.size() > 1)
	{
		firstName = match.str(1);
		lastName = match.str(2);
	}

	sqlQuery =
		"SELECT "
			"students.firstName AS FirstName, "
			"students.lastName AS LastName, "
			"courses.name AS CourseName, "
			"results.firstTake AS FirstTake, "
			"results.reTake AS Retake "
		"FROM "
			"results "	//The base table
			"INNER JOIN students ON results.studentId = students.id "	//Table1 you want to match, so here its the student id
			"INNER JOIN courses ON results.courseId = courses.id "		//Table2 you want to match, so here its the courses id
		"WHERE "
			"students.firstName = '" + firstName + "' "
		"AND "
			"students.lastName = '" + lastName + "' "
		;
	sql = sqlQuery.c_str();
	DispayTableData(db, zErrMsg, rc, sql);
}
void DisplayAverageScoreOfCourse(sqlite3* db, char* zErrMsg, int& rc, const std::string& course)
{
	const char* sql{};
	int scoreTotal{};
	float scoreAverage{};
	std::string sqlQuery{};

	std::vector<std::vector<std::string>> columnsData{};
	std::vector<std::vector<std::string>> columnsDataUpdated{};

	sqlQuery =
		"SELECT "
			"courses.name AS CourseName, "
			"results.firstTake AS Retake, "
			"results.reTake AS Retake "
		"FROM "
			"results "	//The base table
			"INNER JOIN courses ON results.courseId = courses.id "		//Table1 you want to match, so here its the courses id
		"WHERE "
			"courses.name == '" + course + "' "
		;
	sql = sqlQuery.c_str();
	columnsData = GetTableDataCustom(db, zErrMsg, rc, sql);

	//Add the course name
	columnsDataUpdated.push_back(std::vector<std::string>{columnsData[0][0].c_str(), columnsData[0][1].c_str()});

	//Add the avetrage score of the first take and the retake
	for (int i{1}; i < int(columnsData.size()); i++)
	{
		columnsDataUpdated.push_back(std::vector<std::string>{columnsData[i][0].c_str()});
		for (int j{1}; j < int(columnsData[i].size()); j++)
		{
			scoreTotal += std::stoi(columnsData[i][j]);
		}
		scoreAverage = float(scoreTotal) / float(columnsData[i].size() - 1);
		columnsDataUpdated[i].push_back(std::to_string(scoreAverage));
		scoreTotal = 0;
	}

	std::cout << "The Average: \n";
	DisplayTableInConsole(columnsDataUpdated);
}

#pragma region UnitTests
TEST_CASE("CheckStudentInput")
{
	REQUIRE(CheckStudentInputData("cat,joe"));
	REQUIRE(CheckStudentInputData("john-si,k'rdhun"));

	REQUIRE(!CheckStudentInputData("abc1,jay"));
	REQUIRE(!CheckStudentInputData("ab&*c1,jay,"));
}
TEST_CASE("CheckCourseInput")
{
	REQUIRE(CheckCourseInputData("math1"));
	REQUIRE(CheckCourseInputData("justmath"));

	REQUIRE(!CheckCourseInputData("math_2"));
	REQUIRE(!CheckCourseInputData("%german+"));
	REQUIRE(!CheckCourseInputData("English and Welsh"));
}
TEST_CASE("CheckResultInput")
{
	REQUIRE(CheckResultsInputData("Joe,Doe,Math,0,10"));
	REQUIRE(CheckResultsInputData("Joe,m,1,0,10"));
	REQUIRE(CheckResultsInputData("Joe,d,Math,0,20"));
	REQUIRE(CheckResultsInputData("1.1.1.1,Math,1,15"));

	REQUIRE(!CheckResultsInputData("Joe,Math,0,21"));
	REQUIRE(!CheckResultsInputData("Joe,d,Math,0,21"));
	REQUIRE(!CheckResultsInputData("math_2"));
	REQUIRE(!CheckResultsInputData("1.1.1.1,2.2.2.2,1,15"));
	REQUIRE(!CheckResultsInputData("English and Welsh"));
}
TEST_CASE("CheckModifyScoreInputData")
{
	REQUIRE(CheckModifyScoreInputData("Joe,Doe,Math,retake,10"));
	REQUIRE(CheckModifyScoreInputData("Joe,m,1,firsttake,10"));
	REQUIRE(CheckModifyScoreInputData("Joe,d,Math,retake,20"));
	REQUIRE(CheckModifyScoreInputData("1.1.1.1,Math,retake,15"));

	REQUIRE(!CheckModifyScoreInputData("Joe,Math,firsttake,21"));
	REQUIRE(!CheckModifyScoreInputData("Joe,d,Math,0,21"));
	REQUIRE(!CheckModifyScoreInputData("math_2"));
	REQUIRE(!CheckModifyScoreInputData("1.1.1.1,2.2.2.2,1,15"));
	REQUIRE(!CheckModifyScoreInputData("English and Welsh"));
}
TEST_CASE("CheckFileStudentNameModify")
{
	REQUIRE(CheckFileStudentNameModify("Joe,Doe,0,10"));
	REQUIRE(CheckFileStudentNameModify("john-si,k'rdhun,5,20"));

	REQUIRE(!CheckFileStudentNameModify("Joe,Math,0,21"));
	REQUIRE(!CheckFileStudentNameModify("Joe,0,21"));
	REQUIRE(!CheckFileStudentNameModify("Joe,Math,21"));
}
TEST_CASE("CheckFileStudentIdModify")
{
	REQUIRE(CheckFileStudentIdModify("1.1.1.1,0,10"));

	REQUIRE(!CheckFileStudentIdModify("1.1.1.1,0,21"));
	REQUIRE(!CheckFileStudentIdModify("1.1.1.1,21"));
	REQUIRE(!CheckFileStudentIdModify("1.1.1,21"));
	REQUIRE(!CheckFileStudentIdModify("1.1,21"));
	REQUIRE(!CheckFileStudentIdModify("1.,21"));
}
#pragma endregion
