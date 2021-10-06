#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sqlite3.h> 
#include <sstream>
#include <wchar.h>
#include <time.h>

#define OK_BUTTON 1
#define MAX 100
#define CONFIRM_BUTTON 2
#define MAX_BUTTON 5
#define ID_TIMER 1
#define SET_TIMER 10
using namespace std;

ofstream SaveData;
ifstream InputData;

std::ostringstream temp;
std::string command;
string tmp;

HWND hUsers, hLogin, hApp;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowProcedure_app(HWND, UINT, WPARAM, LPARAM);

void AddControl(HWND);
void AddControl_app(HWND);
wchar_t user_name[MAX];

vector<string> name;

int cnt;

sqlite3* db;
sqlite3_stmt* stmt;
char* zErrMsg = 0;
int rc;
const char* sql;
const char* data = "Callback function called";

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		tmp = (argv[i]);
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

	}
	printf("\n");
	return 0;
}

HDC hdc;
PAINTSTRUCT ps;
RECT r;
char szBuffer[200];
int timer;
int seconds = 0;
int minutes = 0;
int hours = 0;

//Using database to save data (library sqlite)

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	//cnt = MAX_BUTTON;
	WNDCLASSW wc = { 0 };
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;

	WNDCLASSW wc_app = { 0 };
	wc_app.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc_app.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc_app.hInstance = hInst;
	wc_app.lpszClassName = L"myWindowClass_app";
	wc_app.lpfnWndProc = WindowProcedure_app;

	string line;
	ifstream myfile;

	const WCHAR SavingUsers[] = L"C:\\Users\\dungphan16499\\source\\repos\\lab2_TechProg_AppUsers\\UsersName.txt";

	myfile.open("C:\\Users\\dungphan16499\\source\\repos\\lab2_TechProg_AppUsers\\UsersName.txt");

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			name.push_back(line);
			cout << line << '\n';
		}
		myfile.close();
	}
	else
		cout << "Unable to open file";

	/* Open database */
	rc = sqlite3_open("test.db", &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}


	/* Create SQL statement */
	sql = "CREATE TABLE USERS("  \
		"NAME TEXT PRIMARY KEY     NOT NULL," \
		"TIMER            INT    NOT NULL);";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Table created successfully\n");
	}

	/* Create SQL statement */

	//temp << "INSERT or IGNORE INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) VALUES (" << _id << ", '" << _name << "', " << _age << ", '" << _address << "', " << _salary << ")";
	for (int k = 0; k < name.size(); k++)
	{
		temp.str("");
		temp << "INSERT or IGNORE INTO USERS (NAME,TIMER) VALUES ('" << name[k] << "', " << SET_TIMER << ")";
		command = temp.str();
		rc = sqlite3_exec(db, command.c_str(), callback, 0, &zErrMsg);
	}


	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Records created successfully\n");
	}
	//sqlite3_close(db);


	if (!RegisterClassW(&wc))
		return -1;

	if (!RegisterClassW(&wc_app))
		return -1;

	hLogin = CreateWindowW(L"myWindowClass",
		L"Time-limited",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 400, 200, NULL, NULL, NULL, NULL);


	MSG msg = { 0 };
	MSG msg_app = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	while (GetMessage(&msg_app, NULL, NULL, NULL))
	{
		TranslateMessage(&msg_app);
		DispatchMessage(&msg_app);
	}
	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CLOSE:
		exit(0);
		//sqlite3_close(db);
		//PostQuitMessage(0);
		break;
	case WM_CREATE:
		AddControl(hWnd);
	case WM_COMMAND:
		switch (wp)
		{
		case OK_BUTTON:
			GetWindowTextW(hUsers, user_name, MAX);
			//rc = sqlite3_open("test.db", &db);
			wstring ws(user_name);
			string str1(ws.begin(), ws.end()); //convert wchar_t to string 
			//cnt = MAX_BUTTON;
			ShowWindow(hLogin, SW_HIDE);
			if (find(name.begin(), name.end(), str1) != name.end())
			{
				temp.str("");
				temp << "SELECT * from USERS where NAME =  '" << str1 << "'";

				command = temp.str();
				sqlite3_prepare_v2(db, command.c_str(), -1, &stmt, 0);

				sqlite3_step(stmt);
				timer = sqlite3_column_int(stmt, 1); //take TIMER of each user from database
				if (timer == 0)
					timer = SET_TIMER;

				wchar_t buffer1[MAX] = L"Wellcome ";
				//wchar_t* ptr;
				wcscat(buffer1, user_name);
				MessageBoxW(NULL, buffer1, L"Users", MB_OK);
				hApp = CreateWindowW(L"myWindowClass_app", //Application window
					L"Time-limited",
					WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					150, 150, 300, 300, NULL, NULL, NULL, NULL);
				 //Hide Login window in "using" mode
				
				//sqlite3_close(db);
			}
			else
			{
				wchar_t head[MAX] = L"User ";
				wchar_t tail[MAX] = L" does not exist";
				wcscat(head, user_name);
				wcscat(head, tail);
				MessageBoxW(NULL, head, L"Users", MB_ICONHAND);
				ShowWindow(hLogin, SW_SHOW);
			}

		}
	
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

LRESULT CALLBACK WindowProcedure_app(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		ShowWindow(hLogin, SW_SHOW);  //Show Login window after turn off application window
		//KillTimer(hWnd, ID_TIMER);
		//return 0;
		//exit(0);
		//PostQuitMessage(1);
		break;
	case WM_CREATE:
		SetTimer(hWnd, ID_TIMER, 1000, NULL);
		AddControl_app(hWnd);
		break;
	//case WM_COMMAND:
	//	switch (wp)
	//	{
	//	case CONFIRM_BUTTON:
			////cnt = MAX_BUTTON;
			//cnt--;
			//wstring ws(user_name);
			//string str1(ws.begin(), ws.end());
			////rc = sqlite3_open("test.db", &db);

			//temp.str("");
			//temp << "UPDATE USERS set NUMBER  = " << cnt << "  where NAME = '" << str1 << "'";
			//command = temp.str();
			//rc = sqlite3_exec(db, command.c_str(), callback, 0, &zErrMsg);

			//if (cnt == 0)
			//	DestroyWindow(hApp);

	//	}
	case WM_TIMER:
	{
		timer--;
		InvalidateRect(hWnd, NULL, TRUE);
		wstring ws(user_name);
		string str1(ws.begin(), ws.end());
		//rc = sqlite3_open("test.db", &db);

		temp.str("");
		temp << "UPDATE USERS set TIMER  = " << timer << "  where NAME = '" << str1 << "'";
		command = temp.str();
		rc = sqlite3_exec(db, command.c_str(), callback, 0, &zErrMsg);
	}

	break;

	case WM_PAINT: //
			if (timer > 0)
			{
				hdc = BeginPaint(hWnd, &ps);
				GetClientRect(hWnd, &r);
				hours = timer / 3600;
				minutes = (timer / 60) % 60;
				seconds = timer % 60;
				wsprintf(szBuffer, "%d: %d: %d", hours, minutes, seconds);
				DrawTextA(hdc, szBuffer, -1, &r, DT_CENTER);
				EndPaint(hWnd, &ps);
			}
			else if (timer <= 0)
				DestroyWindow(hWnd);
				//SendMessage(hApp, WM_CLOSE, 0, 0L);
	break;

	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

void AddControl(HWND hWnd)
{
	CreateWindowW(L"static", //Using class "static" to print lable
		L"Enter user: ",
		WS_VISIBLE | WS_CHILD,
		90, 60, 80, 40,
		hWnd,
		NULL, NULL, NULL);
	hUsers = CreateWindowW(L"edit", //Using class "edit" to get input string
		NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		190, 60, 100, 30,
		hWnd,
		NULL, NULL, NULL);
	CreateWindowW(L"button",
		L"OK",
		WS_VISIBLE | WS_CHILD,
		200, 100, 40, 20,
		hWnd,
		(HMENU)OK_BUTTON, NULL, NULL);
}


void AddControl_app(HWND hWnd)
{
	CreateWindowW(L"static", //Using class "static" to print lable
		L"Time limited",
		WS_VISIBLE | WS_CHILD,
		110, 50, 200, 50,
		hWnd,
		NULL, NULL, NULL);

	CreateWindowW(L"button",
		L"Confirm",
		WS_VISIBLE | WS_CHILD,
		110, 70, 70, 30,
		hWnd,
		(HMENU)CONFIRM_BUTTON, NULL, NULL);

}