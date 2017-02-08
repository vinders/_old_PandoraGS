#pragma once

#include "res/resource.h"

// Window management functions
int createMenuWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow); // creation + message loop
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM); // event handler
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);   // utility 'about' dialog
