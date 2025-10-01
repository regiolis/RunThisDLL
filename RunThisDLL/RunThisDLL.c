#include "Resource.h"
#include <stdio.h>
#include "Windows.h"
#include <shellapi.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

typedef void(WINAPI* RunDllProc)(HWND, HINSTANCE, LPWSTR, INT);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    int argc;
    wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    {
        if (argc < 2)
        {
            wchar_t buff[MAX_PATH];
            swprintf_s(buff, MAX_PATH, L"Usage: %ls ModuleFileName,ProcName [Arguments]", argv[0]);
            MessageBoxW(NULL, buff, L"Usage", MB_OK | MB_ICONINFORMATION);
            LocalFree(argv);
            return 0;
        }

        if (_wcsicmp(argv[1], L"About") == 0 || _wcsicmp(argv[1], L"/About") == 0)
        {
            // Afficher la boîte de dialogue About
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, AboutDlgProc);
            LocalFree(argv);
            return 0;
        }

        // Parse "DllName,FunctionName"
        wchar_t* comma = wcschr(argv[1], L',');
        if (comma == NULL)
        {
            MessageBoxW(NULL, L"Error: Invalid format. Expected ModuleName,ProcName", L"Invalid Format", MB_OK | MB_ICONERROR);
            LocalFree(argv);
            return 1;
        }

        *comma = L'\0';
        wchar_t* moduleName = argv[1];
        wchar_t* procName = comma + 1;
        wchar_t* cmdLine = (argc >= 3) ? argv[2] : L"";

        // Check if file exists
        wchar_t fullModulePath[MAX_PATH];
        DWORD result = SearchPathW(NULL, moduleName, NULL, MAX_PATH, fullModulePath, NULL);
        if (result < 1)
        {
            wchar_t buff[MAX_PATH];
            swprintf_s(buff, MAX_PATH, L"The module %ls could not be found", moduleName);
            MessageBoxW(NULL, buff, L"Error loading module", MB_OK | MB_ICONERROR);
            return 2;
        }

        wchar_t* moduleFileName = PathFindFileNameW(fullModulePath);

        // Load the DLL
        HMODULE hMod = LoadLibraryW(fullModulePath);
        if (hMod == NULL)
        {
            wchar_t buff[MAX_PATH];
            swprintf_s(buff, MAX_PATH, L"An error occurred while loading the module %ls", moduleFileName);
            MessageBoxW(NULL, buff, L"Error loading module", MB_OK | MB_ICONERROR);
            return STATUS_DLL_INIT_FAILED;
        }

        // Find the procedure (try Wide version first, then ANSI)
        char procNameA[256];
        RunDllProc proc = NULL;

        // Try with 'W' suffix first
        wchar_t procNameW[256];
        swprintf_s(procNameW, 256, L"%sW", procName);
        WideCharToMultiByte(CP_ACP, 0, procNameW, -1, procNameA, 256, NULL, NULL);
        proc = (RunDllProc)GetProcAddress(hMod, procNameA);

        // If not found, try without 'W'
        if (proc == NULL)
        {
            WideCharToMultiByte(CP_ACP, 0, procName, -1, procNameA, 256, NULL, NULL);
            proc = (RunDllProc)GetProcAddress(hMod, procNameA);
        }

        // Execute or show error
        if (proc != NULL)
        {
            proc(NULL, hMod, cmdLine, SW_SHOWNORMAL);
        }
        else
        {
            wchar_t buff[MAX_PATH];
            swprintf_s(buff, MAX_PATH, L"The procedure %ls could not be found in %ls", procName, moduleFileName);
            MessageBoxW(NULL, buff, L"Error finding procedure", MB_OK | MB_ICONERROR);
            FreeLibrary(hMod);
            return 3;
        }

        FreeLibrary(hMod);
        LocalFree(argv);
        return 0;
    }
}