// windowManager.cpp : window creation and management (generated).
//

#include <Windows.h>
#include <string>
#include "res/resource.h"
#include "window_manager.h"
#include "tool_main.h"

HINSTANCE WindowManager::m_hInstance = 0; ///< Application instance


/// @brief Create window container
/// @param[in] hInstance      Application instance handle
/// @param[in] hPrevInstance  Previous instance handle
/// @param[in] lpCmdLine      Command line arguments
WindowManager::WindowManager(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	m_hInstance = hInstance;

	// initialize strings
	LoadString(hInstance, IDS_APP_TITLE, m_title, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTTOOL, m_windowClass, MAX_LOADSTRING);
	windowRegisterClass();
}

/// @brief Register the window class
ATOM WindowManager::windowRegisterClass()
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = eventHandler;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TESTTOOL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TESTTOOL);
	wcex.lpszClassName = m_windowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassEx(&wcex);
}

/// @brief Show modal window
/// @param[in] nCmdShow  Window display flag
/// @returns Success (after closing window) or failure
int WindowManager::showDialog(int nCmdShow)
{
	// create window
	HWND hWindow = CreateWindow(m_windowClass, m_title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 300, 260, 60, NULL, NULL, m_hInstance, NULL);
	if (!hWindow)
		return -1;
	ShowWindow(hWindow, nCmdShow);
	UpdateWindow(hWindow);
	HACCEL hAccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_TESTTOOL));

	// main event loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
	return 0;
}

/// @brief Main window event handler
/// @param[in] hWindow  Window handle
/// @param[in] msg      Event code
/// @param[in] wParam   Param value
/// @param[in] lParam   Param value
LRESULT CALLBACK WindowManager::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
		case WM_COMMAND: // WM_COMMAND - process application menu events
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch (wmId)
			{
				case IDM_ABOUT:
					DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWindow, aboutDialogHandler);
					break;
				case IDM_START:
					startDemoSequence(hWindow);
					break;
				case IDM_UNIT:
					startUnitTesting(hWindow);
					break;
				case IDM_PRIM:
					startPrimitiveTesting(hWindow);
					break;
				case IDM_CONFIGDIAL:
					openDialog(plugin_dialog_t::config);
					break;
				case IDM_ABOUTDIAL:
					openDialog(plugin_dialog_t::about);
					break;
				case IDM_EXIT:
					DestroyWindow(hWindow);
					break;
				default:
					return DefWindowProc(hWindow, msg, wParam, lParam);
			}
			break;

		case WM_PAINT: // WM_PAINT - draw window
			hdc = BeginPaint(hWindow, &ps);
			EndPaint(hWindow, &ps);
			break;
		case WM_DESTROY: // WM_DESTROY - destroy window
			PostQuitMessage(0);
			break;

		default: // other event - default window handler
			return DefWindowProc(hWindow, msg, wParam, lParam);
	}
	return 0;
}

/// @brief About dialog event handler
/// @param[in] hDialog  Dialog handle
/// @param[in] msg      Event code
/// @param[in] wParam   Param value
/// @param[in] lParam   Param value
INT_PTR CALLBACK WindowManager::aboutDialogHandler(HWND hDialog, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (msg)
	{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) // close dialog
			{
				EndDialog(hDialog, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}
