#pragma once
#include <fstream>
#include <string>
#include <vector>

//SQLite library
#include "SQL_Library/sqlite3.h"

//---- File reading ----
std::string ReadFile(const std::string& filePath, const char endLineCharachter = char());
void ReadSentence(std::istream& istream, std::string& sentence);
void ReadSentence(std::istream& istream, std::string& sentence, const char endLineCharachter);

//---- SQL basics ----
void CreateTable(sqlite3* db, char* zErrMsg, int& rc, const char* sql);
void Insert(sqlite3* db, char* zErrMsg, int& rc, const char* sql);
void SelectTable(sqlite3* db, char* zErrMsg, int& rc, const std::string& table);
void GeneralExecuteSQLStatement(sqlite3* db, char* zErrMsg, int& rc, const char* sql);
//Get data from a table
std::vector<std::vector<std::string>> GetTableData(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, std::string columnsToDisplay = "", const std::string& where = "");
//GetData fromTable, but with custom sql
std::vector<std::vector<std::string>> GetTableDataCustom(sqlite3* db, char* zErrMsg, int& rc, const char* sql);

//Table display
void DispayTableData(sqlite3* db, char* zErrMsg, int& rc, const char* sql);
void DisplayTableWhere(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, std::string columnsToDisplay = "", const std::string& where = "");
void DisplayTableInnerJoin(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, const std::string& otherTable, std::string columnsToDisplay, const std::string& InnerJoin);
void DisplayTableInConsole(const std::vector<std::vector<std::string>>& _tableInformation, const std::vector<std::string>& columnsToDisplay);
//Display the entire table
void DisplayTableInConsole(const std::vector<std::vector<std::string>>& _tableInformation);
void SortDataStringIntoVector(const std::string& _data, std::vector<std::vector<std::string>>& sortedVector);

//Update items in Table
void UpdateItemInTable(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, const std::string& columnData, const std::string& column, const char* whereSql);
void UpdateItemsInTable(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, const std::vector<std::string>& columnData, const std::vector<std::string>& columns, const char* whereSql);

//SQl callbacks
static int callback(void* data, int argc, char** argv, char** azColName);
static int callbackDisplayTable(void* data, int argc, char** argv, char** azColName);

//---- InputCheck ----
std::string GetInputLine();
bool CheckInput(const std::string& input, std::vector<std::string> inputOptions);

//---- Misc ----
float RandomFloatValue(const float start, const float end);
int RandomIntValue(const int start, const int end);
std::string RandomIP4Adress();
