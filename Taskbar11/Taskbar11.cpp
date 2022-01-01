// TaskbarXI for Windows 11.
// By Chris Andriessen 2022

#define WIN32_LEAN_AND

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>


BOOL CALLBACK EnumCallbackFunction(HWND hWND, LPARAM lParam);

// Create a hwnd array with max 10
HWND taskbar_List[10];
int taskbar_Count;

int main() //WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    // Find all taskbar(s)
    EnumWindows(EnumCallbackFunction, NULL);


    for (;;) {
        for (HWND tb : taskbar_List)
        {
            if (tb != 0) {
                int length = 256;
                wchar_t* title = new wchar_t[length];
                GetClassName(tb, title, length);

                // This is the main taskbar
                if (wcscmp(title, L"Shell_TrayWnd") == 0) {

                    HWND Shell_TrayWnd = tb;
                    //HWND Start = FindWindowEx(Shell_TrayWnd, 0, L"Start", NULL);
                    HWND RebarWindow32 = FindWindowEx(Shell_TrayWnd, 0, L"RebarWindow32", NULL);
                    HWND MSTaskSwWClass = FindWindowEx(RebarWindow32, 0, L"MSTaskSwWClass", NULL);

                    RECT rect_Shell_TrayWnd;
                    GetWindowRect(Shell_TrayWnd, &rect_Shell_TrayWnd);

                    //RECT rect_Start;
                    //GetWindowRect(Start, &rect_Start);

                    RECT rect_MSTaskSwWClass;
                    GetWindowRect(MSTaskSwWClass, &rect_MSTaskSwWClass);

                    INT curDPI = GetDpiForWindow(Shell_TrayWnd) * 1.041666666666667;

                    int width_Shell_TrayWnd = (rect_Shell_TrayWnd.right - rect_Shell_TrayWnd.left);
                    int height_Shell_TrayWnd = (rect_Shell_TrayWnd.bottom - rect_Shell_TrayWnd.top);

                    int left = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.left + 2) * curDPI / 100;
                    int top = 2 * curDPI / 100;
                    int right = abs(rect_MSTaskSwWClass.right - rect_Shell_TrayWnd.right + 1) * curDPI / 100;
                    int bottom = rect_MSTaskSwWClass.bottom * curDPI / 100;

                    HRGN region_ShellTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 10, 10);
                    SetWindowRgn(Shell_TrayWnd, region_ShellTrayWnd, TRUE);

                }

                // This is a secondary taskbar
                if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {

                    HWND Shell_SecondaryTrayWnd = tb;
                    //HWND Start = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Start", NULL);
                    HWND WorkerW = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"WorkerW", NULL);
                    HWND MSTaskListWClass = FindWindowEx(WorkerW, 0, L"MSTaskListWClass", NULL);

                    RECT rect_Shell_SecondaryTrayWnd;
                    GetWindowRect(Shell_SecondaryTrayWnd, &rect_Shell_SecondaryTrayWnd);

                    //RECT rect_Start;
                    //GetWindowRect(Start, &rect_Start);

                    RECT rect_MSTaskListWClass;
                    GetWindowRect(MSTaskListWClass, &rect_MSTaskListWClass);

                    INT curDPI = GetDpiForWindow(Shell_SecondaryTrayWnd) * 1.041666666666667;

                    int width_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.right - rect_Shell_SecondaryTrayWnd.left);
                    int height_Shell_SecondaryTrayWnd = (rect_Shell_SecondaryTrayWnd.bottom - rect_Shell_SecondaryTrayWnd.top);

                    int left = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 2) * curDPI / 100;
                    int top = 2 * curDPI / 100;
                    int right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.right + 1) * curDPI / 100;
                    int bottom = rect_MSTaskListWClass.bottom * curDPI / 100;


                    //std::wcout << (left) << std::endl;

                    HRGN region_Shell_SecondaryTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 10, 10);
                    SetWindowRgn(Shell_SecondaryTrayWnd, region_Shell_SecondaryTrayWnd, TRUE);

                }

            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

}


BOOL CALLBACK EnumCallbackFunction(HWND hWND, LPARAM lParam) {

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