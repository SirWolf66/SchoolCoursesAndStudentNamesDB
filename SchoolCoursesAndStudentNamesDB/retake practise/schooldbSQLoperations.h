#pragma once
#include <string>
//SQLite library
#include "SQL_Library/sqlite3.h"

//---- File reading ----
std::string ReadCSVFile(sqlite3* db, char* zErrMsg, int& rc, const std::string& filePath);

//---- Data adding ----
void addDataEntry(sqlite3* db, char* zErrMsg, int& rc, const std::string& data, const std::string& dataKind, const std::string& dbNAme);
void AddStudent(sqlite3* db, char* zErrMsg, int& rc, const std::string& data);
void AddCourse(sqlite3* db, char* zErrMsg, int& rc, const std::string& data);
void AddResults(sqlite3* db, char* zErrMsg, int& rc, const std::string& data);

//---- Modify student scores ----
void ModifyStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& firstName, const std::string& lastName, const std::string& course, const std::string& examPeriod,const int score);
void ModifyStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& studentId, const std::string& courseName, const std::string& examPeriod, const int score);
void ModifyStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& data);


//---- Input data checking ----
bool CheckStudentInputData(const std::string& data);
bool CheckCourseInputData(const std::string& data);
bool CheckResultsInputData(const std::string& data);
bool CheckModifyScoreInputData(const std::string& data);
bool CheckFileStudentNameModify(const std::string& data);
bool CheckFileStudentIdModify(const std::string& data);


//---- Displaying data ----
void DisplayStudents(sqlite3* db, char* zErrMsg, int& rc);
void DisplayCourses(sqlite3* db, char* zErrMsg, int& rc);
void DisplayAverageScoreOfCourse(sqlite3* db, char* zErrMsg, int& rc, const std::string& course);
void DisplayStudentScores(sqlite3* db, char* zErrMsg, int& rc, const std::string& studentName);


