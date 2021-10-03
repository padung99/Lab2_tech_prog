#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <atlstr.h>

#define OK_BUTTON 1
#define MAX 100

using namespace std;
HWND hUsers;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddControl(HWND);
wchar_t user_name[MAX];

vector<string> name;
int flag = 0;
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	WNDCLASSW wc = { 0 };
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;

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

	//MessageBox(NULL, L"Hello", L"msg", MB_OK);
	if (!RegisterClassW(&wc))
		return -1;
	
	CreateWindowW(L"myWindowClass",
		L"My Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		100, 100, 500, 500, NULL, NULL, NULL, NULL);
	
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);  
	}
	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		AddControl(hWnd);
	case WM_COMMAND:
		switch (wp)
		{	
			case OK_BUTTON:
				GetWindowText(hUsers, user_name, MAX);

				wstring ws(user_name); 
				string str(ws.begin(), ws.end()); //convert wchar_t to string 
				if(find(name.begin(), name.end(), str) != name.end())
					MessageBox(NULL, user_name, L"msg", MB_OK);

		}
	default: 
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

void AddControl(HWND hWnd)
{
	CreateWindowW(L"static", //class static to print lable
		L"Enter user: ",
		WS_VISIBLE | WS_CHILD,
		200, 100, 100, 50,
		hWnd,
		NULL, NULL, NULL);
	hUsers = CreateWindowW(L"edit", //class edit to get input string
		NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER |ES_MULTILINE |ES_AUTOVSCROLL |ES_AUTOHSCROLL,
		200, 155, 100, 50,
		hWnd,
		NULL, NULL, NULL);
	CreateWindowW(L"button",
		L"OK",
		WS_VISIBLE | WS_CHILD,
		200, 204, 100, 50,
		hWnd,
		(HMENU)OK_BUTTON, NULL, NULL);
}

