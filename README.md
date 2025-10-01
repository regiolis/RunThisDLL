# RunThisDLL

A custom implementation of Windows `rundll32.exe` - a utility to execute functions from DLL files.

## Overview

RunThisDLL is a lightweight Windows application that allows you to call exported functions from Dynamic Link Libraries (DLLs), similar to the native `rundll32.exe`. It supports both Unicode (Wide) and ANSI function variants.

## Features

- ✅ Execute DLL functions with the standard `rundll32` signature
- ✅ Automatic detection of Unicode (`FunctionW`) and ANSI (`FunctionA`) variants
- ✅ Smart DLL path resolution (searches in system directories and PATH)
- ✅ Support for both short names and full paths
- ✅ Built-in "About" dialog
- ✅ User-friendly error messages via GUI dialog boxes

## Usage

### Basic Syntax

```cmd
RunThisDLL.exe ModuleFileName,ProcName [Arguments]
```

### Examples

```cmd
# Using a system DLL
RunThisDLL.exe shell32.dll,Control_RunDLL desk.cpl

# Using full path
RunThisDLL.exe C:\Windows\System32\shell32.dll,Control_RunDLL

# With arguments
RunThisDLL.exe MyCustom.dll,MyFunction "Hello World"

# Display About dialog
RunThisDLL.exe About
```

## DLL Function Signature

For a function to be callable by RunThisDLL, it must follow this signature:

```cpp
extern "C" __declspec(dllexport) void WINAPI YourFunctionW(
    HWND hWnd,
    HINSTANCE hInstDll,
    LPWSTR lpszCmdLine,
    int nCmdShow
)
{
    // Your code here
    MessageBoxW(NULL, lpszCmdLine, L"RunThisDLL", MB_OK);
}
```

**Important notes:**
- The function name can be anything, but the signature must match exactly
- Use `LPWSTR` for Unicode (Wide) version or `LPSTR` for ANSI version
- Export the function with `__declspec(dllexport)` or use a `.def` file
- The tool automatically tries the `W` suffix first, then without

## DLL Search Order

RunThisDLL uses Windows' `SearchPathW()` API to locate DLLs in the following order:

1. The application directory
2. The current working directory
3. The Windows System32 directory
4. The Windows directory
5. Directories in the PATH environment variable

## Building

### Requirements
- Visual Studio 2019 or later
- Windows SDK

### Build Instructions

1. Clone the repository
```bash
git clone https://github.com/yourusername/RunThisDLL.git
cd RunThisDLL
```

2. Open `RunThisDLL.sln` in Visual Studio

3. Build the solution (Ctrl+Shift+B)

The compiled executable will be in the `Release` or `Debug` folder.

## Technical Details

- **Language**: C (Win32 API)
- **Subsystem**: Windows GUI
- **Entry Point**: `wWinMain`
- **Character Set**: Unicode (Wide)

## License

This project is provided as-is for educational purposes.

## Author

© Régiolis 2025

## Acknowledgments

Inspired by the native Windows `rundll32.exe` utility.