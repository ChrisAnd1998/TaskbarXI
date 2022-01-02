// TaskbarXI for Windows 11.
// By Chris Andriessen 2022

#pragma comment (lib, "dwmapi.lib")

#define WIN32_LEAN_AND_MEAN
#define NOT_BUILD_WINDOWS_DEPRECATE

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <dwmapi.h>


BOOL CALLBACK EnumCallbackTaskbars(HWND hWND, LPARAM lParam);
BOOL CALLBACK EnumCallbackMaximized(HWND hWND, LPARAM lParam);

HWND maximized_List[10];
HWND taskbar_List[10];
int taskbar_Count;
int maximized_Count;

int mtaskbar_Revert;
int staskbar_Revert;

int main() //WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    std::wcout << "Initializing..." << std::endl;

    std::wcout << "Looking for taskbars..." << std::endl;

    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    // Find all taskbar(s)
    EnumWindows(EnumCallbackTaskbars, NULL);

    std::wcout << "Initialize complete!" << std::endl;
    std::wcout << "Application is running!" << std::endl;

    for (;;) {

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


                int length = 256;
                wchar_t* title = new wchar_t[length];
                GetClassName(tb, title, length);

              
                // Check if hWid is still valid if not find again
                if (wcscmp(title, L"") == 0) {

                    std::wcout << "hWID invalid!" << std::endl;

                    HWND Explorer = NULL;

                    do
                    {
                        std::wcout << "Looking for Explorer..." << std::endl;
                        Explorer = FindWindow(0, L"Shell_TrayWnd");
                        std::this_thread::sleep_for(std::chrono::milliseconds(250));
                    } while (Explorer != 0);

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
                  
                    std::wcout << "Initializing..." << std::endl;
                    std::wcout << "Looking for taskbars..." << std::endl;

                    EnumWindows(EnumCallbackTaskbars, NULL);

                    std::wcout << "Initialize complete!" << std::endl;
                    std::wcout << "Application is running!" << std::endl;
                } // end failsafe
               

                // This is the main taskbar
                if (wcscmp(title, L"Shell_TrayWnd") == 0) {

                    HWND Shell_TrayWnd = tb;
                    HWND Start = FindWindowEx(Shell_TrayWnd, 0, L"Start", NULL);
                    HWND RebarWindow32 = FindWindowEx(Shell_TrayWnd, 0, L"RebarWindow32", NULL);
                    HWND MSTaskSwWClass = FindWindowEx(RebarWindow32, 0, L"MSTaskSwWClass", NULL);
                    HWND TrayNotifyWnd = FindWindowEx(Shell_TrayWnd, 0, L"TrayNotifyWnd", NULL);


                    RECT rect_Shell_TrayWnd;
                    GetWindowRect(Shell_TrayWnd, &rect_Shell_TrayWnd);

                    RECT rect_Start;
                    GetWindowRect(Start, &rect_Start);

                    RECT rect_TrayNotifyWnd;
                    GetWindowRect(TrayNotifyWnd, &rect_TrayNotifyWnd);

                    RECT rect_MSTaskSwWClass;
                    GetWindowRect(MSTaskSwWClass, &rect_MSTaskSwWClass);

                    INT curDPI = GetDpiForWindow(Shell_TrayWnd) * 1.041666666666667;

                    int width_Shell_TrayWnd = (rect_Shell_TrayWnd.right - rect_Shell_TrayWnd.left);
                    int height_Shell_TrayWnd = (rect_Shell_TrayWnd.bottom - rect_Shell_TrayWnd.top);

                    int left = abs(rect_Start.left - rect_Shell_TrayWnd.left + 1) * curDPI / 100;
                    int top = 2 * curDPI / 100;
                    int right = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100;
                    int bottom = rect_MSTaskSwWClass.bottom * curDPI / 100;

                    HRGN region_ShellTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 15, 15);
                    HRGN region_TrayNotifyWnd = CreateRoundRectRgn(abs(rect_TrayNotifyWnd.left - rect_Shell_TrayWnd.left + 2) * curDPI / 100, top, abs(rect_TrayNotifyWnd.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100, bottom, 15, 15);

                   // HRGN region_ShellTrayWnd = CreateRectRgn(left, top, right, bottom);
                   // HRGN region_TrayNotifyWnd = CreateRectRgn(abs(rect_TrayNotifyWnd.left - rect_Shell_TrayWnd.left + 2) * curDPI / 100, top, abs(rect_TrayNotifyWnd.right - rect_Shell_TrayWnd.left + 1) * curDPI / 100, bottom);
                    
                    mtaskbar_Revert = 0;
                    for (HWND mx1 : maximized_List) {
                        if (mx1 != 0) {
                            HMONITOR tbm1 = MonitorFromWindow(Shell_TrayWnd, MONITOR_DEFAULTTONEAREST);
                            HMONITOR wm1 = MonitorFromWindow(mx1, MONITOR_DEFAULTTONEAREST);
                            if (tbm1 == wm1) {
                                std::wcout << "The main taskbar has a maximized window!" << std::endl;
                                HRGN region_Empty = CreateRectRgn(abs(rect_Shell_TrayWnd.left - rect_Shell_TrayWnd.left) * curDPI / 100, 0, abs(rect_Shell_TrayWnd.right - rect_Shell_TrayWnd.left) * curDPI / 100, rect_Shell_TrayWnd.bottom * curDPI / 100);
                                SetWindowRgn(Shell_TrayWnd, region_Empty, TRUE);
                                mtaskbar_Revert = 1;
                            }
                        }
                    }

                 
                    if (mtaskbar_Revert == 0) {
                        HRGN region_Both = CreateRectRgn(0, 0, 0, 0);
                        CombineRgn(region_Both, region_ShellTrayWnd, region_TrayNotifyWnd, RGN_OR);
                        SetWindowRgn(Shell_TrayWnd, region_Both, TRUE);
                    }

                    mtaskbar_Revert = 0;

                    std::wcout << "Done with the main taskbar." << std::endl;
                } //end primary taskbar

          

                // This is a secondary taskbar
                if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {

                    HWND Shell_SecondaryTrayWnd = tb;
                    HWND Start = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Start", NULL);
                    HWND WorkerW = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"WorkerW", NULL);
                    HWND MSTaskListWClass = FindWindowEx(WorkerW, 0, L"MSTaskListWClass", NULL);

                    RECT rect_Shell_SecondaryTrayWnd;
                    GetWindowRect(Shell_SecondaryTrayWnd, &rect_Shell_SecondaryTrayWnd);

                    RECT rect_Start;
                    GetWindowRect(Start, &rect_Start);

                    RECT rect_WorkerW;
                    GetWindowRect(WorkerW, &rect_WorkerW);

                    RECT rect_MSTaskListWClass;
                    GetWindowRect(MSTaskListWClass, &rect_MSTaskListWClass);

                    INT curDPI = GetDpiForWindow(Shell_SecondaryTrayWnd) * 1.041666666666667;

                    int width_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.right - rect_Shell_SecondaryTrayWnd.left);
                    int height_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.bottom - rect_Shell_SecondaryTrayWnd.top);                 

                    staskbar_Revert = 0;
                    for (HWND mx2 : maximized_List) {
                        if (mx2 != 0) {
                            HMONITOR tbm2 = MonitorFromWindow(Shell_SecondaryTrayWnd, MONITOR_DEFAULTTONEAREST);
                            HMONITOR wm2 = MonitorFromWindow(mx2, MONITOR_DEFAULTTONEAREST);
                            if (tbm2 == wm2) {
                                std::wcout << "A secondary taskbar has a maximized window! " << std::endl;
                                HRGN region_Empty = CreateRectRgn(abs(rect_Shell_SecondaryTrayWnd.left - rect_Shell_SecondaryTrayWnd.left) * curDPI / 100, 0, abs(rect_Shell_SecondaryTrayWnd.right - rect_Shell_SecondaryTrayWnd.left) * curDPI / 100, rect_Shell_SecondaryTrayWnd.bottom * curDPI / 100);
                                SetWindowRgn(Shell_SecondaryTrayWnd, region_Empty, TRUE);
                                staskbar_Revert = 1;
                            }
                        }
                    }

                    if (staskbar_Revert == 0) {
                        HKEY hKey;
                        DWORD buffer;
                        LONG result;
                        unsigned long type = REG_DWORD, size = 1024;

                        result = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, KEY_READ, &hKey);
                        if (result == ERROR_SUCCESS)
                        {
                            RegQueryValueEx(hKey, L"TaskbarAl", NULL, &type, (LPBYTE)&buffer, &size);
                            RegCloseKey(hKey);

                            //Center
                            if (buffer == 1) {
                                int left = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 2) * curDPI / 100;
                                int top = 2 * curDPI / 100;
                                int right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.right + 1) * curDPI / 100;
                                int bottom = rect_MSTaskListWClass.bottom * curDPI / 100;

                                HRGN region_Shell_SecondaryTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 10, 10);
                                //HRGN region_Shell_SecondaryTrayWnd = CreateRectRgn(left, top, right, bottom);
                                SetWindowRgn(Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd, TRUE);
                            }

                            //Left
                            if (buffer == 0) {
                                int left = abs(rect_Start.left - rect_Shell_SecondaryTrayWnd.left + 2) * curDPI / 100;
                                int top = 2 * curDPI / 100;
                                int right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 1) * curDPI / 100;
                                int bottom = rect_MSTaskListWClass.bottom * curDPI / 100;

                                HRGN region_Shell_SecondaryTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 10, 10);
                                //HRGN region_Shell_SecondaryTrayWnd = CreateRectRgn(left, top, right, bottom);
                                SetWindowRgn(Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd, TRUE);
                            }
                        }
                    }

                    staskbar_Revert = 0;

                    std::wcout << "Done with a secondary taskbar." << std::endl;
                } //end secondary taskbar
                

            }
        }
        std::wcout << "Done with all taskbars. Sleeping for 250 milliseconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

       
    }
}


BOOL CALLBACK EnumCallbackTaskbars(HWND hWND, LPARAM lParam) {

    int length = 256;
    wchar_t* title = new wchar_t[length];

    GetClassName(hWND, title, length);

    if (wcscmp(title, L"Shell_TrayWnd") == 0) {
        std::wcout << "Main taskbar found! @ hWid : " << hWND << std::endl;
        taskbar_List[taskbar_Count] = hWND;
        taskbar_Count += 1;
    }

    if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {
        std::wcout << "A Secondary taskbar found! @ hWid : " << hWND << std::endl;
        taskbar_List[taskbar_Count] = hWND;
        taskbar_Count += 1;
    }



    return true;
}



BOOL CALLBACK EnumCallbackMaximized(HWND hWND, LPARAM lParam) {

    WINDOWPLACEMENT wp;
    GetWindowPlacement(hWND, &wp);
    INT wl = GetWindowLong(hWND, GWL_STYLE);
    INT Cloaked;
    INT cl = DwmGetWindowAttribute(hWND, DWMWINDOWATTRIBUTE::DWMWA_CLOAKED, &Cloaked, sizeof(Cloaked));

    // Look for non Phanthom maximized windows
    if (wp.showCmd == 3) {
        if (Cloaked == 0) {
            if (wl and WS_VISIBLE == WS_VISIBLE) {
                //std::wcout << hWND << std::endl;
                maximized_List[maximized_Count] = hWND;
                maximized_Count += 1;
            }
        }
    }

    return true;
}


