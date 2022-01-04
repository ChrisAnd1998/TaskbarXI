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
#include <iostream>    
#include <stdio.h>



std::string cur_dir;

int working;

BOOL CALLBACK EnumCallbackTaskbars(HWND hWND, LPARAM lParam);
BOOL CALLBACK EnumCallbackMaximized(HWND hWND, LPARAM lParam);

HWND maximized_List[10];
HWND taskbar_List[10];
int taskbar_Count;
int maximized_Count;

int mtaskbar_Revert;
int staskbar_Revert;

int trayleft;


VOID SetTaskbar();

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
   
    if (working == 0) {
        SetTaskbar();
    }
    
}




//int main(int argc, char* argv[]) 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    
    

    working = 1;
    //SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);
   // SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_DESTROY, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);
   // SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, NULL, WinEventProcCallback, 0, 0, WINEVENT_SKIPOWNPROCESS);

    SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

    std::wcout << "Initializing..." << std::endl;

    std::wcout << "Looking for taskbars..." << std::endl;

    //::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    FreeConsole();
    
    

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

    std::wcout << "Initialize complete!" << std::endl;
    std::wcout << "Application is running!" << std::endl;


    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");


    cur_dir = std::string(buffer);// (argv[0]);

   
    //DWORD GetCurrentProcessId(HANDLE ytt);

 

   

    

   // std::wstring chars = L"";
   // chars += (wchar_t)34;
   // std::string quote(chars.begin(), chars.end());

   // WinExec((quote + cur_dir + quote).c_str(), SW_HIDE);





    SetTaskbar();
    
   // MSG msg;
    for (;;) {//while (GetMessage(&msg, NULL, 0, 0)) {

        //TranslateMessage(&msg);
        //DispatchMessage(&msg);
        SetTaskbar();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

}


void SetTaskbar() {
    std::cout.clear();

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
            Explorer = FindWindow(0, L"Shell_TrayWnd");
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        } while (Explorer != 0);

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

        // std::this_thread::sleep_for(std::chrono::milliseconds(500));



        std::wstring chars = L"";
        chars += (wchar_t)34;
        std::string quote(chars.begin(), chars.end());

        WinExec((quote + cur_dir + quote).c_str(), SW_HIDE);

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
                    Explorer = FindWindow(0, L"Shell_TrayWnd");
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                } while (Explorer != 0);

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

                

               WinExec((quote + cur_dir + quote).c_str(), SW_HIDE);

               //WinExec((quote + cur_dir + quote).c_str(), SW_HIDE);

               //WinExec(("taskkill /F /IM " + quote + cur_dir.substr(pos + 1) + quote).c_str(), SW_HIDE);

                std::this_thread::sleep_for(std::chrono::milliseconds(500));


                exit(0);
                abort;
                
            } // end failsafe


            // This is the main taskbar
            if (wcscmp(title, L"Shell_TrayWnd") == 0) {

                HWND Shell_TrayWnd = tb;
                HWND Start = FindWindowEx(Shell_TrayWnd, 0, L"Start", NULL);
                //HWND DesktopWindowContentBridge = FindWindowEx(Shell_TrayWnd, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
                HWND RebarWindow32 = FindWindowEx(Shell_TrayWnd, 0, L"RebarWindow32", NULL);
                HWND MSTaskSwWClass = FindWindowEx(RebarWindow32, 0, L"MSTaskSwWClass", NULL);
                HWND TrayNotifyWnd = FindWindowEx(Shell_TrayWnd, 0, L"TrayNotifyWnd", NULL);

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

                // SetWindowPos(TrayNotifyWnd, NULL, rect_MSTaskSwWClass.right, 0, 0, 0, SWP_NOSIZE | SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSENDCHANGING);

                RECT rect_TrayNotifyWnd;
                GetWindowRect(TrayNotifyWnd, &rect_TrayNotifyWnd);

                

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
                    CombineRgn(region_Both, region_ShellTrayWnd, region_TrayNotifyWnd, RGN_OR);

                    RECT newtbrect;
                    GetRgnBox(region_ShellTrayWnd, &newtbrect);

                   // std::wcout << rect_TrayNotifyWnd.left << " " << trayleft << std::endl;


                    if (newtbrect.left != abs(currenttbrect.left) * curDPI / 100) {
                        SetWindowRgn(Shell_TrayWnd, region_Both, TRUE);
                    } else {
                        if (rect_TrayNotifyWnd.left != trayleft) {
                            SetWindowRgn(Shell_TrayWnd, region_Both, TRUE);
                            trayleft = rect_TrayNotifyWnd.left;
                            //SendMessage(tb, WM_SETREDRAW, TRUE, NULL);
                        } else {
                            std::wcout << title << " @ " << Shell_TrayWnd << " does not need new HRGN!" << std::endl;
                        }
                        
                    }

                    region_Both = NULL;
                }

                mtaskbar_Revert = 0;

               

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




                
            } //end primary taskbar



            // This is a secondary taskbar
            if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {

                HWND Shell_SecondaryTrayWnd = tb;
                HWND Start = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Start", NULL);
                HWND WorkerW = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"WorkerW", NULL);
                HWND DesktopWindowContentBridge = FindWindowEx(Shell_SecondaryTrayWnd, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
                HWND MSTaskListWClass = FindWindowEx(WorkerW, 0, L"MSTaskListWClass", NULL);

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


                            left = NULL;
                            right = NULL;
                            bottom = NULL;
                            top = NULL;
                            region_Shell_SecondaryTrayWnd = NULL;
                        }

                        //Left
                        if (buffer == 0) {
                            int left = abs(rect_Start.left - rect_Shell_SecondaryTrayWnd.left + 2) * curDPI / 100;
                            int top = 2 * curDPI / 100;
                            int right = abs(rect_MSTaskListWClass.right - rect_Shell_SecondaryTrayWnd.left + 1) * curDPI / 100;
                            int bottom = rect_MSTaskListWClass.bottom * curDPI / 100;

                            HRGN region_Shell_SecondaryTrayWnd = CreateRoundRectRgn(left, top, right, bottom, 10, 10);
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


                            left = NULL;
                            right = NULL;
                            bottom = NULL;
                            top = NULL;
                            region_Shell_SecondaryTrayWnd = NULL;
                        }
                    }

                    hKey = NULL;
                    buffer = NULL;
                    result = NULL;
                    type = NULL;
                    size = NULL;

                }

                staskbar_Revert = 0;


                std::wcout << "Done with " << title << " @ " << Shell_SecondaryTrayWnd << std::endl;

                // dispose
                currenttbreg = NULL;
                Shell_SecondaryTrayWnd = NULL;
                Start = NULL;
                WorkerW = NULL;
                MSTaskListWClass = NULL;
                curDPI = NULL;
                width_Shell_SecondaryTrayWnd = NULL;
                height_Shell_SecondaryTrayWnd = NULL;




               
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
        std::wcout << "Main taskbar found! @ hWid : " << hWND << std::endl;
        taskbar_List[taskbar_Count] = hWND;
        taskbar_Count += 1;
    }

    if (wcscmp(title, L"Shell_SecondaryTrayWnd") == 0) {
        std::wcout << "A Secondary taskbar found! @ hWid : " << hWND << std::endl;
        taskbar_List[taskbar_Count] = hWND;
        taskbar_Count += 1;
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
    if (wp.showCmd == 3) {
        if (Cloaked == 0) {
            if (wl and WS_VISIBLE == WS_VISIBLE) {
                //std::wcout << hWND << std::endl;
                maximized_List[maximized_Count] = hWND;
                maximized_Count += 1;
            }
        }
    }

    wp.length = NULL;
    wl = NULL;
    Cloaked = NULL;
    cl = NULL;

    return true;
}


