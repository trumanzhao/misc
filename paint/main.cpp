#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <vector>
#include <list>
#include "winbox.h"
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib") 

static LRESULT CALLBACK win_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE pre_instance, char* cmd_line, int cmd_show)
{
	ULONG_PTR gdiplus_token;
	GdiplusStartupInput gdiplus_startup_input;
	GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, nullptr);

	g_app = new winbox();

	WNDCLASSEX  wcex;
	char window_class[] = "paint";

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = window_class;
	wcex.hIconSm = nullptr;

	RegisterClassEx(&wcex);

	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW | WS_EX_TOPMOST, window_class, "paint", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, instance, nullptr);
	if (hwnd == nullptr)
		return 0;

	ShowWindow(hwnd, cmd_show);
	UpdateWindow(hwnd);
	SetTimer(hwnd, 1, 60, nullptr);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete g_app;
	GdiplusShutdown(gdiplus_token);
	return 0;
}

LRESULT CALLBACK win_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_PAINT:
		g_app->on_paint();
		break;

	case WM_TIMER:
		g_app->on_timer();
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_CREATE:
		g_app->on_create(hwnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		g_app->on_size(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_KEYDOWN:
		g_app->on_key_down(wparam);
		break;

	case WM_RBUTTONUP:
		g_app->on_rbutton_up(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_LBUTTONUP:
		g_app->on_lbutton_up(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_MOUSEMOVE:
		g_app->on_mouse_move(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}

	return 0;
}
