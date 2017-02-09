#pragma once

#include <Windows.h>
#define MAX_LOADSTRING 100

/// @class WindowManager
/// @brief Window display and events manager
class WindowManager
{
private:
	static HINSTANCE m_hInstance;        ///< Application instance
	TCHAR m_title[MAX_LOADSTRING];       ///< title bar text container
	TCHAR m_windowClass[MAX_LOADSTRING]; ///< main window class name

public:
	/// @brief Create window container
	/// @param[in] hInstance      Application instance handle
	/// @param[in] hPrevInstance  Previous instance handle
	/// @param[in] lpCmdLine      Command line arguments
	WindowManager(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine);

	/// @brief Register the window class
	ATOM windowRegisterClass();

	/// @brief Show modal window
	/// @param[in] nCmdShow  Window display flag
	/// @returns Success (after closing window) or failure
	int showDialog(int nCmdShow);

	/// @brief Main window event handler
	/// @param[in] hWindow  Window handle
	/// @param[in] msg      Event code
	/// @param[in] wParam   Param value
	/// @param[in] lParam   Param value
	static LRESULT CALLBACK eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
	/// @brief About dialog event handler
	/// @param[in] hDialog  Dialog handle
	/// @param[in] msg      Event code
	/// @param[in] wParam   Param value
	/// @param[in] lParam   Param value
	static INT_PTR CALLBACK aboutDialogHandler(HWND hDialog, UINT msg, WPARAM wParam, LPARAM lParam);
};
