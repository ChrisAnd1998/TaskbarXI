// TaskbarXI for Windows 11.
// By Chris Andriessen 2022
// https://github.com/ChrisAnd1998/TaskbarXI

#pragma comment (lib, "dwmapi.lib")

#define WIN32_LEAN_AND_MEAN
#define NOT_BUILD_WINDOWS_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread>
#include <dwmapi.h>

//Notifyicon
#include <shellapi.h>
#include "resource.h"
#define APPWM_ICONNOTIFY (WM_APP + 1)
#define WM_NOTIFY_TB 3141
NOTIFYICONDATA nid = {};

std::string cur_dir;
std::string cur_cmd;

int working;

BOOL CALLBACK EnumCallbackTaskbars(HWND hWND, LPARAM lParam);
BOOL CALLBACK EnumCallbackMaximized(HWND hWND, LPARAM lParam);
BOOL CALLBACK EnumCallbackInstances(HWND hWND, LPARAM lParam);

HWND maximized_List[10];
HWND taskbar_List[10];
int taskbar_Count;
int maximized_Count;

int mtaskbar_Revert;
int staskbar_Revert;

int trayleft;

int isstore;

void initTray(HWND parent);
VOID SetTaskbar();

//Settings
int square;
int ignoremax;
int notray;
int hidetraywnd;
int stop;
int createstartup;
int removestartup;
int sticky;

//VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
//{
//
//    if (working == 0) {
//       SetTaskbar();
//    }
//
//}

// MessageBox(NULL, L"Tray icon double clicked!", L"clicked", MB_OK);

void exiting() {
	std::wcout << "Exiting TaskbarXI..." << std::endl;

	Shell_NotifyIcon(NIM_DELETE, &nid);

	for (HWND tb : taskbar_List) {
		if (tb != 0) {
			RECT rect_tb;
			GetWindowRect(tb, &rect_tb);

			INT curDPI = GetDpiForWindow(tb) * 1.041666666666667;

			HRGN region_Empty = CreateRectRgn(abs(rect_tb.left - rect_tb.left) * curDPI / 100, 0, abs(rect_tb.right - rect_tb.left) * curDPI / 100, rect_tb.bottom * curDPI / 100);
			SetWindowRgn(tb, region_Empty, TRUE);
		}
	}

	exit(0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case APPWM_ICONNOTIFY:
	{
		switch (lParam)
		{
		case WM_LBUTTONUP:
			if (MessageBox(NULL, L"Do you want to EXIT TaskbarXI?", L"TaskbarXI", MB_YESNO) == IDYES)
			{
				exiting();
			}
			else {
				//continue
			}
			break;
		}
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void create_startup() {
	if (isstore == 0) {
		std::wstring chars = L"";
		chars += (wchar_t)34;
		std::string quote(chars.begin(), chars.end());

		std::string path = quote;
		path.append(cur_dir);
		path.append(quote);
		path.append(cur_cmd);
		HKEY hkey = NULL;
		LONG createStatus = RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
		LONG status = RegSetValueExA(hkey, "TaskbarXI", 0, REG_SZ, (LPBYTE)path.c_str(), (path.size() + 1) * sizeof(wchar_t));
		RegCloseKey(hkey);
	}
	if (isstore == 1) {
		std::string storepath = "Explorer.exe taskbarxi:";
		std::string path = storepath.append(cur_cmd);
		HKEY hkey = NULL;
		LONG createStatus = RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
		LONG status = RegSetValueExA(hkey, "TaskbarXI", 0, REG_SZ, (LPBYTE)path.c_str(), (path.size() + 1) * sizeof(wchar_t));
		RegCloseKey(hkey);
	}
}

void remove_startup() {
	HKEY hkey = NULL;
	LONG createStatus = RegOpenKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
	LONG status = RegDeleteValue(hkey, L"TaskbarXI");
	RegCloseKey(hkey);
	exit(0);
}

//int main(int argc, char* argv[])
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FreeConsole();

	// look for commandline
	LPWSTR* szArgList;
	int argCount;

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

	for (int i = 0; i < argCount; i++)
	{
		if (wcscmp(szArgList[i], L"-stop") == 0) {
			stop = 1;
			//cur_cmd.append(" -stop");
		}
		if (wcscmp(szArgList[i], L"-square") == 0) {
			square = 1;
			cur_cmd.append(" -square");
		}
		if (wcscmp(szArgList[i], L"-ignoremax") == 0) {
			ignoremax = 1;
			cur_cmd.append(" -ignoremax");
		}
		if (wcscmp(szArgList[i], L"-notray") == 0) {
			notray = 1;
			cur_cmd.append(" -notray");
		}
		if (wcscmp(szArgList[i], L"-hidetraywnd") == 0) {
			hidetraywnd = 1;
			cur_cmd.append(" -hidetraywnd");
		}
		if (wcscmp(szArgList[i], L"-sticky") == 0) {
			sticky = 1;
			cur_cmd.append(" -sticky");
		}
		if (wcscmp(szArgList[i], L"-createstartup") == 0) {
			createstartup = 1;
			//cur_cmd.append(" -createstartup");
		}
		if (wcscmp(szArgList[i], L"-removestartup") == 0) {
			removestartup = 1;
			//cur_cmd.append(" -removestartup");
		}
		if (wcscmp(szArgList[i], L"-console") == 0) {
			AllocConsole();
			FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;
			//freopen_s(&fpstdin, "CONIN$", "r", stdin);
			freopen_s(&fpstdout, "CONOUT$", "w", stdout);
			freopen_s(&fpstderr, "CONOUT$", "w", stderr);
		}
	}

	LocalFree(szArgList);

	EnumWindows(EnumCallbackInstances, NULL);

	working = 1;
	//SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);
	//SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_DESTROY, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);
	//SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, NULL, WinEventProcDestroy, 0, 0, WINEVENT_SKIPOWNPROCESS);
	//SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);

	SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

	std::wcout << "Initializing..." << std::endl;

	HWND Explorer = NULL;

	do
	{
		std::wcout << "Looking for Explorer..." << std::endl;
		Explorer = FindWindow(L"Shell_TrayWnd", 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (Explorer == 0);

	std::wcout << "Explorer found!" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	//Setup Notifyicon
	MSG msg;

	WNDCLASSEX wnd = { 0 };

	wnd.hInstance = hInstance;
	wnd.lpszClassName = L"TaskbarXI";
	wnd.lpfnWndProc = WndProc;
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.cbSize = sizeof(WNDCLASSEX);

	wnd.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hbrBackground = (HBRUSH)BLACK_BRUSH;
	RegisterClassEx(&wnd);

	HWND tray_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, L"TaskbarXI", L"TrayWindow", WS_OVERLAPPEDWINDOW, 0, 0, 400, 400, NULL, NULL, hInstance, NULL);

	nid.cbSize = sizeof(nid);
	nid.hWnd = tray_hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = APPWM_ICONNOTIFY;
	nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wcscpy_s(nid.szTip, L"TaskbarXI");

	Shell_NotifyIcon(NIM_DELETE, &nid);

	//ShowWindow(tray_hwnd, WM_SHOWWINDOW);
	//End setup Notifyicon

	Explorer = NULL;

	std::wcout << "Looking for taskbars..." << std::endl;

	// Find all taskbar(s)
	taskbar_Count = 0;
	taskbar_List[0] = 0;
	taskbar_List[1] = 0;
	taskbar_List[2] = 0;
	taskbar_List[3] = 0;
	taskbar_List[4] = 0;
	taskbar_List[5] = 0;
	taskbar_List[6] = 0;
	taskbar_List[7] = 0;
	taskbar_List[8] = 0;
	taskbar_List[9] = 0;

	EnumWindows(EnumCallbackTaskbars, NULL);

	if (stop == 1) {
		exiting();
	}

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	cur_dir = std::string(buffer);// (argv[0]);

	if (cur_dir.find("40210ChrisAndriessen") != std::string::npos) {
		// Application is store version.
		isstore = 1;
	}

	if (notray == 0) {
		// Finalize tray icon
		Shell_NotifyIcon(NIM_ADD, &nid);
	}

	if (createstartup == 1) {
		create_startup();
	}

	if (removestartup == 1) {
		remove_startup();
	}

	std::wcout << "Initialize complete!" << std::endl;
	std::wcout << "Application is running!" << std::endl;

	SetTaskbar();

	std::atexit(exiting);

	for (;;) { //while (GetMessage(&msg, NULL, 0, 0)) {
		//GetMessage() blocks until there is a message available to retrieve. If you don't want to wait, then use PeekMessage() instead.
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		SetTaskbar();
	}
}

void SetTaskbar() {
	std::wcout.clear();

	working = 1;

	HWND curreg_Check_handle = FindWindow(0, L"Shell_TrayWnd");
	HRGN curreg_Check_region = CreateRectRgn(0, 0, 0, 0);
	GetWindowRgn(curreg_Check_handle, curreg_Check_region);

	if (curreg_Check_region == 0) {
		std::wcout << "HRGN invalid!" << std::endl;

		HWND Explorer = NULL;

		do
		{
			std::wcout << "Looking for Explorer..." << std::endl;
			Explorer = FindWindow(L"Shell_TrayWnd", 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		} while (Explorer == 0);

		Explorer = NULL;

		std::wcout << "Explorer found!" << std::endl;
		std::wcout << "Resetting..." << std::endl;

		taskbar_Count = 0;
		taskbar_List[0] = 0;
		taskbar_List[1] = 0;
		taskbar_List[2] = 0;
		taskbar_List[3] = 0;
		taskbar_List[4] = 0;
		taskbar_List[5] = 0;
		taskbar_List[6] = 0;
		taskbar_List[7] = 0;
		taskbar_List[8] = 0;
		taskbar_List[9] = 0;

		std::wstring chars = L"";
		chars += (wchar_t)34;
		std::string quote(chars.begin(), chars.end());

		WinExec((quote + cur_dir + quote + cur_cmd).c_str(), SW_HIDE);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		exit(0);
		abort;
	}

	std::wcout << "Clearing maximized window list..." << std::endl;

	maximized_Count = 0;
	maximized_List[0] = 0;
	maximized_List[1] = 0;
	maximized_List[2] = 0;
	maximized_List[3] = 0;
	maximized_List[4] = 0;
	maximized_List[5] = 0;
	maximized_List[6] = 0;
	maximized_List[7] = 0;
	maximized_List[8] = 0;
	maximized_List[9] = 0;
	maximized_List[10] = 0;

	EnumWindows(EnumCallbackMaximized, NULL);

	for (HWND tb : taskbar_List) {
		if (tb != 0) {
			//SendMessage(tb, WM_SETREDRAW, TRUE, NULL);
			//SendMessage(tb, WM_ERASEBKGND, TRUE, NULL);
			//SendMessage(tb, WM_SETTINGCHANGE, TRUE, NULL);
			//SendMessage(tb, WM_THEMECHANGED, TRUE, NULL);

			int taskbariscenter;

			HKEY hKey;
			DWORD buffer;
			LONG result;
			unsigned long type = REG_DWORD, size = 1024;

			result = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, KEY_READ, &hKey);
			if (result == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey, L"TaskbarAl", NULL, &type, (LPBYTE)&buffer, &size);

				RegCloseKey(hKey);

				if (size == 1024) { //Registry key has never been set so i assume it is centered.
					taskbariscenter = 1;
				}
				else {
					if (buffer == 1) {
						taskbariscenter = 1;
					}
					if (buffer == 0) {
						taskbariscenter = 0;
					}
				}
			}

			hKey = NULL;
			buffer = NULL;
			result = NULL;
			type = NULL;
			size = NULL;

			curreg_Check_handle = NULL;
			curreg_Check_region = NULL;

			int length = 256;
			wchar_t* title = new wchar_t[length];
			GetClassName(tb, title, length);

			//std::wcout << "Looping for " << title << " @ " << tb << std::endl;

			// Check if hWid is still valid if not find again
			if (wcscmp(title, L"Shell_TrayWnd") != 0 && wcscmp(title, L"Shell_SecondaryTrayWnd") != 0) {
				std::wcout << "hWID invalid!" << std::endl;

				HWND Explorer = NULL;

				do
				{
					std::wcout << "Looking for Explorer..." << std::endl;
					Explorer = FindWindow(L"Shell_TrayWnd", 0);
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
				} while (Explorer == 0);

				Explorer = NULL;

				std::wcout << "Explorer found!" << std::endl;
				std::wcout << "Resetting..." << std::endl;

				taskbar_Count = 0;
				taskbar_List[0] = 0;
				taskbar_List[1] = 0;
				taskbar_List[2] = 0;
				taskbar_List[3] = 0;
				taskbar_List[4] = 0;
				taskbar_List[5] = 0;
				taskbar_List[6] = 0;
				taskbar_List[7] = 0;
				taskbar_List[8] = 0;
				taskbar_List[9] = 0;

				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				int pos = cur_dir.find_last_of("/\\");
				std::wstring chars = L"";
				chars += (wchar_t)34;
				std::string quote(chars.begin(), chars.end());

				WinExec((quote + cur_dir + quote + cur_cmd).c_str(), SW_HIDE);

				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				exit(0);
				abort;
			}

			// This is the main taskbar
			if (wcscmp(title, L"Shell_TrayWnd") == 0) {
				HWND Shell_TrayWnd = tb;
				HWND Start = FindWindowEx(Shell_TrayWnd, 0, L"Start", NULL);
				HWND DesktopWindowContentBridge = FindWindowEx(Shell_TrayWnd, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
				HWND RebarWindow32 = FindWindowEx(Shell_TrayWnd, 0, L"RebarWindow32", NULL);
				HWND MSTaskSwWClass = FindWindowEx(RebarWindow32, 0, L"MSTaskSwWClass", NULL);
				HWND TrayNotifyWnd = FindWindowEx(Shell_TrayWnd, 0, L"TrayNotifyWnd", NULL);

				HWND SysPager = FindWindowEx(TrayNotifyWnd, 0, L"SysPager", NULL);
				HWND ToolbarWindow32 = FindWindowEx(TrayNotifyWnd, 0, L"ToolbarWindow32", NULL);
				HWND Button = FindWindowEx(TrayNotifyWnd, 0, L"Button", NULL);

				SendMessage(Shell_TrayWnd, WM_WINDOWPOSCHANGED, TRUE, 0);

				//SendMessage(tb, WM_SETREDRAW, FALSE, NULL);
				HRGN currenttbreg = CreateRectRgn(0, 0, 0, 0);
				GetWindowRgn(tb, currenttbreg);
				RECT currenttbrect;
				GetRgnBox(currenttbreg, &currenttbrect);

				RECT rect_Shell_TrayWnd;
				GetWindowRect(Shell_TrayWnd, &rect_Shell_TrayWnd);

				RECT rect_Start;
				GetWindowRect(Start, &rect_Start);

				RECT rect_MSTaskSwWClass;
				GetWindowRect(MSTaskSwWClass, &rect_MSTaskSwWClass);

				RECT rect_TrayNotifyWnd;
				GetWindowRect(TrayNotifyWnd, &rect_TrayNotifyWnd);

				RECT rect_ToolbarWindow32;
				GetWindowRect(ToolbarWindow32, &rect_ToolbarWindow32);

				RECT rect_SysPager;
				GetWindowRect(SysPager, &rect_SysPager);

				INT curDPI = GetDpiForWindow(Shell_TrayWnd) * 1.041666666666667;

				int width_Shell_TrayWnd = (rect_Shell_TrayWnd.right - rect_Shell_TrayWnd.left);
				int height_Shell_TrayWnd = (rect_Shell_TrayWnd.bottom - rect_Shell_TrayWnd.top);

				//int width_SysPager = (rect_SysPager.right - rect_SysPager.left) / 2;

				//int left = abs(rect_MSTaskSwWClass.right - width_Shell_TrayWnd - rect_Shell_TrayWnd.left + 1) * curDPI / 100;
				//SendMessage(Start, WM_SETREDRAW, TRUE, NULL);

				//int left = abs(rect_Start.left - rect_Shell_TrayWnd.left + 1) * curDPI / 100;
				//int top = 2 * curDPI / 100;
				//int right = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100;
				//int bottom = rect_MSTaskSwWClass.bottom * curDPI / 100;
				//SendMessage(Shell_TrayWnd, WM_SETTINGCHANGE, TRUE, NULL);

				//SendMessage(Shell_TrayWnd, WM_SIZING, 0x3, 0x0354D4D0);

				int left;
				int top;
				int right;
				int bottom;

				if (taskbariscenter == 1) {
					if (sticky == 1) {
						left = (abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.right + 1) * curDPI / 100) - (((rect_TrayNotifyWnd.right - rect_TrayNotifyWnd.left) / 2) * curDPI / 100);
						//left = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.right + 1) * curDPI / 100;
					}
					else {
						left = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.right + 1) * curDPI / 100;
					}

					top = 2 * curDPI / 100;
					right = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100;
					bottom = rect_MSTaskSwWClass.bottom * curDPI / 100;
				}

				if (taskbariscenter == 0) {
					left = abs(rect_Shell_TrayWnd.left + 1) * curDPI / 100;
					top = 2 * curDPI / 100;
					right = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100;
					bottom = rect_MSTaskSwWClass.bottom * curDPI / 100;
				}

				if (sticky == 1) {
					//ShowWindow(ToolbarWindow32, SW_HIDE);
					//ShowWindow(SysPager, SW_HIDE);
					//ShowWindow(Button, SW_HIDE);
					//SendMessage(Button, WM_SETREDRAW, FALSE, 0);
					//SendMessage(ToolbarWindow32, WM_SETREDRAW, FALSE, 0);
					//SendMessage(SysPager, WM_SETREDRAW, FALSE, 0);

					//ShowWindow(ToolbarWindow32, SW_HIDE);
					//ShowWindow(SysPager, SW_HIDE);
					//ShowWindow(Button, SW_HIDE);
					//std::wcout << width_SysPager << std::endl;

					//SendMessage(Button, WM_SETREDRAW, TRUE, 0);

					SetWindowPos(DesktopWindowContentBridge, NULL, -(((rect_TrayNotifyWnd.right - rect_TrayNotifyWnd.left) / 2)), 0, 0, 0, SWP_NOSIZE | SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSENDCHANGING);
					SetWindowPos(TrayNotifyWnd, NULL, rect_MSTaskSwWClass.right - ((rect_TrayNotifyWnd.right - rect_TrayNotifyWnd.left) / 2), 0, 0, 0, SWP_NOSIZE | SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSENDCHANGING);
				}

				HRGN region_ShellTrayWnd;
				HRGN region_TrayNotifyWnd;

				if (square == 0) {
					region_ShellTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 15, 15);
					region_TrayNotifyWnd = CreateRoundRectRgn(abs(rect_TrayNotifyWnd.left - rect_Shell_TrayWnd.left - 5) * curDPI / 100, top, abs(rect_TrayNotifyWnd.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100, bottom, 15, 15);
				}
				else {
					region_ShellTrayWnd = CreateRectRgn(left, top, right, bottom);
					region_TrayNotifyWnd = CreateRectRgn(abs(rect_TrayNotifyWnd.left - rect_Shell_TrayWnd.left - 5) * curDPI / 100, top, abs(rect_TrayNotifyWnd.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100, bottom);
				}

				mtaskbar_Revert = 0;

				for (HWND mx1 : maximized_List) {
					if (mx1 != 0) {
						HMONITOR tbm1 = MonitorFromWindow(Shell_TrayWnd, MONITOR_DEFAULTTONEAREST);
						HMONITOR wm1 = MonitorFromWindow(mx1, MONITOR_DEFAULTTONEAREST);
						if (tbm1 == wm1) {
							std::wcout << title << " @ " << Shell_TrayWnd << " has a maximized window!" << std::endl;
							HRGN region_Empty = CreateRectRgn(abs(rect_Shell_TrayWnd.left - rect_Shell_TrayWnd.left) * curDPI / 100, 0, abs(rect_Shell_TrayWnd.right - rect_Shell_TrayWnd.left) * curDPI / 100, rect_Shell_TrayWnd.bottom * curDPI / 100);
							SetWindowRgn(Shell_TrayWnd, region_Empty, TRUE);
							mtaskbar_Revert = 1;
							region_Empty = NULL;
						}
					}
				}

				if (mtaskbar_Revert == 0) {
					HRGN region_Both = CreateRectRgn(0, 0, 0, 0);
					if (hidetraywnd == 0) {
						CombineRgn(region_Both, region_ShellTrayWnd, region_TrayNotifyWnd, RGN_OR);
					}
					else {
						region_Both = region_ShellTrayWnd;
					}

					RECT newtbrect;
					GetRgnBox(region_ShellTrayWnd, &newtbrect);

					// std::wcout << rect_TrayNotifyWnd.left << " " << trayleft << std::endl;

					if (newtbrect.left != abs(currenttbrect.left) * curDPI / 100) {
						//SendMessage(tb, WM_SETREDRAW, FALSE, NULL);
						//SendMessage(tb, WM_THEMECHANGED, TRUE, 0);
						SetWindowRgn(Shell_TrayWnd, region_Both, TRUE);
						//SendMessage(tb, WM_SETTINGCHANGE, FALSE, 0);
						//SendMessage(tb, WM_SETREDRAW, TRUE, NULL);
					}
					else {
						if (rect_TrayNotifyWnd.left != trayleft) {
							SetWindowRgn(Shell_TrayWnd, region_Both, TRUE);
							trayleft = rect_TrayNotifyWnd.left;
							//SendMessage(tb, WM_SETREDRAW, TRUE, NULL);
						}
						else {
							std::wcout << title << " @ " << Shell_TrayWnd << " does not need new HRGN!" << std::endl;
						}
					}

					region_Both = NULL;
				}

				// HRGN region_ShellTrayWnd = CreateRectRgn(left, top, right, bottom);
				// HRGN region_TrayNotifyWnd = CreateRectRgn(abs(rect_TrayNotifyWnd.left - rect_Shell_TrayWnd.left + 2) * curDPI / 100, top, abs(rect_TrayNotifyWnd.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100, bottom);

				std::wcout << "Done with " << title << " @ " << Shell_TrayWnd << std::endl;

				// dispose
				//currenttbreg = NULL;
				Shell_TrayWnd = NULL;
				Start = NULL;
				RebarWindow32 = NULL;
				MSTaskSwWClass = NULL;
				TrayNotifyWnd = NULL;
				curDPI = NULL;
				width_Shell_TrayWnd = NULL;
				height_Shell_TrayWnd = NULL;
				left = NULL;
				right = NULL;
				bottom = NULL;
				top = NULL;
				region_ShellTrayWnd = NULL;
				region_TrayNotifyWnd = NULL;

				mtaskbar_Revert = 0;

				//SendMessage(tb, WM_SETREDRAW, FALSE, NULL);
				//SendMessage(tb, WM_THEMECHANGED, TRUE, 0);
				//SendMessage(tb, WM_ERASEBKGND, TRUE, NULL);
				//SendMessage(tb, WM_SETTINGCHANGE, TRUE, NULL);
				//SendMessage(tb, WM_SETREDRAW, TRUE, NULL);

				//SendMessage(Shell_TrayWnd, WM_SETTINGCHANGE, TRUE, NULL);
				//SendMessage(Shell_TrayWnd, WM_THEMECHANGED, TRUE, 0);
			} //end primary taskbar

			// This is a secondary taskbar
			if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {
				HWND Shell_SecondaryTrayWnd = tb;
				HWND Start = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Start", NULL);
				HWND WorkerW = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"WorkerW", NULL);
				HWND DesktopWindowContentBridge = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
				HWND MSTaskListWClass = FindWindowEx(WorkerW, 0, L"MSTaskListWClass", NULL);

				SendMessage(Shell_SecondaryTrayWnd, WM_WINDOWPOSCHANGED, TRUE, 0);

				HRGN currenttbreg = CreateRectRgn(0, 0, 0, 0);
				GetWindowRgn(tb, currenttbreg);
				RECT currenttbrect;
				GetRgnBox(currenttbreg, &currenttbrect);

				RECT rect_Shell_SecondaryTrayWnd;
				GetWindowRect(Shell_SecondaryTrayWnd, &rect_Shell_SecondaryTrayWnd);

				RECT rect_DesktopWindowContentBridge;
				GetWindowRect(DesktopWindowContentBridge, &rect_DesktopWindowContentBridge);

				RECT rect_Start;
				GetWindowRect(Start, &rect_Start);

				RECT rect_WorkerW;
				GetWindowRect(WorkerW, &rect_WorkerW);

				RECT rect_MSTaskListWClass;
				GetWindowRect(MSTaskListWClass, &rect_MSTaskListWClass);

				INT curDPI = GetDpiForWindow(Shell_SecondaryTrayWnd) * 1.041666666666667;

				int width_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.right - rect_Shell_SecondaryTrayWnd.left);
				int height_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.bottom - rect_Shell_SecondaryTrayWnd.top);

				//SendMessage(Shell_SecondaryTrayWnd, WM_SETTINGCHANGE, TRUE, 0);
				//SendMessage(Shell_SecondaryTrayWnd, WM_THEMECHANGED, TRUE, 0);

				int left;
				int top;
				int right;
				int bottom;

				if (taskbariscenter == 1) {
					left = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.right + 2) * curDPI / 100;
					top = 2 * curDPI / 100;
					right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 1) * curDPI / 100;
					bottom = rect_MSTaskListWClass.bottom * curDPI / 100;
				}

				if (taskbariscenter == 0) {
					left = abs(rect_Shell_SecondaryTrayWnd.left - rect_Shell_SecondaryTrayWnd.left + 2) * curDPI / 100;
					top = 2 * curDPI / 100;
					right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 1) * curDPI / 100;
					bottom = rect_MSTaskListWClass.bottom * curDPI / 100;
				}

				HRGN region_Shell_SecondaryTrayWnd;

				if (square == 0) {
					region_Shell_SecondaryTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 15, 15);
				}
				else {
					region_Shell_SecondaryTrayWnd = CreateRectRgn(left, top, right, bottom);
				}

				staskbar_Revert = 0;

				for (HWND mx2 : maximized_List) {
					if (mx2 != 0) {
						HMONITOR tbm2 = MonitorFromWindow(Shell_SecondaryTrayWnd, MONITOR_DEFAULTTONEAREST);
						HMONITOR wm2 = MonitorFromWindow(mx2, MONITOR_DEFAULTTONEAREST);
						if (tbm2 == wm2) {
							std::wcout << title << " @ " << Shell_SecondaryTrayWnd << " has a maximized window!" << std::endl;
							HRGN region_Empty = CreateRectRgn(abs(rect_Shell_SecondaryTrayWnd.left - rect_Shell_SecondaryTrayWnd.left) * curDPI / 100, 0, abs(rect_Shell_SecondaryTrayWnd.right - rect_Shell_SecondaryTrayWnd.left) * curDPI / 100, rect_Shell_SecondaryTrayWnd.bottom * curDPI / 100);
							SetWindowRgn(Shell_SecondaryTrayWnd, region_Empty, TRUE);
							staskbar_Revert = 1;
							region_Empty = NULL;
						}
					}
				}

				if (staskbar_Revert == 0) {
					//HRGN region_Shell_SecondaryTrayWnd = CreateRectRgn(left, top, right, bottom);

					RECT newtbrect;
					GetRgnBox(region_Shell_SecondaryTrayWnd, &newtbrect);

					//std::wcout << newtbrect.left << std::endl;
					//std::wcout << abs(currenttbrect.left) * curDPI / 100 << std::endl;

					if (newtbrect.left != abs(currenttbrect.left) * curDPI / 100) {
						SetWindowRgn(Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd, TRUE);
					}
					else {
						std::wcout << title << " @ " << Shell_SecondaryTrayWnd << " does not need new HRGN!" << std::endl;
					}

					region_Shell_SecondaryTrayWnd = NULL;
				}

				staskbar_Revert = 0;

				std::wcout << "Done with " << title << " @ " << Shell_SecondaryTrayWnd << std::endl;

				// dispose
				left = NULL;
				right = NULL;
				bottom = NULL;
				top = NULL;
				currenttbreg = NULL;
				Shell_SecondaryTrayWnd = NULL;
				Start = NULL;
				WorkerW = NULL;
				MSTaskListWClass = NULL;
				curDPI = NULL;
				width_Shell_SecondaryTrayWnd = NULL;
				height_Shell_SecondaryTrayWnd = NULL;

				//SendMessage(Shell_SecondaryTrayWnd, WM_SETTINGCHANGE, TRUE, 0);
				//SendMessage(Shell_SecondaryTrayWnd, WM_THEMECHANGED, TRUE, 0);
			} //end secondary taskbar
		}
	}

	//std::wcout << "Done with all taskbars. Sleeping for 250 milliseconds..." << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	working = 0;
}

BOOL CALLBACK EnumCallbackTaskbars(HWND hWND, LPARAM lParam) {
	int length = 256;
	wchar_t* title = new wchar_t[length];

	GetClassName(hWND, title, length);

	if (wcscmp(title, L"Shell_TrayWnd") == 0) {
		//SendMessage(hWND, WM_SETREDRAW, TRUE, NULL);
		std::wcout << "Main taskbar found! @ hWid : " << hWND << std::endl;
		taskbar_List[taskbar_Count] = hWND;
		taskbar_Count += 1;
		//SendMessage(hWND, WM_SETREDRAW, FALSE, NULL);
		//SendMessage(hWND, WM_THEMECHANGED, TRUE, 0);
		//SendMessage(hWND, WM_ERASEBKGND, TRUE, NULL);
		//SendMessage(hWND, WM_SETTINGCHANGE, TRUE, NULL);
		//SendMessage(hWND, WM_SETREDRAW, TRUE, NULL);
	}

	if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {
		//SendMessage(hWND, WM_SETREDRAW, TRUE, NULL);
		std::wcout << "A Secondary taskbar found! @ hWid : " << hWND << std::endl;
		taskbar_List[taskbar_Count] = hWND;
		taskbar_Count += 1;
		//SendMessage(hWND, WM_SETREDRAW, FALSE, NULL);
		//SendMessage(hWND, WM_THEMECHANGED, TRUE, 0);
		//SendMessage(hWND, WM_ERASEBKGND, TRUE, NULL);
		//SendMessage(hWND, WM_SETTINGCHANGE, TRUE, NULL);
		//SendMessage(hWND, WM_SETREDRAW, TRUE, NULL);
	}

	hWND = NULL;
	title = NULL;
	length = NULL;

	return true;
}

BOOL CALLBACK EnumCallbackMaximized(HWND hWND, LPARAM lParam) {
	WINDOWPLACEMENT wp;
	GetWindowPlacement(hWND, &wp);
	INT wl = GetWindowLong(hWND, GWL_STYLE);
	INT Cloaked;
	INT cl = DwmGetWindowAttribute(hWND, DWMWINDOWATTRIBUTE::DWMWA_CLOAKED, &Cloaked, sizeof(Cloaked));

	// Look for non Phanthom maximized windows
	if (ignoremax == 0) {
		if (wp.showCmd == 3) {
			if (Cloaked == 0) {
				if (wl and WS_VISIBLE == WS_VISIBLE) {
					if ((wl | WS_MAXIMIZE) == wl) {
						if ((wl | WS_VISIBLE) == wl) {
							//std::wcout << hWND << wl << std::endl;
							maximized_List[maximized_Count] = hWND;
							maximized_Count += 1;
						}
					}
				}
			}
		}
	}

	wp.length = NULL;
	wl = NULL;
	Cloaked = NULL;
	cl = NULL;

	return true;
}

BOOL CALLBACK EnumCallbackInstances(HWND hWND, LPARAM lParam) {
	int length = 256;
	wchar_t* title = new wchar_t[length];

	GetClassName(hWND, title, length);

	if (wcscmp(title, L"TaskbarXI") == 0) {
		DWORD PID;
		GetWindowThreadProcessId(hWND, &PID);

		DWORD MYPID;
		MYPID = GetCurrentProcessId();

		if (MYPID != PID) {
			std::wcout << "Another TaskbarXI instance has been detected! Terminating other instance..." << std::endl;
			HANDLE HTARGET = OpenProcess(PROCESS_ALL_ACCESS, false, PID);

			NOTIFYICONDATA inid = {};
			inid.cbSize = sizeof(inid);
			inid.hWnd = hWND;
			inid.uID = 1;

			Shell_NotifyIcon(NIM_DELETE, &inid);

			TerminateProcess(HTARGET, 0);
		}
	}

	hWND = NULL;
	title = NULL;
	length = NULL;

	return true;
}