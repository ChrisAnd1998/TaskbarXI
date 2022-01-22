// TaskbarXI for Windows 11.
// By Chris Andriessen 2022
// https://github.com/ChrisAnd1998/TaskbarXI

#pragma comment (lib, "dwmapi.lib")

#define WIN32_LEAN_AND_MEAN
#define NOT_BUILD_WINDOWS_DEPRECATE

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

int working = -1;

int eventtrigger;

BOOL CALLBACK EnumCallbackTaskbars(HWND hWND, LPARAM lParam);
BOOL CALLBACK EnumCallbackMaximized(HWND hWND, LPARAM lParam);
BOOL CALLBACK EnumCallbackInstances(HWND hWND, LPARAM lParam);

std::thread thread_List[10];
HWND animating_List[10];
HWND maximized_List[10];
HWND taskbar_List[10];
int thread_Count;
int animating_Count;
int taskbar_Count;
int maximized_Count;

int mtaskbar_Revert;
int staskbar_Revert;

int maxCountChanged;
int oldMaxCount;

int trayleft;

int taskbariscenter = 1;

int isstore;

bool boxopen;

void initTray(HWND parent);
VOID SetTaskbar();

//Settings
int square;
int corner_Radius;
int ignoremax;
int notray;
int hidetraywnd;
int stop;
int restart;
int createstartup;
int removestartup;
int sticky;
int smoothresize;
int blur;
int expandspeed;
int shrinkspeed;

const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));

struct ACCENTPOLICY
{
	int nAccentState;
	int nFlags;
	int nColor;
	int nAnimationId;
};
struct WINCOMPATTRDATA
{
	int nAttribute;
	PVOID pData;
	ULONG ulDataSize;
};

void callSetTaskbar() {
	std::thread{ SetTaskbar }.detach();
}

int __cdecl main(int argc, char** argv) {
	return WinMain(0, 0, NULL, 0);
}

typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");

void SetWindowBlur()
{
	//https://github.com/TranslucentTB/TranslucentTB
	if (hModule)
	{
		if (SetWindowCompositionAttribute)
		{
			ACCENTPOLICY policy = { 3, 0, 0, 0 };
			WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) };
			for (;;) {
				for (HWND tb : taskbar_List) {
					if (tb != 0) {
						SetWindowCompositionAttribute(tb, &data);
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(14));
			}
		}
	}
}

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (eventtrigger == 0) {
		if (working == 0) {
			eventtrigger = 1;
			int length = 7;
			wchar_t* title = new wchar_t[length];
			GetClassName(hwnd, title, length);
			if (wcscmp(title, L"MSTask") == 0) {
				callSetTaskbar();
			}
			if (wcscmp(title, L"Toolba") == 0) {
				callSetTaskbar();
			}
			title = NULL;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			eventtrigger = 0;
		}
	}
}

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

			HWND Shell_TrayWnd = FindWindow(L"Shell_TrayWnd", 0);
			HWND TrayNotifyWnd = FindWindowEx(Shell_TrayWnd, 0, L"TrayNotifyWnd", NULL);

			HWND SysPager = FindWindowEx(TrayNotifyWnd, 0, L"SysPager", NULL);
			HWND ToolbarWindow32 = FindWindowEx(TrayNotifyWnd, 0, L"ToolbarWindow32", NULL);
			HWND Button = FindWindowEx(TrayNotifyWnd, 0, L"Button", NULL);

			HWND RebarWindow32 = FindWindowEx(tb, 0, L"RebarWindow32", NULL);
			HWND WorkerW = FindWindowEx(tb, 0, L"WorkerW", NULL);
			HWND MSTaskSwWClass = FindWindowEx(tb, 0, L"MSTaskSwWClass", NULL);
			HWND MSTaskListWClass = FindWindowEx(tb, 0, L"MSTaskListWClass", NULL);

			if (RebarWindow32 != 0) {
				SendMessage(RebarWindow32, WM_SETREDRAW, TRUE, NULL);
			}

			if (WorkerW != 0) {
				SendMessage(WorkerW, WM_SETREDRAW, TRUE, NULL);
			}

			if (MSTaskSwWClass != 0) {
				SendMessage(MSTaskSwWClass, WM_SETREDRAW, TRUE, NULL);
			}

			if (MSTaskListWClass != 0) {
				SendMessage(MSTaskListWClass, WM_SETREDRAW, TRUE, NULL);
			}

			SendMessage(GetParent(tb), WM_SETREDRAW, FALSE, NULL);

			ShowWindow(ToolbarWindow32, SW_SHOW);
			ShowWindow(SysPager, SW_SHOW);
			ShowWindow(Button, SW_SHOW);

			SendMessage(tb, WM_THEMECHANGED, TRUE, NULL);
			SendMessage(tb, WM_SETTINGCHANGE, TRUE, NULL);
		}
	}

	exit(0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (boxopen == true) {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	switch (uMsg)
	{
	case APPWM_ICONNOTIFY:
	{
		switch (lParam)
		{
		case WM_LBUTTONUP:
			boxopen = true;
			callSetTaskbar();
			if (MessageBox(NULL, L"Do you want to EXIT TaskbarXI?", L"TaskbarXI", MB_YESNO) == IDYES)
			{
				exiting();
			}
			else {
				boxopen = false;
			}
			break;
		case WM_RBUTTONUP:
			boxopen = true;
			callSetTaskbar();
			if (MessageBox(NULL, L"Do you want to EXIT TaskbarXI?", L"TaskbarXI", MB_YESNO) == IDYES)
			{
				exiting();
			}
			else {
				boxopen = false;
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

HRESULT UpdateWindows11RoundCorners(HWND hWnd)
{
	typedef HRESULT(WINAPI* PFNSETWINDOWATTRIBUTE)(HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
	enum DWMWINDOWATTRIBUTE
	{
		DWMWA_WINDOW_CORNER_PREFERENCE = 33
	};
	enum DWM_WINDOW_CORNER_PREFERENCE
	{
		DWMWCP_DEFAULT = 0,
		DWMWCP_DONOTROUND = 1,
		DWMWCP_ROUND = 2,
		DWMWCP_ROUNDSMALL = 3
	};
	HMODULE hDwmApi = ::LoadLibrary(TEXT("dwmapi.dll"));
	if (hDwmApi)
	{
		auto* pfnSetWindowAttribute = reinterpret_cast<PFNSETWINDOWATTRIBUTE>(
			::GetProcAddress(hDwmApi, "DwmSetWindowAttribute"));
		if (pfnSetWindowAttribute)
		{
			auto preference = static_cast<DWM_WINDOW_CORNER_PREFERENCE>(2);
			return pfnSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE,
				&preference, sizeof(DWM_WINDOW_CORNER_PREFERENCE));
		}
		::FreeLibrary(hDwmApi);
	}
	return E_FAIL;
}

std::string animating;

void SetWindowRegionAnimated(HWND hWND, HRGN region) {
	try {
		INT curDPI = GetDpiForWindow(hWND) * 1.041666666666667;
		HRGN currenttbreg = CreateRectRgn(0, 0, 0, 0);
		RECT currenttbrect;
		RECT newtbrect;

		GetWindowRgn(hWND, currenttbreg);
		GetRgnBox(currenttbreg, &currenttbrect);
		GetRgnBox(region, &newtbrect);

		int makebigger;

		if (currenttbrect.right == 0) {
			SetWindowRgn(hWND, region, TRUE);
			GetWindowRgn(hWND, currenttbreg);
			GetRgnBox(currenttbreg, &currenttbrect);
			GetRgnBox(region, &newtbrect);
		}

		if (abs((currenttbrect.right * curDPI / 100) - (newtbrect.right)) >= 100) {
			SetWindowRgn(hWND, region, TRUE);
			return;
		}

		if (currenttbrect.right * curDPI / 100 <= newtbrect.right) {
			makebigger = 1;
		}
		else {
			makebigger = 0;
		}

		int left = abs(currenttbrect.left * curDPI / 100);
		int top = 0;
		int right = abs(currenttbrect.right * curDPI / 100);
		int bottom = newtbrect.bottom;

		if (newtbrect.right == 0) {
			SetWindowRgn(hWND, region, TRUE);
			return;
		}

		for (;;) {
			int currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

			if (taskbariscenter == 1) {
				if (left == newtbrect.left) {
					//End reached
					SetWindowRgn(hWND, region, TRUE);
					break;
				}
			}

			if (right == newtbrect.right) {
				//End reached
				SetWindowRgn(hWND, region, TRUE);
				break;
			}

			if (makebigger == 1) {
				//The taskbar needs to expand
				if (taskbariscenter == 1) {
					if (left <= newtbrect.left) {
						//End reached
						SetWindowRgn(hWND, region, TRUE);
						break;
					}
				}

				if (right >= newtbrect.right) {
					//End reached
					SetWindowRgn(hWND, region, TRUE);
					break;
				}

				if (taskbariscenter == 1) {
					left = abs(left - 1);
				}

				right = abs(right + 1);
			}
			else {
				//The taskbar needs to shrink
				if (taskbariscenter == 1) {
					if (left >= newtbrect.left) {
						//End reached
						SetWindowRgn(hWND, region, TRUE);
						break;
					}
				}

				if (right <= newtbrect.right) {
					//End reached
					SetWindowRgn(hWND, region, TRUE);
					break;
				}

				if (taskbariscenter == 1) {
					left = abs(left + 1);
				}

				right = abs(right - 1);
			}

			//Speed
			if (makebigger == 1) {
				for (;;) {
					int elapsed = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) - currentTime;
					int speed = expandspeed;
					if (taskbariscenter == 0) {
						speed = expandspeed / 12;
					}
					if (elapsed >= speed / curDPI) {
						//SetWindowRgn(hWND, region, TRUE);
						break;
					}
					elapsed = NULL;
					speed = NULL;
					//std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
			else {
				for (;;) {
					int elapsed = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) - currentTime;
					int speed = shrinkspeed;
					if (taskbariscenter == 0) {
						speed = shrinkspeed / 12;
					}
					if (elapsed >= speed / curDPI) {
						//SetWindowRgn(hWND, region, TRUE);
						break;
					}
					elapsed = NULL;
					speed = NULL;
					//std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}

			if (square == 0) {
				HRGN framereg = CreateRoundRectRgn(left, top, right, bottom, corner_Radius, corner_Radius);
				SetWindowRgn(hWND, framereg, TRUE);
				framereg = NULL;
			}
			else {
				HRGN framereg = CreateRectRgn(left, top, right, bottom);
				SetWindowRgn(hWND, framereg, TRUE);
				framereg = NULL;
			}

			currentTime = NULL;
		}

		SetWindowRgn(hWND, region, TRUE);

		SendMessage(hWND, WM_WINDOWPOSCHANGED, TRUE, 0);
		SendMessage(hWND, WM_PARENTNOTIFY, 0x00000201, 0x0039065A);

		SetWindowRgn(hWND, region, TRUE);

		curDPI = NULL;
		currenttbreg = NULL;

		left = NULL;
		top = NULL;
		right = NULL;
		bottom = NULL;

		return;
	}

	catch (...) { SetWindowRgn(hWND, region, TRUE); }
}

void taskbarLoop() {
	MSG msg;
	for (;;) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		callSetTaskbar();
	}
}

int WINAPI WinMain(_In_opt_ HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
		if (wcscmp(szArgList[i], L"-restart") == 0) {
			restart = 1;
			//cur_cmd.append(" -stop");
		}
		if (wcscmp(szArgList[i], L"-square") == 0) {
			square = 1;
			cur_cmd.append(" -square");
		}
		if (wcscmp(szArgList[i], L"-radius") == 0) {
			corner_Radius = _wtoi(szArgList[++i]);
			cur_cmd.append(" -radius " + corner_Radius);
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
		if (wcscmp(szArgList[i], L"-smoothresize") == 0) {
			smoothresize = 1;
			cur_cmd.append(" -smoothresize");
		}
		if (wcscmp(szArgList[i], L"-expandspeed") == 0) {
			expandspeed = _wtoi(szArgList[++i]);
			cur_cmd.append(" -expandspeed " + expandspeed);
		}
		if (wcscmp(szArgList[i], L"-shrinkspeed") == 0) {
			shrinkspeed = _wtoi(szArgList[++i]);
			cur_cmd.append(" -shrinkspeed " + shrinkspeed);
		}

		if (wcscmp(szArgList[i], L"-blur") == 0) {
			blur = 1;
			cur_cmd.append(" -blur");
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
		if (wcscmp(szArgList[i], L"-help") == 0) {
			AllocConsole();
			FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;
			//freopen_s(&fpstdin, "CONIN$", "r", stdin);
			freopen_s(&fpstdout, "CONOUT$", "w", stdout);
			freopen_s(&fpstderr, "CONOUT$", "w", stderr);

			std::wcout << "                                                  " << std::endl;
			std::wcout << "    bTTTTTTTTTTTTTTTTTTTTTTTTTTTbb     bTTTTTb    " << std::endl;
			std::wcout << "    bTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTb   bTTTTTb    " << std::endl;
			std::wcout << "    bbTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTb  bTTTTTb    " << std::endl;
			std::wcout << "             bbbbbbbb         bTTTTTb  bTTTTTb    " << std::endl;
			std::wcout << "              bbbbbbb     bbbbTTTTTTb  bTTTTTb    " << std::endl;
			std::wcout << "               bTTTTTT   bTTTTTTTTTb   bTTTTTb    " << std::endl;
			std::wcout << "           bbbb bTTTTTT  bTTTTTTTTTb   bTTTTTb    " << std::endl;
			std::wcout << "         bbTTTTb bTTTTTT  bbbbbTTTTTb  bTTTTTb    " << std::endl;
			std::wcout << "        bTTTTTTb  bTTTTTT      bTTTTbb bTTTTTb    " << std::endl;
			std::wcout << "      bTTTTTTTb    bTTTTTTbbbbbTTTTTT  bTTTTTb    " << std::endl;
			std::wcout << "    bTTTTTTTb       bTTTTTTTTTTTTTTTb  bTTTTTb    " << std::endl;
			std::wcout << "     TTTTTb          bTTTTTTTTTTTTb    bTTTTTb    " << std::endl;
			std::wcout << "      bbb             bbbbbbbbbbb      bbbbbb     " << std::endl;
			std::wcout << "                                                  " << std::endl;

			std::wcout << "An application written in C++ to modify the Windows 11 Taskbar. By Chris Andriessen." << std::endl;
			std::wcout << "https://github.com/ChrisAnd1998/TaskbarXI" << std::endl;
			std::wcout << "" << std::endl;
			std::wcout << "PARAMETERS:" << std::endl;
			std::wcout << "-help				(Shows this window)" << std::endl;
			std::wcout << "-stop				(Stops TaskbarXI and reverts the taskbar to default)" << std::endl;
			std::wcout << "-restart				(Does not refresh the taskbar region when starting)" << std::endl;
			std::wcout << "-square				(Uses square corners instead of rounded corners)" << std::endl;
			std::wcout << "-radius <radius>		(Define the corner radius you want to be used)" << std::endl;
			std::wcout << "-ignoremax			(Does not revert the taskbar on maximized window)" << std::endl;
			std::wcout << "-notray				(Disables system tray icon)" << std::endl;
			std::wcout << "-hidetraywnd			(Hides the system tray area)" << std::endl;
			std::wcout << "-createstartup			(Creates a startup entry including the current parameters)" << std::endl;
			std::wcout << "-removestartup			(Removes startup entry and exits TaskbarXI)" << std::endl;
			std::wcout << "-console			(Displays a console window)" << std::endl;
			std::wcout << "-sticky				(Sticks the system tray to the taskbar (removes the tray icons to keep it stable))" << std::endl;
			std::wcout << "-smoothresize				(Resizes the taskbar smoothly)" << std::endl;
			std::wcout << "-expandspeed <speed>		(Define the speed you want to be used for the expand animation (default: 90))" << std::endl;
			std::wcout << "-shrinkspeed <speed>		(Define the speed you want to be used for the shrink animation (default: 700))" << std::endl;
			std::wcout << "-blur				(Makes the taskbar blurred)" << std::endl;
			std::wcout << "" << std::endl;
			std::wcout << "EXAMPLE: TaskbarXI.exe -ignoremax -expandspeed 100 -square ";

			FreeConsole();
		}
	}

	LocalFree(szArgList);

	if (corner_Radius == 0) {
		corner_Radius = 15;
	}

	if (expandspeed == 0) {
		expandspeed = 90;
	}

	if (shrinkspeed == 0) {
		shrinkspeed = 700;
	}

	EnumWindows(EnumCallbackInstances, NULL);

	working = -1;

	SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);
	SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_DESTROY, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);
	SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);
	SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);

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

	if (notray == 0) {
		// Finalize tray icon
		if (sticky == 0) {
			Shell_NotifyIcon(NIM_ADD, &nid);
		}
	}

	//End setup Notifyicon

	Explorer = NULL;

	std::wcout << "Looking for taskbars..." << std::endl;

	// Find all taskbar(s)
	taskbar_Count = 0;
	ZeroMemory(&taskbar_List, sizeof(taskbar_List));

	EnumWindows(EnumCallbackTaskbars, NULL);

	for (HWND tb : taskbar_List) {
		//RECT rect_tb;
		//GetWindowRect(tb, &rect_tb);

		//INT curDPI = GetDpiForWindow(tb) * 1.041666666666667;
		if (restart == 0) {
			HRGN region_Empty = CreateRectRgn(0, 0, 0, 0);
			SetWindowRgn(tb, region_Empty, FALSE);
		}

		SendMessage(tb, WM_THEMECHANGED, TRUE, NULL);

		//SendMessage(tb, WM_SETTINGCHANGE, TRUE, NULL);
	}

	//Initilize animating list
	animating_Count = 0;
	ZeroMemory(&animating_List, sizeof(animating_List));

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

	if (createstartup == 1) {
		create_startup();
	}

	if (removestartup == 1) {
		remove_startup();
	}

	if (blur == 1) {
		std::thread{ SetWindowBlur }.detach();
	}

	std::wcout << "Initialize complete!" << std::endl;
	std::wcout << "Application is running!" << std::endl;

	std::atexit(exiting);

	std::thread{ taskbarLoop }.detach();

	for (;;) {
		//GetMessage() blocks until there is a message available to retrieve. If you don't want to wait, then use PeekMessage() instead.
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void SetTaskbar() {
	try {
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
			ZeroMemory(&taskbar_List, sizeof(taskbar_List));

			std::wstring chars = L"";
			chars += (wchar_t)34;
			std::string quote(chars.begin(), chars.end());

			if (isstore == 1) {
				WinExec(("Explorer.exe taskbarxi: -restart " + cur_cmd).c_str(), SW_HIDE);
			}
			else {
				WinExec((quote + cur_dir + quote + " -restart " + cur_cmd).c_str(), SW_HIDE);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			exit(0);
			abort;
		}

		std::wcout << "Clearing maximized window list..." << std::endl;

		maxCountChanged = 0;
		maximized_Count = 0;
		ZeroMemory(&maximized_List, sizeof(maximized_List));

		EnumWindows(EnumCallbackMaximized, NULL);

		for (HWND tb : taskbar_List) {
			if (tb != 0) {
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

				//int length = 256;
				//wchar_t* title = new wchar_t[length];
				//GetClassName(tb, title, length);

				HWND isataskbar = FindWindowEx(tb, 0, L"Start", NULL); //All Taskbars have a Start button
				HWND isprimarytaskbar = FindWindowEx(tb, 0, L"RebarWindow32", NULL); //Only the primary taskbar has a RebarWindow32

				// Check if hWid is still valid if not find again
				//if (wcscmp(title, L"Shell_TrayWnd") != 0 && wcscmp(title, L"Shell_SecondaryTrayWnd") != 0) {
				if (isataskbar == 0) {
					//free(title);

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
					ZeroMemory(&taskbar_List, sizeof(taskbar_List));

					std::this_thread::sleep_for(std::chrono::milliseconds(500));

					int pos = cur_dir.find_last_of("/\\");
					std::wstring chars = L"";
					chars += (wchar_t)34;
					std::string quote(chars.begin(), chars.end());

					if (isstore == 1) {
						WinExec(("Explorer.exe taskbarxi: -restart " + cur_cmd).c_str(), SW_HIDE);
					}
					else {
						WinExec((quote + cur_dir + quote + " -restart " + cur_cmd).c_str(), SW_HIDE);
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(500));

					exit(0);
					abort;
				}

				thread_Count = 0;
				ZeroMemory(&thread_List, sizeof(thread_List));

				// This is the main taskbar
				//if (wcscmp(title, L"Shell_TrayWnd") == 0) {
				if (isataskbar != 0 && isprimarytaskbar != 0) {
					HWND Shell_TrayWnd = tb;
					HWND Start = FindWindowEx(Shell_TrayWnd, 0, L"Start", NULL);
					HWND DesktopWindowContentBridge = FindWindowEx(Shell_TrayWnd, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
					HWND RebarWindow32 = FindWindowEx(Shell_TrayWnd, 0, L"RebarWindow32", NULL);
					HWND MSTaskSwWClass = FindWindowEx(RebarWindow32, 0, L"MSTaskSwWClass", NULL);
					HWND TrayNotifyWnd = FindWindowEx(Shell_TrayWnd, 0, L"TrayNotifyWnd", NULL);

					HWND SysPager = FindWindowEx(TrayNotifyWnd, 0, L"SysPager", NULL);
					HWND ToolbarWindow32 = FindWindowEx(TrayNotifyWnd, 0, L"ToolbarWindow32", NULL);
					HWND Button = FindWindowEx(TrayNotifyWnd, 0, L"Button", NULL);

					//SendMessage(Shell_TrayWnd, WM_WINDOWPOSCHANGED, TRUE, 0);
					//PostMessage(tb, WM_DWMCOLORIZATIONCOLORCHANGED, NULL, NULL);
					//SendMessage(tb, WM_PARENTNOTIFY, 0x00000201, 0x0039065A);
					SendMessage(RebarWindow32, WM_SETREDRAW, FALSE, NULL);
					SendMessage(MSTaskSwWClass, WM_SETREDRAW, FALSE, NULL);
					SendMessage(GetParent(tb), WM_SETREDRAW, FALSE, NULL);

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

					int left;
					int top;
					int right;
					int bottom;

					if (taskbariscenter == 1) {
						if (sticky == 1) {
							left = (abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.right + 1) * curDPI / 100) - (((rect_TrayNotifyWnd.right - rect_TrayNotifyWnd.left) / 2) * curDPI / 100);
						}
						else {
							left = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.right + 1) * curDPI / 100;
						}

						top = 0 * curDPI / 100;
						right = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.left + 3) * curDPI / 100;
						bottom = rect_MSTaskSwWClass.bottom * curDPI / 100;

						if (left == 0) {
							//Not possible
							break;
						}
						if (left <= 0) {
							//Not possible
							break;
						}
						if (left >= 10000) {
							//Not possible
							break;
						}
					}

					if (taskbariscenter == 0) {
						left = abs(rect_Shell_TrayWnd.left + 1) * curDPI / 100;
						top = 0 * curDPI / 100;
						right = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.left + 3) * curDPI / 100;
						bottom = rect_MSTaskSwWClass.bottom * curDPI / 100;
						if (left >= 50) {
							//Not possible
							break;
						}
						if (left <= 0) {
							//Not possible
							break;
						}
					}

					if (sticky == 1) {
						// Cool feature but to have this running stable all the tray icons have to be disabled to prevent the system tray area from refreshing.

						ShowWindow(ToolbarWindow32, SW_HIDE);
						ShowWindow(SysPager, SW_HIDE);
						ShowWindow(Button, SW_HIDE);

						SetWindowPos(DesktopWindowContentBridge, NULL, -(((rect_TrayNotifyWnd.right - rect_TrayNotifyWnd.left) / 2)), 0, 0, 0, SWP_NOSIZE | SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSENDCHANGING);
						SetWindowPos(TrayNotifyWnd, NULL, rect_MSTaskSwWClass.right - ((rect_TrayNotifyWnd.right - rect_TrayNotifyWnd.left) / 2), 0, 0, 0, SWP_NOSIZE | SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSENDCHANGING);
					}
					else {
						ShowWindow(ToolbarWindow32, SW_SHOW);
						ShowWindow(SysPager, SW_SHOW);
						ShowWindow(Button, SW_SHOW);
					}

					HRGN region_ShellTrayWnd;
					HRGN region_TrayNotifyWnd;

					int trayleft = abs(rect_TrayNotifyWnd.left - rect_Shell_TrayWnd.left);

					if (square == 0) {
						region_ShellTrayWnd = CreateRoundRectRgn(left, top, right, bottom, corner_Radius, corner_Radius);
						region_TrayNotifyWnd = CreateRoundRectRgn(abs(rect_TrayNotifyWnd.left - rect_Shell_TrayWnd.left - 5) * curDPI / 100, top, abs(rect_TrayNotifyWnd.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100, bottom, corner_Radius, corner_Radius);
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
								std::wcout << Shell_TrayWnd << " @ " << "Shell_TrayWnd" << " has a maximized window!" << std::endl;
								HRGN region_Empty = CreateRectRgn(abs(rect_Shell_TrayWnd.left - rect_Shell_TrayWnd.left) * curDPI / 100, 0, abs(rect_Shell_TrayWnd.right - rect_Shell_TrayWnd.left) * curDPI / 100, rect_Shell_TrayWnd.bottom * curDPI / 100);
								SetWindowRgn(Shell_TrayWnd, region_Empty, TRUE);
								mtaskbar_Revert = 1;
								region_Empty = NULL;
								break;
							}
						}
					}

					if (mtaskbar_Revert == 0) {
						HRGN region_Both = CreateRectRgn(0, 0, 0, 0);
						if (hidetraywnd == 0) {
							if (smoothresize == 0) {
								CombineRgn(region_Both, region_ShellTrayWnd, region_TrayNotifyWnd, RGN_OR);
							}
							else {
								region_Both = region_ShellTrayWnd;
							}
						}
						else {
							region_Both = region_ShellTrayWnd;
						}

						RECT newtbrect;
						GetRgnBox(region_ShellTrayWnd, &newtbrect);

						if (newtbrect.right != abs(currenttbrect.right) * curDPI / 100 && newtbrect.right + 1 != abs(currenttbrect.right) * curDPI / 100 && newtbrect.right - 1 != abs(currenttbrect.right) * curDPI / 100) {
							if (smoothresize == 0) {
								SetWindowRgn(Shell_TrayWnd, region_Both, TRUE);
							}
							else {
								//std::thread{ SetWindowRegionAnimated, Shell_TrayWnd, region_Both }.detach();
								//SetWindowRegionAnimated(Shell_TrayWnd, region_Both);

								thread_List[thread_Count] = std::thread(SetWindowRegionAnimated, Shell_TrayWnd, region_Both);
								//thread_List[thread_Count] = 1;
								//
								thread_Count += 1;
								//std::thread th(SetWindowRegionAnimated, Shell_TrayWnd, region_Both);
								//th.join();
							}
						}
						else {
							if (rect_TrayNotifyWnd.left != trayleft) {
								SetWindowRgn(Shell_TrayWnd, region_Both, TRUE);
							}
							else {
								std::wcout << "Shell_TrayWnd" << " @ " << Shell_TrayWnd << " does not need new HRGN!" << std::endl;
							}
						}

						region_Both = NULL;
					}

					std::wcout << "Done with " << "Shell_TrayWnd" << " @ " << Shell_TrayWnd << std::endl;

					// dispose
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
					DesktopWindowContentBridge = NULL;
					ToolbarWindow32 = NULL;
					SysPager = NULL;
					Button = NULL;

					free(Shell_TrayWnd);
					free(Start);
					free(RebarWindow32);
					free(MSTaskSwWClass);
					free(TrayNotifyWnd);
					free(DesktopWindowContentBridge);
					free(ToolbarWindow32);
					free(SysPager);
					free(Button);

					//free(title);

					mtaskbar_Revert = 0;
				} //end primary taskbar

				// This is a secondary taskbar
				//if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {
				if (isprimarytaskbar == 0) {
					HWND Shell_SecondaryTrayWnd = tb;
					//HWND Start = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Start", NULL);
					HWND WorkerW = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"WorkerW", NULL);
					HWND DesktopWindowContentBridge = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
					HWND MSTaskListWClass = FindWindowEx(WorkerW, 0, L"MSTaskListWClass", NULL);

					//SendMessage(Shell_SecondaryTrayWnd, WM_WINDOWPOSCHANGED, TRUE, 0);

					//PostMessage(tb, WM_DWMCOLORIZATIONCOLORCHANGED, NULL, NULL);

					//SendMessage(tb, WM_PARENTNOTIFY, 0x00000201, 0x0039065A);

					SendMessage(WorkerW, WM_SETREDRAW, FALSE, NULL);
					SendMessage(MSTaskListWClass, WM_SETREDRAW, FALSE, NULL);
					SendMessage(GetParent(tb), WM_SETREDRAW, FALSE, NULL);

					HRGN currenttbreg = CreateRectRgn(0, 0, 0, 0);
					GetWindowRgn(tb, currenttbreg);
					RECT currenttbrect;
					GetRgnBox(currenttbreg, &currenttbrect);

					RECT rect_Shell_SecondaryTrayWnd;
					GetWindowRect(Shell_SecondaryTrayWnd, &rect_Shell_SecondaryTrayWnd);

					RECT rect_DesktopWindowContentBridge;
					GetWindowRect(DesktopWindowContentBridge, &rect_DesktopWindowContentBridge);

					//RECT rect_Start;
					//GetWindowRect(Start, &rect_Start);

					RECT rect_WorkerW;
					GetWindowRect(WorkerW, &rect_WorkerW);

					RECT rect_MSTaskListWClass;
					GetWindowRect(MSTaskListWClass, &rect_MSTaskListWClass);

					INT curDPI = GetDpiForWindow(Shell_SecondaryTrayWnd) * 1.041666666666667;

					int width_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.right - rect_Shell_SecondaryTrayWnd.left);
					int height_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.bottom - rect_Shell_SecondaryTrayWnd.top);

					int left;
					int top;
					int right;
					int bottom;

					if (taskbariscenter == 1) {
						left = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.right + 2) * curDPI / 100;
						top = 0 * curDPI / 100;
						right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 3) * curDPI / 100;
						bottom = rect_MSTaskListWClass.bottom * curDPI / 100;
						if (left == 0) {
							//Not possible
							break;
						}
						if (left <= 0) {
							//Not possible
							break;
						}
						if (left >= 10000) {
							//Not possible
							break;
						}
					}

					if (taskbariscenter == 0) {
						left = abs(rect_Shell_SecondaryTrayWnd.left - rect_Shell_SecondaryTrayWnd.left + 2) * curDPI / 100;
						top = 0 * curDPI / 100;
						right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 3) * curDPI / 100;
						bottom = rect_MSTaskListWClass.bottom * curDPI / 100;
						if (left >= 50) {
							//Not possible
							break;
						}
						if (left <= 0) {
							//Not possible
							break;
						}
					}

					HRGN region_Shell_SecondaryTrayWnd;

					if (square == 0) {
						region_Shell_SecondaryTrayWnd = CreateRoundRectRgn(left, top, right, bottom, corner_Radius, corner_Radius);
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
								std::wcout << "Shell_SecondaryTrayWnd" << " @ " << Shell_SecondaryTrayWnd << " has a maximized window!" << std::endl;
								HRGN region_Empty = CreateRectRgn(abs(rect_Shell_SecondaryTrayWnd.left - rect_Shell_SecondaryTrayWnd.left) * curDPI / 100, 0, abs(rect_Shell_SecondaryTrayWnd.right - rect_Shell_SecondaryTrayWnd.left) * curDPI / 100, rect_Shell_SecondaryTrayWnd.bottom * curDPI / 100);
								SetWindowRgn(Shell_SecondaryTrayWnd, region_Empty, TRUE);
								staskbar_Revert = 1;
								region_Empty = NULL;
								break;
							}
						}
					}

					if (staskbar_Revert == 0) {
						RECT newtbrect;
						GetRgnBox(region_Shell_SecondaryTrayWnd, &newtbrect);
						if (newtbrect.right != abs(currenttbrect.right) * curDPI / 100 && newtbrect.right - 1 != abs(currenttbrect.right) * curDPI / 100 && newtbrect.right + 1 != abs(currenttbrect.right) * curDPI / 100) {
							if (smoothresize == 0) {
								SetWindowRgn(Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd, TRUE);
							}
							else {
								//	std::thread{ SetWindowRegionAnimated, Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd }.detach();

									//thread_List[thread_Count] =

								thread_List[thread_Count] = std::thread(SetWindowRegionAnimated, Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd);
								//th.join();
								thread_Count += 1;

								//SetWindowRegionAnimated(Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd);
							}
						}
						else {
							std::wcout << "Shell_SecondaryTrayWnd" << " @ " << Shell_SecondaryTrayWnd << " does not need new HRGN!" << std::endl;
						}
						region_Shell_SecondaryTrayWnd = NULL;
					}

					std::wcout << "Done with " << "Shell_SecondaryTrayWnd" << " @ " << Shell_SecondaryTrayWnd << std::endl;

					// dispose
					left = NULL;
					right = NULL;
					bottom = NULL;
					top = NULL;
					currenttbreg = NULL;
					Shell_SecondaryTrayWnd = NULL;
					//Start = NULL;
					WorkerW = NULL;
					MSTaskListWClass = NULL;
					curDPI = NULL;
					width_Shell_SecondaryTrayWnd = NULL;
					height_Shell_SecondaryTrayWnd = NULL;
					DesktopWindowContentBridge = NULL;

					free(WorkerW);
					free(Shell_SecondaryTrayWnd);
					free(MSTaskListWClass);
					free(DesktopWindowContentBridge);

					//free(title);

					staskbar_Revert = 0;
				} //end secondary taskbar
				//title = NULL;
			}
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(100));

		for (std::thread& th : thread_List) {
			if (th.joinable() == TRUE) {
				th.join();
			}
		}

		working = 0;
	}
	catch (...) {}
}

BOOL CALLBACK EnumCallbackTaskbars(HWND hWND, LPARAM lParam) {
	//int length = 256;
	//wchar_t* title = new wchar_t[length];
	//GetClassName(hWND, title, length);

	HWND isataskbar = FindWindowEx(hWND, 0, L"Start", NULL); //All Taskbars have a Start button
	HWND isprimarytaskbar = FindWindowEx(hWND, 0, L"RebarWindow32", NULL); //Only the primary taskbar has a RebarWindow32

	//if (wcscmp(title, L"Shell_TrayWnd") == 0) {
	if (isataskbar != 0 && isprimarytaskbar != 0) {
		std::wcout << "Main taskbar found! @ hWid : " << hWND << std::endl;
		taskbar_List[taskbar_Count] = hWND;
		taskbar_Count += 1;
	}

	//if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {
	if (isataskbar != 0 && isprimarytaskbar == 0) {
		std::wcout << "A Secondary taskbar found! @ hWid : " << hWND << std::endl;
		taskbar_List[taskbar_Count] = hWND;
		taskbar_Count += 1;
	}

	hWND = NULL;
	//title = NULL;
	//length = NULL;

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
							maximized_List[maximized_Count] = hWND;
							maximized_Count += 1;
						}
					}
				}
			}
		}
	}

	//	if (oldMaxCount != maximized_Count) {
	//		for (HWND tb : taskbar_List) {
	//			if (tb != 0) {
	//				PostMessage(tb, WM_DWMCOLORIZATIONCOLORCHANGED, NULL, NULL);
	//				//setWindowlong(tb, WS_EX_NOREDIRECTIONBITMAP);
	//			}
	//		}
	//	}
		//
		//	HRGN region_Empty = CreateRectRgn(0, 0, 0, 0);
		//							SetWindowRgn(tb, region_Empty, FALSE);
		//}

	oldMaxCount = maximized_Count;

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