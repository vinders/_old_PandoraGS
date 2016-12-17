#pragma once

#include "resource.h"

// Window management functions
int CreateBaseWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow); // creation + message loop
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM); // event handler
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);   // utility 'about' dialog
