// Fate Client Dll Injector.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Fate Client Dll Injector.h"

#include "inject.h"
#include "FixFilePerms.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;
json config;

char working_dir[1024]; //create string buffer to hold path

#define MAX_LOADSTRING 130




#define ID_INJECT_BUTTON 100
#define ID_SELECT_BUTTON 101
#define ID_CHECKPROC_BUTTON 102
#define ID_CHECKAINJECT_BUTTON 103
#define ID_HIDEMENU_BUTTON 104
#define ID_DEBUG_LABEL 130

#define APPWM_ICONNOTIFY 110

HMENU hmenu;
#define IDR_QUIT 120
#define IDR_STOPAUTO 121
#define IDR_OPEN 122
#define IDR_INJECT 123

int debugColorFlag = 0;
bool autoinjectstate = false;


HWND hEditProcName;
HWND hEditDllPath;
HWND hDebugLabel;
HWND hCheckProc;
HWND hCheckAInject;
HWND hAutoDelay;
HWND hHideMenu;

NOTIFYICONDATA nid;



// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);




bool loopInject() {
    std::string debug;
    char procName[256];
    GetWindowText(hEditProcName, procName, 256);
    char delaystr[256];
    GetWindowText(hAutoDelay, delaystr, 256);
    char dllPath[1024];
    GetWindowText(hEditDllPath, dllPath, 1024);


    int delay = atoi(delaystr);

    if (delay <= 1) {
        delay = 1;
        SetWindowText(hAutoDelay, "1");
        debug = "AutoInject: Enabled | trying every second";
    } else {
        debug = "AutoInject: Enabled | trying every " + std::to_string(delay) + " seconds";
    }
    SetWindowText(hDebugLabel, debug.c_str());
    debugColorFlag = 0;
    InvalidateRect(hDebugLabel, NULL, TRUE);


    DWORD procId = 0;
    DWORD oldProcId = 0;

    delay *= 1000;

    while (autoinjectstate) {
        
        while (procId == oldProcId || procId == 0) {
            Sleep(delay);
            procId = GetProcId(procName);
            if (procId == 0) {
                debug = "AutoInject: Can't find process! | " + std::to_string(procId);
                SetWindowText(hDebugLabel, debug.c_str());
                debugColorFlag = 1;
                InvalidateRect(hDebugLabel, NULL, TRUE);
            }
            else if (procId == oldProcId) {
                debug = "AutoInject: Already Injected! | " + std::to_string(procId);
                SetWindowText(hDebugLabel, debug.c_str());
                debugColorFlag = 2;
                InvalidateRect(hDebugLabel, NULL, TRUE);
            }
            if (autoinjectstate == false) {
                return false;
            }
        }
        std::ifstream test(dllPath); // test if file path is valid
        if (!test) {

            debug = "AutoInject: Process found! | " + std::to_string(procId) + " | invalid file path";
            SetWindowText(hDebugLabel, debug.c_str());
            debugColorFlag = 1;
            InvalidateRect(hDebugLabel, NULL, TRUE);
            return true;
        }
        size_t len = strlen(dllPath);
        std::wstring ws(&dllPath[0], &dllPath[len]);
        SetAccessControl(ws, L"S-1-15-2-1");
        performInjection(procId, dllPath);
        debug = "Process found! | " + std::to_string(procId) + " | valid file path | Injected!";
        SetWindowText(hDebugLabel, debug.c_str());
        debugColorFlag = 2;
        InvalidateRect(hDebugLabel, NULL, TRUE);
        oldProcId = procId;

        config["settings"]["delaystr"] = delaystr;
        config["settings"]["procName"] = procName;
        std::string temp = "\\config.json";
        std::ofstream save(working_dir + temp);
        save << std::setw(4) << config << std::endl;
    }
    return false;
}




bool clickInject() {
    std::string debug;
    char procName[1024];
    GetWindowText(hEditProcName, procName, 1024);
    char dllPath[1024];
    GetWindowText(hEditDllPath, dllPath, 1024);

    DWORD procId = 0;

    procId = GetProcId(procName);


    if (procId == 0) {
        debug = "Can't find process! | " + std::to_string(procId);
        SetWindowText(hDebugLabel, debug.c_str());
        debugColorFlag = 1;
        InvalidateRect(hDebugLabel, NULL, TRUE);
        return true;
    }
    std::ifstream test(dllPath); // test if file path is valid
    if (!test) {

        debug = "Process found! | " + std::to_string(procId) + " | invalid file path";
        SetWindowText(hDebugLabel, debug.c_str());
        debugColorFlag = 1;
        InvalidateRect(hDebugLabel, NULL, TRUE);
        return true;
    }
    size_t len = strlen(dllPath);
    std::wstring ws(&dllPath[0], &dllPath[len]);
    SetAccessControl(ws, L"S-1-15-2-1");
    performInjection(procId, dllPath);
    debug = "Process found! | " + std::to_string(procId) + " | valid file path | Injected!";
    SetWindowText(hDebugLabel, debug.c_str());
    debugColorFlag = 2;
    InvalidateRect(hDebugLabel, NULL, TRUE);


    char delaystr[0xF];
    GetWindowText(hAutoDelay, delaystr, 0xF);
    config["settings"]["delaystr"] = delaystr;
    config["settings"]["procName"] = procName;
    std::string temp = "\\config.json";
    std::ofstream save(working_dir + temp);
    save << std::setw(4) << config << std::endl;
    return false;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int nCmdShow)
{


    char*shut_up_VS = _getcwd(working_dir, FILENAME_MAX);


    std::ifstream input("config.json");


    if (input.is_open()) {

        input >> config; // read json

    }else {
        if (config["settings"].is_null()) { // if settings are missing reset settings

            std::ofstream create("config.json");
            config = {};
            config["settings"]["customProcName"] = false;
            config["settings"]["procName"] = "minecraft.windows.exe";
            config["settings"]["dllPath"] = "no path defined";
            config["settings"]["delaystr"] = "5";
            config["unbrandInjector"] = false;
            create << std::setw(4) << config << std::endl;
        }
    }
    
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    LoadStringW(hInstance, IDC_FATECLIENTDLLINJECTOR, szWindowClass, MAX_LOADSTRING);
    
    if (config["unbrandInjector"]) {
        LoadStringW(hInstance, IDS_APP_TITLE2, szTitle, MAX_LOADSTRING); // for unbranding
    }
    else {
        LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    }



    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    // Main message loop:
    while (true) {
        BOOL result = GetMessage(&msg, 0, 0, 0);
        if (result > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            return result;
        }

    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    if (config["unbrandInjector"]) {
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    }
    else {
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    }
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    if (config["unbrandInjector"]) {
        wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    }
    else {
        wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    }



    return RegisterClassExW(&wcex);
}


//   FUNCTION: InitInstance(HINSTANCE, int)
//   PURPOSE: Saves instance handle and creates main window
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable


   HFONT hFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
   HFONT hFont2 = CreateFont(18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

   HWND hButtonInject;
   HWND hButtonSelectDll;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU /*| WS_THICKFRAME*/ | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, 0, 350, 175, 0, 0, hInstance, 0);
   
   if (!hWnd)
   {
      return FALSE; // is window present
   }
   
   
   hButtonInject = CreateWindow("button", "Inject", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 10, 10, 100, 40, hWnd, (HMENU)ID_INJECT_BUTTON, hInstance, 0); // inject Button
   SendMessage(hButtonInject, WM_SETFONT, WPARAM(hFont2), true);

   hCheckProc = CreateWindow("button", "Custom target", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 122, 36, 105, 20, hWnd, (HMENU)ID_CHECKPROC_BUTTON, hInstance, 0); // Checkbox
   SendMessage(hCheckProc, WM_SETFONT, WPARAM(hFont), true);


   std::string temp = "minecraft.windows.exe";

   if (config["settings"]["customProcName"]) { // decide vs custom or "minecraft.windows.exe"
       temp = config["settings"]["procName"];
   }
   else {
       config["settings"]["procName"] = temp;
   }

   
   
   
   hEditProcName = CreateWindow("edit", temp.c_str(), WS_TABSTOP | WS_CHILD | WS_BORDER | EM_LINELENGTH | WS_VISIBLE, 120, 10, 200, 22, hWnd, 0, hInstance, 0);
   SendMessage(hEditProcName, WM_SETFONT, WPARAM(hFont2), true);

   hCheckAInject = CreateWindow("button", "Auto Inject", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 122, 56, 105, 20, hWnd, (HMENU)ID_CHECKAINJECT_BUTTON, hInstance, 0); // Checkbox
   SendMessage(hCheckAInject, WM_SETFONT, WPARAM(hFont), true);



   SendMessage(hAutoDelay, EM_SETLIMITTEXT, 2, true);

   hHideMenu = CreateWindow("button", "Hide Menu", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 10, 54, 100, 22, hWnd, (HMENU)ID_HIDEMENU_BUTTON, hInstance, 0); // Select Dll
   SendMessage(hHideMenu, WM_SETFONT, WPARAM(hFont), true);

   hButtonSelectDll = CreateWindow("button", "Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 10, 80, 60, 22, hWnd, (HMENU)ID_SELECT_BUTTON, hInstance, 0); // Select Dll
   SendMessage(hButtonSelectDll, WM_SETFONT, WPARAM(hFont), true);

   temp = config["settings"]["dllPath"];  // load dll path 
   hEditDllPath = CreateWindow("edit", temp.c_str(), WS_TABSTOP | WS_CHILD | WS_BORDER | EM_LINELENGTH | WS_VISIBLE, 80, 80, 240, 22, hWnd, 0, hInstance, 0);
   SendMessage(hEditDllPath, WM_SETFONT, WPARAM(hFont2), true);

   hDebugLabel = CreateWindow("static", "Version 0.9 | Made by youtube.com/Fligger", WS_CHILD | WS_VISIBLE, 10, 110, 320, 22, hWnd, (HMENU)ID_DEBUG_LABEL, hInstance, 0);
   SendMessage(hDebugLabel, WM_SETFONT, WPARAM(hFont), true);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    POINT pt; // for receiving the mouse coordinates
    CheckDlgButton(hWnd, ID_CHECKPROC_BUTTON, config["settings"]["customProcName"]);
    Edit_Enable(hEditProcName, config["settings"]["customProcName"] && !autoinjectstate);       // checkbox states
    Edit_Enable(hEditDllPath, !autoinjectstate);    //disable path bar while in auto inject

    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId) {
        case ID_CHECKPROC_BUTTON: {

            if (IsDlgButtonChecked(hWnd, ID_CHECKPROC_BUTTON)) { // custom procName checkbox control
                SetWindowText(hEditProcName, "minecraft.windows.exe");
                config["settings"]["customProcName"] = false;
            }
            else {
                config["settings"]["customProcName"] = true;
            }
            break;
        }
        case ID_CHECKAINJECT_BUTTON: {      // auto inject checkbox
            if (IsDlgButtonChecked(hWnd, ID_CHECKAINJECT_BUTTON)) {
                CheckDlgButton(hWnd, ID_CHECKAINJECT_BUTTON, BST_UNCHECKED);
                Edit_Enable(hAutoDelay, true);
                Edit_Enable(hCheckProc, true);
                autoinjectstate = false;
            }
            else {
                CheckDlgButton(hWnd, ID_CHECKAINJECT_BUTTON, BST_CHECKED);
                Edit_Enable(hAutoDelay, false);
                autoinjectstate = true;
                std::thread loopthread(loopInject); // for autoinject
                Edit_Enable(hCheckProc, false);
                loopthread.detach();

            }
            break;
        }
        case ID_SELECT_BUTTON: {
            if (autoinjectstate) {      //if in autoinject mode
                SetWindowText(hDebugLabel, "Disable auto inject to select new file!");
                debugColorFlag = 1;
                InvalidateRect(hDebugLabel, NULL, TRUE);
            }
            else {
                char dllPath[1024]; // select dll dialog
                dllPath[0] = 0;
                char dllName[1024];
                dllName[0] = 0;
                OPENFILENAME openDialog = {};
                openDialog.lStructSize = sizeof(OPENFILENAME);
                openDialog.hwndOwner = hWnd;
                openDialog.lpstrFile = dllPath;
                openDialog.nMaxFile = sizeof(dllPath);
                openDialog.lpstrFilter = "Show all files (*.*)\0*.*\0Dynamic link library (*.dll)\0*.dll";
                openDialog.lpstrInitialDir = 0;
                openDialog.lpstrDefExt = ".dll";
                if (GetOpenFileName(&openDialog) != 0) {
                    SetWindowText(hEditDllPath, dllPath);
                    config["settings"]["dllPath"] = dllPath;

                    std::string temp = "\\config.json";
                    std::ofstream save(working_dir + temp);
                    save << std::setw(4) << config << std::endl;
                }
            }
            break;
        }
        case ID_INJECT_BUTTON: {
            clickInject();
            break;
        }

        case ID_HIDEMENU_BUTTON: {
            ShowWindow(hWnd, SW_HIDE);
            break;
        }
        default: {
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        if ((HWND)lParam == GetDlgItem(hWnd, ID_DEBUG_LABEL)) {

            switch (debugColorFlag) {       // debug text color defines
            case 0:
                SetTextColor((HDC)wParam, RGB(0, 0, 0)); break; // black
            case 1:
                SetTextColor((HDC)wParam, RGB(255, 0, 0)); break; // red
            case 2:
                SetTextColor((HDC)wParam, RGB(0, 0, 255)); break; // blue
            }

            return LRESULT(GetStockObject(WHITE_BRUSH)); // element background color

        }
        if ((HWND)lParam == GetDlgItem(hWnd, ID_CHECKPROC_BUTTON)) {  // element background color

            return LRESULT(GetStockObject(WHITE_BRUSH));
        }
        if ((HWND)lParam == GetDlgItem(hWnd, ID_CHECKAINJECT_BUTTON)) {// element background color

            return LRESULT(GetStockObject(WHITE_BRUSH));
        }
        break;
    }
    case WM_CREATE: {
        // Notification tray stuff
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = 0;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER;
        if (config["unbrandInjector"]) {
            nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
        }
        else {
            nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
        }

        if (config["unbrandInjector"]) {
            lstrcpy(nid.szTip, "UWP Dll Injector"); // name in debrand mode
        }
        else {
            lstrcpy(nid.szTip, "Fate Client Injector");
        }
        Shell_NotifyIcon(NIM_ADD, &nid);
        hmenu = CreatePopupMenu(); // Build menu
        AppendMenu(hmenu, MF_STRING, IDR_INJECT, "Inject"); // rightclick on Notification tray icon menu
        AppendMenu(hmenu, MF_STRING, IDR_OPEN, "Open");
        AppendMenu(hmenu, MF_STRING, IDR_QUIT, "Quit");
        break;
    }
    case WM_USER: {// news of the program when used continuously.
        switch (lParam) {
        case WM_LBUTTONDOWN: {
            break;
        }
        case WM_LBUTTONDBLCLK: {
            ShowWindow(hWnd, SW_SHOW);
            break;
        }
        case WM_RBUTTONDOWN: {
            GetCursorPos(&pt); // get the mouse coordinates
            ::SetForegroundWindow(hWnd); // settled out menu left-click menu does not disappear problem
            int iconSelection = TrackPopupMenu(hmenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);

            switch (iconSelection) {
            case IDR_QUIT: {
                PostQuitMessage(0);
                Shell_NotifyIcon(NIM_DELETE, &nid);
                break;
            }
            case IDR_OPEN: {
                ShowWindow(hWnd, SW_SHOW);
                break;
            }
            case IDR_INJECT: {
                clickInject();
                break;
            }
            }
            break;
        }
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_CLOSE:
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
    }
}

