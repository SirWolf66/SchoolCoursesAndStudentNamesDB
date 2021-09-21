// retake practise.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

//SQLite library
#include "SQL_Library/sqlite3.h"

//Catch2 library
#define CATCH_CONFIG_RUNNER
#include "Catch2/catch.hpp"

//The helperfunctions
#include "HelperFunctions.h"
#include "schooldbSQLoperations.h"

//Forward declerations
void ManualInput(sqlite3* db, char* zErrMsg, int& rc, std::string& data);
void InputHandeling(sqlite3* db, char* zErrMsg, int& rc, std::string& input, const std::string& category, std::vector<std::string> inputOptions);

int main()
{
    const char* sql{};
    char* zErrMsg = 0;
    int rc;
    std::string text{};
    std::string data;
    sqlite3* db{};

    #pragma region Catch2
    Catch::Session().run();
    #pragma endregion


#pragma region Connect to a database
    rc = sqlite3_open("school.db", &db);

    if (rc)
    {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 0;
    }
    else
    {
        std::cout << "Opened database succesfully \n" << std::endl;
    }
#pragma endregion

    // ---REMEMBER--- foreign keys are NOT activated by DEFAULT
    sql =
        "PRAGMA foreign_keys = ON;";
    GeneralExecuteSQLStatement(db, zErrMsg, rc, sql);

#pragma region Table creation
    //Creating the student table
    sql =
        "CREATE TABLE students("
        "id            INTEGER PRIMARY KEY NOT NULL,"
        "firstName     VARCHAR,"
        "lastName      VARCHAR,"
        "studentId     VARCHAR UNIQUE"
        ")";
    CreateTable(db, zErrMsg, rc, sql);

    //Creating the Courses table
    sql =
        "CREATE TABLE courses("
        "id         INTEGER PRIMARY KEY NOT NULL,"
        "name       VARCHAR"
        ")";
    CreateTable(db, zErrMsg, rc, sql);

    //Create the Results table
    sql =
        "CREATE TABLE results("
        //"id         INTEGER PRIMARY KEY NOT NULL,"
        "studentId  INTEGER NOT NULL,"
        "courseId  INTEGER NOT NULL,"
        "firstTake  INTEGER DEFAULT 0,"
        "reTake     INTEGER DEFAULT 0,"
        "CONSTRAINT houseItemsPK PRIMARY KEY(studentId, courseId)," //Adding a Primary key that consist out of the studentId and the CourseId, this makes sure a student has only 1 entry for a course
        "CONSTRAINT fk_student_id "
        "FOREIGN KEY (studentId) "
        "REFERENCES students(id) "
            "ON DELETE CASCADE "
            "ON UPDATE CASCADE,"
        "CONSTRAINT fk_courses_id "
        "FOREIGN KEY (courseId) "
        "REFERENCES courses(id) "
            "ON DELETE CASCADE "
            "ON UPDATE CASCADE"
        ")";
    CreateTable(db, zErrMsg, rc, sql);
#pragma endregion

#pragma region Adding some initial data
    //std::string data;
    //students  (first name, last name, studentid)
  /*  data = "John,Doe";
    AddStudent(db, zErrMsg, rc, data);
    data = "Jane,Doe";
    AddStudent(db, zErrMsg, rc, data);*/
    ////Courses   (name)
    //data = "Math";
    //AddCourse(db, zErrMsg, rc, data);
    //data = "English";
    //AddCourse(db, zErrMsg, rc, data);

    ////Results   (studentid, courseid, first take, retake)
    //data = "1, 1, 0, 10";
    //AddResults(db, zErrMsg, rc, data);
    //data = "2, 1, 20, 0";
    //AddResults(db, zErrMsg, rc, data);
    //data = "2, 1, 20, 0";
    //AddResults(db, zErrMsg, rc, data);
    //data = "2, 1, 20, 0";
    //AddResults(db, zErrMsg, rc, data);
    //data = "3, 1, 20, 0";
    //AddResults(db, zErrMsg, rc, data);
    //data = "1, 2, 6, 15";
    //AddResults(db, zErrMsg, rc, data);
#pragma endregion

    #pragma region Testing
    std::vector<std::vector<std::string>> columnsData;
    //Getting the data
    columnsData = GetTableData(db, zErrMsg, rc, "students", "firstName,lastName");
    //Displaying the data
    DisplayTableWhere(db, zErrMsg, rc, "students", "firstName,lastName", "firstName == 'John'");

    //File reading
    std::string input{};
    input = ReadCSVFile(db, zErrMsg, rc, "Resources/NotMath2.csv");

    input;
    #pragma endregion


#pragma region User Input
    while (true)
    {
        ManualInput(db, zErrMsg, rc, data);
    }
#pragma endregion
}

void ManualInput(sqlite3* db, char* zErrMsg, int& rc, std::string& data)
{ 
    std::string category;
    std::string input;
    std::vector<std::string> categoryOptions{ "Add", "Modify", "Display" };

    do
    {
        std::cout << '\n';
        std::cout << "---- What would you like do do? ---- \n";
        for (int i{}; i < int(categoryOptions.size()); i++)
        {
            std::cout << "-Type \"" << categoryOptions[i] << "\" to " + categoryOptions[i] +" data\n";
        }
        category = input = GetInputLine();
    } while (!CheckInput(category, categoryOptions));

    if (categoryOptions[0] == category) //Adding data
    {
        std::cout << "---- Choose Option ---- \n";
        std::cout << "What would you like to add? \n";
        std::cout << "-Add student in the format \"First name,Last name\": \n";
        std::cout << "-Add course in the format \"Course name\": \n";
        input = GetInputLine();

        if (CheckStudentInputData(input))
        {
            AddStudent(db, zErrMsg, rc, input);
            std::cout << "-Student added";
        }
        else if (CheckCourseInputData(input))
        {
            AddCourse(db, zErrMsg, rc, input);
            std::cout << "-Course added";
        }
        else
        {
            std::cout << "*Invalid input";
        }
        std::cout << '\n';
    }
    else if (categoryOptions[1] == category)    //Modifying data
    {
        std::cout << "---- Choose Option ---- \n";
        std::cout << "What would you like to modify? \n";
        std::cout << "-Type \"First name,Last name,course,examPeriod,score\" to Modify the score of a student \n";
        input = GetInputLine();

        if (CheckModifyScoreInputData(input))
        {
            ModifyStudentScores(db, zErrMsg, rc, input);
        }
        else
        {
            std::cout << "Invalid input";
        }
    }
    else if (categoryOptions[2] == category)    //Displaying data
    {
        std::cout << "---- Choose Option ---- \n";
        std::cout << "Display data of: \n";
        std::cout << "-Type \"Courses\" to display courses \n";
        std::cout << "-Type \"Students\" to display students \n";
        std::cout << "-Type \"First name,Last name\" to display the results of a student \n";
        std::cout << "-Type \"Course name\" to display the average scores of a subject \n";
        input = GetInputLine();

        if (input == "Courses")
        {
            DisplayCourses(db, zErrMsg, rc);
        }
        else if (input == "Students")
        {
            DisplayStudents(db, zErrMsg, rc);
        }
        else if (CheckStudentInputData(input))
        {
            DisplayStudentScores(db, zErrMsg, rc, input);
        }
        else if (CheckCourseInputData(input))
        {
            DisplayAverageScoreOfCourse(db, zErrMsg, rc, input);
        }
        else
        {
            std::cout << "Invalid input";
        }
    }
    else
    {
        std::cout << "Input error";
    }
}

void InputHandeling(sqlite3* db, char* zErrMsg, int& rc, std::string& input, const std::string& category, std::vector<std::string> inputOptions)
{
    if (inputOptions[0] == category)
    {
        std::cout << "---- Choose Option ---- \n";
        std::cout << "-Add student in the format \"First name,Last name\": \n";
        std::cout << "-Add course in the format \"Course name\": \n";

        if (CheckStudentInputData(input))
        {
            AddStudent(db, zErrMsg, rc, input);
            std::cout << "-Student added";
        }
        else if (CheckCourseInputData(input))
        {
            AddCourse(db, zErrMsg, rc, input);
            std::cout << "-Course added";
        }
        else
        {
            std::cout << "*Invalid input";
        }
        std::cout << '\n';
    }
    else if (inputOptions[1] == category)
    {

    }
    else if (inputOptions[2] == category)
    {
        std::cout << "---- Choose Option ---- \n";
        std::cout << "-Type \"Courses\" to display courses \n";
        std::cout << "-Type \"Students\" to display students \n";

         if (input == "Courses")
         {
             DisplayCourses(db, zErrMsg, rc);
         }
         else if (input == "Students")
         {
             DisplayStudents(db, zErrMsg, rc);
         }
    }
    else
    {
        std::cout << "Input error";
    }
}

