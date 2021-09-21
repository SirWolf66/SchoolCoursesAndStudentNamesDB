#include "HelperFunctions.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <array>
#include <sstream>

//SQLite library
#include "SQL_Library/sqlite3.h"
#include <random>

//---- File reading ----
std::string ReadFile(const std::string& filePath, const char endLineCharachter)
{
	std::string text{};
	std::string sentence{};

	std::ifstream in{ filePath };

	if (endLineCharachter != char())
	{
		while (in)
		{
			ReadSentence(in, sentence, endLineCharachter);
			text += sentence;
			sentence.clear();
			std::string line{};
		}
	}
	else
	{
		while (in)
		{
			ReadSentence(in, sentence);
			text += sentence;
			sentence.clear();
			std::string line{};
		}
	}

	return text;
}
void ReadSentence(std::istream& istream, std::string& sentence, const char endLineCharachter)
{
	const char seperatingChar{'\n'};
	std::string line{};

	std::getline(istream, line);

	while (sentence.rfind(endLineCharachter) == std::string::npos && line != "")
	{
		sentence += line + seperatingChar;
		std::getline(istream, line);
	} 
}
void ReadSentence(std::istream& istream, std::string& sentence)
{
	const char seperatingChar{ '\n' };
	std::string line{};

	std::getline(istream, line);
	if (!line.empty())
	{
		sentence += line/* + seperatingChar*/;
	}
}

//---- SQL basics ----
void CreateTable(sqlite3* db, char* zErrMsg, int& rc, const char* sql)
{
	/* Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		std::cout << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		std::cout << "Table created succesfully" << std::endl;
	}
}
void Insert(sqlite3* db, char* zErrMsg, int& rc, const char* sql)
{
	/*Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc)
	{
		std::cout << "Insertions failed: " << sqlite3_errmsg(db) << std::endl;
		return;
	}
	else
	{
		std::cout << "Insertions succesfull " << std::endl;
	}
}
void SelectTable(sqlite3* db, char* zErrMsg, int& rc, const std::string& table)
{
	std::string sqlString = "SELECT * from " + table;
	/*Create SQL statement*/
	const char* sql = sqlString.c_str();
	const char* data = "Callback function called";

	/*Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

	if (rc)
	{
		std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}
	else
	{
		//std::cout << "Opened database successfully \n" << std::endl;
	}
}
void GeneralExecuteSQLStatement(sqlite3* db, char* zErrMsg, int& rc, const char* sql)
{
	/* Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		std::cout << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		std::cout << "Executed succesfully" << std::endl;
	}
}

//Get table Data
//If you want to display all the columns, just use an empty string, or do not give the argument
//Seperate the columns that you want to be displayed in "columnsToDisplay", by a ','
std::vector<std::vector<std::string>> GetTableData(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, std::string columnsToDisplay  /*=""*/, const std::string& where/*=""*/)
{
	std::stringstream* data{ new std::stringstream() };

	//the data that will contain the information found in the selected SQL table
	std::vector<std::vector<std::string>> columnsData{};

	std::string sqlQuery{};
	/*Create SQL statement*/
	if (columnsToDisplay.empty())
	{
		//If the columnsToDisplay is empty, this means we want to display all the columns in the table
		columnsToDisplay = "*";
	}
	sqlQuery += "SELECT " + columnsToDisplay + " FROM " + table;

	if (!where.empty())
	{
		sqlQuery += " WHERE " + where;
	}
	const char* sql = sqlQuery.c_str();

	/*Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callbackDisplayTable, (void*)data, &zErrMsg);

	if (rc)
	{
		std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return columnsData;
	}
	else
	{
		//std::cout << "Opened database successfully \n" << std::endl;
	}

	SortDataStringIntoVector(data->str(), columnsData);
	return columnsData;
}
std::vector<std::vector<std::string>> GetTableDataCustom(sqlite3* db, char* zErrMsg, int& rc, const char* sql)
{
	std::stringstream* data{ new std::stringstream() };
	//the data that will contain the information found in the selected SQL table
	std::vector<std::vector<std::string>> columnsData{};

	/*Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callbackDisplayTable, (void*)data, &zErrMsg);

	if (rc)
	{
		std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return columnsData;
	}
	else
	{
		//std::cout << "Opened database successfully \n" << std::endl;
	}

	SortDataStringIntoVector(data->str(), columnsData);
	return columnsData;
}

//Table display
void DispayTableData(sqlite3* db, char* zErrMsg, int& rc, const char* sql)
{
	std::stringstream* data{ new std::stringstream() };

	//the data that will contain the information found in the selected SQL table
	std::vector<std::vector<std::string>> columnsData{};

	/*Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callbackDisplayTable, (void*)data, &zErrMsg);

	if (rc)
	{
		std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}
	else
	{
		//std::cout << "Opened database successfully \n" << std::endl;
	}

	SortDataStringIntoVector(data->str(), columnsData);
	DisplayTableInConsole(columnsData);
}
//If you want to display all the columns, just use an empty string, or do not give the argument
//Seperate the columns that you want to be displayed in "columnsToDisplay", by a ','
void DisplayTableWhere(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, std::string columnsToDisplay/*=""*/, const std::string& where/*=""*/)
{
	std::vector<std::vector<std::string>> columnsData{};

	if (columnsToDisplay.empty())
	{
		columnsToDisplay = "*";
	}

	columnsData = GetTableData(db, zErrMsg, rc, table, columnsToDisplay, where);
	DisplayTableInConsole(columnsData);
}
void DisplayTableInnerJoin(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, const std::string& otherTable, std::string columnsToDisplay, const std::string& InnerJoin)
{
	std::string sqlQuery{};
	/*Create SQL statement*/
	if (columnsToDisplay.empty())
	{
		columnsToDisplay = "*";
	}
	sqlQuery += "SELECT " + columnsToDisplay + " FROM " + table;

	if (!InnerJoin.empty())
	{
		sqlQuery += " WHERE " + InnerJoin;
	}
	const char* sql = sqlQuery.c_str();

	DispayTableData(db, zErrMsg, rc, sql);
}

//---- Displaying data in console (These are used by the above displayTable functions)
//columnsToTDisplay will contain the columns that you want to be displayed
//Leave columnsToNOTDisplay empty if you want to display everything,
void DisplayTableInConsole(const std::vector<std::vector<std::string>>& _tableInformation, const std::vector<std::string>& columnsToDisplay)
{
	bool found{};

	const char cornerChar{ '+' };
	const char horizontalChar{ '-' };
	const char verticalChar{ '|' };
	const char FillerCharachter{ ' ' };

	const int paddingSpaces{ 2 };

	std::vector<std::vector<std::string>> tableInformation{ _tableInformation };

	std::string horizontalDividingLine{ "+" };
	std::string dataRow{};

	//Remove any columns that are not needed to be displayed
	if (!columnsToDisplay.empty())
	{
		for (int i{}; i < int(tableInformation.size()); i++)
		{
			for (int j{}; j < int(columnsToDisplay.size()); j++)
			{
				if (tableInformation[i][0] == columnsToDisplay[j])
				{
					found = true;
				}
			}
			if (!found)
			{
				tableInformation.erase(tableInformation.begin() + i);
				i--;
			}
			found = false;
		}
	}

	//Only initilise the vector here, cause we need to know the columns that will be shown
	std::vector<int>columnSizes(tableInformation.size());

	//Gettin the max length of the data entries in each column
	for (int i{}; i < int(tableInformation.size()); i++)
	{
		for (int j{}; j < int(tableInformation[i].size()); j++)
		{
			if (columnSizes[i] < (int(tableInformation[i][j].size()) + paddingSpaces))
			{
				columnSizes[i] = int(tableInformation[i][j].size() + paddingSpaces);
			}
		}
	}

	//Making the Horizontal dividing line
	for (int i{}; i < int(columnSizes.size()); i++)
	{
		horizontalDividingLine.append(columnSizes[i], horizontalChar);
		horizontalDividingLine += cornerChar;
	}

	//Print the first horizontal Dividing line
	std::cout << horizontalDividingLine << '\n';

	//Print out the data categories
	for (int i{}; i < int(tableInformation.size()); i++)
	{
		//Vertical line plus spacing
		dataRow += verticalChar;
		dataRow += FillerCharachter * (paddingSpaces / 2);

		//Category name
		dataRow += tableInformation[i][0];	//The first entry is, the category name

		//Filling characthers, if necessary
		dataRow.append((columnSizes[i] - int(tableInformation[i][0].size()) - paddingSpaces), FillerCharachter);

		//Padding at the back
		dataRow += FillerCharachter * (paddingSpaces / 2);
	}
	dataRow += verticalChar;
	std::cout << dataRow << '\n';
	dataRow = std::string{};

	//Print the second horizontal dividing line
	std::cout << horizontalDividingLine << '\n';

	//Print out the data
	for (int i{}; i < int(tableInformation[0].size()); i++)
	{
		if (i != 0)
		{
			for (int j{}; j < int(tableInformation.size()); j++)
			{
				//Vertical line plus spacing
				dataRow += verticalChar;
				dataRow += FillerCharachter * (paddingSpaces / 2);

				//The actual data
				dataRow += tableInformation[j][i];

				//Filling characthers, if necessary
				dataRow.append((columnSizes[j] - int(tableInformation[j][i].size()) - paddingSpaces), FillerCharachter);

				//Padding at the back
				dataRow += FillerCharachter * (paddingSpaces / 2);
			}
			dataRow += verticalChar;
			std::cout << dataRow << '\n';
			dataRow = std::string{};
		}
	}
}

//Display the entire table
void DisplayTableInConsole(const std::vector<std::vector<std::string>>& _tableInformation)
{
	bool found{};

	const char cornerChar{ '+' };
	const char horizontalChar{ '-' };
	const char verticalChar{ '|' };
	const char FillerCharachter{ ' ' };

	const int paddingSpaces{ 2 };

	std::string horizontalDividingLine{ "+" };
	std::string dataRow{};

	std::vector<std::vector<std::string>> tableInformation{ _tableInformation };
	std::vector<int>columnSizes(tableInformation.size());

	if (int(tableInformation.size()) <= 0)
	{
		std::cout << "No entries found matchhing your search \n";
		return;
	}

	//Gettin the max length of the data entries in each column
	for (int i{}; i < int(tableInformation.size()); i++)
	{
		for (int j{}; j < int(tableInformation[i].size()); j++)
		{
			if (columnSizes[i] < (int(tableInformation[i][j].size()) + paddingSpaces))
			{
				columnSizes[i] = int(tableInformation[i][j].size() + paddingSpaces);
			}
		}
	}

	//Making the Horizontal dividing line
	for (int i{}; i < int(columnSizes.size()); i++)
	{
		horizontalDividingLine.append(columnSizes[i], horizontalChar);
		horizontalDividingLine += cornerChar;
	}

	//Print the first horizontal Dividing line
	std::cout << horizontalDividingLine << '\n';

	//Print out the data categories
	for (int i{}; i < int(tableInformation.size()); i++)
	{
		//Vertical line plus spacing
		dataRow += verticalChar;
		dataRow += FillerCharachter * (paddingSpaces / 2);

		//Category name
		dataRow += tableInformation[i][0];	//The first entry is, the category name

		//Filling characthers, if necessary
		dataRow.append((columnSizes[i] - int(tableInformation[i][0].size()) - paddingSpaces), FillerCharachter);

		//Padding at the back
		dataRow += FillerCharachter * (paddingSpaces / 2);
	}
	dataRow += verticalChar;
	std::cout << dataRow << '\n';
	dataRow = std::string{};

	//Print the second horizontal dividing line
	std::cout << horizontalDividingLine << '\n';

	//Print out the data
	for (int i{}; i < int(tableInformation[0].size()); i++)
	{
		if (i != 0)
		{
			for (int j{}; j < int(tableInformation.size()); j++)
			{
				//Vertical line plus spacing
				dataRow += verticalChar;
				dataRow += FillerCharachter * (paddingSpaces / 2);

				//The actual data
				dataRow += tableInformation[j][i];

				//Filling characthers, if necessary
				dataRow.append((columnSizes[j] - int(tableInformation[j][i].size()) - paddingSpaces), FillerCharachter);

				//Padding at the back
				dataRow += FillerCharachter * (paddingSpaces / 2);
			}
			dataRow += verticalChar;
			std::cout << dataRow << '\n';
			dataRow = std::string{};
		}
	}
	std::cout << '\n';
}
void SortDataStringIntoVector(const std::string& _data, std::vector<std::vector<std::string>>& sortedVector)
{
	//const char startDelimiter{'id'};
	std::string endDelimiter{ "|" };
	std::string endCategoryDelimiter{ "=" };
	std::string EraseCategoryDelimiter{ "\n" };
	std::string data{ _data };
	std::string text{};
	std::string category{};
	std::string categoryData{};

	//std::string::size_type first{};
	std::string::size_type endOfDataEntry{};
	std::string::size_type end{};
	std::string::size_type first{};

	while ((data.find(endDelimiter)) != std::string::npos)
	{
		//Get the end
		endOfDataEntry = data.find(endDelimiter);

		text = data.substr(0, endOfDataEntry);		//Find the required string
		if (sortedVector.empty())		//Find the data categories and add them as the first entries in each column
		{
			while ((text.find(endCategoryDelimiter)) != std::string::npos)
			{
				end = text.find(endCategoryDelimiter);
				category = text.substr(0, end);
				sortedVector.push_back(std::vector<std::string>{category});

				end = text.find(EraseCategoryDelimiter);
				text.erase(0, end + EraseCategoryDelimiter.size());
			}
			text = data.substr(0, endOfDataEntry);		//ReFind the required string
		}

		for (int i{}; i < int(sortedVector.size()); i++)
		{
			first = text.find(endCategoryDelimiter);
			first += endCategoryDelimiter.size();
			end = text.find(EraseCategoryDelimiter);
			categoryData = text.substr(first, end - first);
			sortedVector[i].push_back(categoryData);

			text.erase(0, end + EraseCategoryDelimiter.size());
		}
		data.erase(0, endOfDataEntry + endDelimiter.size());			//erase the found string from the data
	}
}

//Update items in table
//Single Item
void UpdateItemInTable(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, const std::string& columnData, const std::string& column, const char* whereSql)
{
	const char* data = "Callback function called";
	std::string sqlQuery{};
	/*Create SQL statement*/
	sqlQuery = "UPDATE " + table + " SET " + column + " = " + columnData + " WHERE " + whereSql;

	const char* sql = sqlQuery.c_str();

	/*Execute SQL statement*/
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

	if (rc)
	{
		std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
	}
	else
	{
		//std::cout << "Opened database successfully \n" << std::endl;
	}
}

//Multiple items
void UpdateItemsInTable(sqlite3* db, char* zErrMsg, int& rc, const std::string& table, const std::vector<std::string>& columnData, const std::vector<std::string>& columns, const char* whereSql)
{
	const char* data = "Callback function called";
	std::string sqlQuery{};
	/*Create SQL statement*/
	for (int i{}; i < int(columnData.size()); i++)
	{
		UpdateItemInTable(db, zErrMsg, rc, table, columnData[i], columns[i], whereSql);
	}
}

//SQl callbacks
static int callback(void* data, int argc, char** argv, char** azColName)
{
	int columns{};

	std::cout << (const char*)data << std::endl;

	for (int i = 0; i < argc; i++)
	{
		std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
		columns++;
	}
	std::cout << std::endl;
	return 0;
}
static int callbackDisplayTable(void* data, int argc, char** argv, char** azColName)
{
	int columns{};
	char dataEntryDivider{ '|' };
	std::stringstream* text = static_cast<std::stringstream*>(data);

	//std::cout << (const char*)data << std::endl;

	for (int i = 0; i < argc; i++)
	{
		*text << azColName[i] << "=" << (argv[i] ? argv[i] : "NULL") << std::endl;
		columns++;
	}
	*text << std::endl;
	*text << dataEntryDivider;
	return 0;
}

//---- Input ----
std::string GetInputLine()
{
	std::string line;
	/*while (std::getline(std::cin, line))
	{
		std::cout << line << std::endl;
	}*/

	std::getline(std::cin, line);

	return line;
}

bool CheckInput(const std::string& input, std::vector<std::string> inputOptions)
{
	for (int i{}; i < int(inputOptions.size()); i++)
	{
		if (inputOptions[i] == input)
		{
			return true;
		}
	}
	std::cout << "Input invalid \n";
	return false;
}

//---- Misc ----
float RandomFloatValue(const float start, const float end)
{
	//Seeding rand
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<>dis(0, RAND_MAX);

   const float range = end - start;

   float random = static_cast<float>(dis(gen));
   float randomValueInRange = ((random / RAND_MAX) * range) + start;

   return randomValueInRange;
}
int RandomIntValue(const int start, const int end)
{
	return int(RandomFloatValue(float(start), float(end)));
}
std::string RandomIP4Adress()
{
	int firstByte{ RandomIntValue(0, 255) };
	int secondByte{ RandomIntValue(0, 255) };
	int thirdByte{ RandomIntValue(0, 255) };
	int fourthByte{ RandomIntValue(0, 255) };

	std::string randomIp4Adress{ std::to_string(firstByte) + '.' + std::to_string(secondByte) + '.' +  std::to_string(thirdByte) + '.' + std::to_string(fourthByte) };

	return randomIp4Adress;
}
