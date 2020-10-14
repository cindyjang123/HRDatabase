#include <iostream>
#include <occi.h>
#include <iomanip>

using oracle::occi::Connection;
using oracle::occi::Environment;
using namespace oracle::occi;
using namespace std;

//structure Employee set to safe empty state
struct Employee
{
    int employeeNumber = 0;
    string lastName = "";
    string firstName = "";
    string email = "";
    string phone = "";
    string extension = "";
    string reportsTo = "";
    string jobTitle = "";
    string city = "";
};

//displays the menu options
void displayMenu()
{
    cout << endl;
    cout << "********************* HR Menu *********************" << endl;
    cout << "1) Find Employee" << endl;
    cout << "2) Employees Report" << endl;
    cout << "3) Add Employee" << endl;
    cout << "4) Update Employee" << endl;
    cout << "5) Remove Employee" << endl;
    cout << "0) Exit" << endl;
    cout << endl;
}

//gets a selection from user by looping until a valid selection is made
int getOption()
{
    int select = 0;
    bool done = false;
    cout << "Please choose an option: ";
    while (!done)
    {
        cin >> select;
        cin.ignore();
        if (cin.fail())
        {
            cin.clear();
            cout << "Invalid selection, try again: ";
            cin.ignore(100, '\n');
        }
        else if (select < 0 || select > 5)
        {
            cout << "Selection does not exist. Enter a number between 0 and 5: ";
        }
        else
        {
            done = true;
        }
    }
    return select;
};

// displays the menu and gets a valid selection from user
int menu()
{
    displayMenu();
    return getOption();
}

//gets an employee ID from user by using loop until valid number is given
int getID()
{
    bool success = false;
    int select;
    cout << "Enter the employee number to search for: ";
    do
    {
        cin >> select;
        if (cin.fail())
        {
            cin.clear();
            cout << "Not a valid number, please input a valid employee number: ";
            cin.ignore(100, '\n');
        }
        else
        {
            success = true;
        }
    } while (!success);
    return select;
}

//saves the employee's data into struct employee based on employee number provided in parameter
bool findEmployee(Connection *conn, int employeeNumber, Employee *emp)
{
    bool success = false;
    try
    {
        string query = "SELECT emp.employeenumber, emp.lastname, emp.firstname, emp.email, off.phone, emp.extension, mng.firstname || ' ' || mng.lastname, emp.jobtitle, off.city FROM dbs211_employees emp LEFT JOIN dbs211_employees mng ON emp.reportsto = mng.employeenumber LEFT JOIN dbs211_offices off ON emp.officecode = off.officecode WHERE emp.employeenumber = ";
        query += to_string(employeeNumber);
        Statement *stmt = conn->createStatement(query);
        ResultSet *rs = stmt->executeQuery();
        if (rs->next())
        {
            emp->employeeNumber = rs->getInt(1);
            emp->lastName = rs->getString(2);
            emp->firstName = rs->getString(3);
            emp->email = rs->getString(4);
            emp->phone = rs->getString(5);
            emp->extension = rs->getString(6);
            emp->reportsTo = rs->getString(7);
            emp->jobTitle = rs->getString(8);
            emp->city = rs->getString(9);
            success = true;
        }
        delete rs;
        conn->terminateStatement(stmt);
    }
    catch (SQLException &sqlExcp)
    {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage() << endl;
    }
    return success;
}

//gets user input for employee number, recalls the employee's data saved into employee struct and displays the info
void displayEmployee(Connection *conn, Employee *emp)
{
    int id = getID();
    if (!findEmployee(conn, id, emp))
    {
        cout << endl;
        cout << "Employee " << id << " does not exist." << endl;
    }
    else
    {
        cout << "employeeNumber = " << emp->employeeNumber << endl;
        cout << "lastName = " << emp->lastName << endl;
        cout << "firstName = " << emp->firstName << endl;
        cout << "email = " << emp->email << endl;
        cout << "phone = " << emp->phone << endl;
        cout << "extension = " << emp->extension << endl;
        cout << "reportsTo = " << emp->reportsTo << endl;
        cout << "jobTitle = " << emp->jobTitle << endl;
        cout << "city = " << emp->city << endl;
        cout << endl;
    }
}
//displays all current employee data in the database by doing a sql query
void displayAllEmployees(Connection *conn)
{
    try
    {
        Statement *stmt = conn->createStatement("SELECT emp.employeenumber, emp.firstname || ' ' || emp.lastname, emp.email, off.phone, emp.extension, mng.firstname || ' ' || mng.lastname FROM dbs211_employees emp LEFT JOIN dbs211_employees mng ON emp.reportsTo = mng.employeeNumber LEFT JOIN dbs211_offices off ON emp.officeCode = off.officeCode ORDER BY employeeNumber");
        ResultSet *rs = stmt->executeQuery();

        if (!rs->next())
        {
            cout << "There is no employees' information to be displayed." << endl;
        }
        else
        {
            cout << left << setw(6) << " E";
            cout << left << setw(19) << " Employee Name";
            cout << left << setw(34) << " Email";
            cout << left << setw(18) << " Phone";
            cout << left << setw(8) << " Ext";
            cout << left << setw(21) << " Manager" << endl;
            cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
            do
            {
                cout << right << setw(6) << rs->getInt(1);
                cout << right << setw(19) << rs->getString(2);
                cout << right << setw(34) << rs->getString(3);
                cout << right << setw(18) << rs->getString(4);
                cout << right << setw(8) << rs->getString(5);
                cout << right << setw(21) << rs->getString(6) << endl;

            } while (rs->next());
        }
        delete rs;
        conn->terminateStatement(stmt);
    }
    catch (SQLException &sqlExcp)
    {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage() << endl;
    }
}

//Add a new employee
void insertEmployee(Connection *conn, struct Employee *emp)
{

    cout << "Employee Number: ";
    cin >> emp->employeeNumber;
    cout << "Last Name: ";
    cin >> emp->lastName;
    cout << "First Name: ";
    cin >> emp->firstName;
    cout << "Email: ";
    cin >> emp->email;
    cout << "extension: ";
    cin >> emp->extension;
    cout << "Job Title: ";
    cin >> emp->jobTitle;
    cout << "City: ";
    cin >> emp->city;

    if (findEmployee(conn, emp->employeeNumber, emp))
    {
        cout << "An employee with the same employee number exists." << endl;
    }
    else
    {

        try
        {
            Statement* stmt = conn->createStatement("
			INSERT INTO dbs211_employees (employeenumber, lastname, firstname, extension, email, officecode, reportsto, jobtitle) VALUES (" 
			+ to_string(emp->employeeNumber) + ", '" + emp->lastName + "', '" + emp->lastName +  "', '" + emp->firstName +  "', '" + emp->extension + "', '" + emp->email +  "', 1, 1002, '" + emp->jobTitle + "')");
			ResultSet* rs = stmt->executeQuery();
			conn->commit();
			cout << "The new employee is added successfully." << endl;
			delete rs;
			conn->terminateStatement(stmt);
        }
        catch (SQLException &sqlExcp)
        {
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage() << endl;
        }
    }
}

//Updates a new employee
void updateEmployee(Connection *conn, int employeeNumber)
{
    struct Employee *emp;
    if (findEmployee(conn, employeeNumber, emp))
    {
        cout << "Enter the new employee extension: ";
        cin >> emp->extension;
        cin.clear();
        cin.ignore(100, '\n');

        try
        {
            Statement* stmt = conn->createStatement("UPDATE dbs211_employees SET extension = '"+ emp->extension + "'WHERE employeenumber = " + to_string(emp->employeeNumber);
			ResultSet* rs = stmt->executeQuery();
			conn->commit();
			cout << "The employee is updated." << endl;
			delete rs;
        	conn->terminateStatement(stmt);
        }
        catch (SQLException &sqlExcp)
        {
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage() << endl;
        }
    }
    else
    {
        cout << "The employee does not exist" << endl;
    }
}

//Deletes an existing employee
void deleteEmployee(Connection *conn, int employeeNumber)
{
    struct Employee *emp;
    if (findEmployee(conn, employeeNumber, emp))
    {
        cout << "The employee does not exist." << endl;
    }
    else
    {
        try
        {
            Statement *stmt = conn->createStatement("DELETE FROM dbs211_ employees WHERE employeenumber = " + to_string(employeeNumber));
            ResultSet *rs = stmt->executeQuery();
            conn->commit();
            cout << "The employee is deleted." << endl;
            delete rs;
            conn->terminateStatement(stmt);
        }
        catch (SQLException &sqlExcp)
        {
            cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage() << endl;
        }
    }
}

int main(void)
{
    Environment *env = nullptr;
    Connection *conn = nullptr;
    string usr = "";
    string pass = "";
    string srv = "myoracle12c.senecacollege.ca:1521/oracle12c";

    //connect to database
    try
    {
        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(usr, pass, srv);
        cout << "successfully connected" << endl;
    }
    // in case the connection fails
    catch (SQLException &sqlExcp)
    {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage() << endl;
    }
    //create a struct pointer emp.
    Employee *emp = new Employee();
    int select, id;

    //a loop of switch case until user selects "0) Exit"
    do
    {
        select = menu();
        switch (select)
        {
        case 1:
            displayEmployee(conn, emp);
            break;
        case 2:
            displayAllEmployees(conn);
            break;
        case 3:
            insertEmployee(conn, emp);
            break;
        case 4:
            id = getID();
            updateEmployee(conn, id);
            break;
        case 5:
            id = getID();
            deleteEmployee(conn, id);
            break;
        }
    } while (select != 0);

    //disconnects from database safely
    env->terminateConnection(conn);
    Environment::terminateEnvironment(env);

    return 0;
}
