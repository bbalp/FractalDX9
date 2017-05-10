#include "stdafx.h"
#include "MainForm.h"

#include "PixelShader.h"
PixelShader UPS;

#define MAX_LOADSTRING 100

HINSTANCE hInst;								// Current instance
TCHAR szTitle[MAX_LOADSTRING];					// Title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// Main window class name

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void RunRendering(HWND hWnd);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialization
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DX9PIXELSHADERTESTPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX9PIXELSHADERTESTPROJECT));

	// Message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX9PIXELSHADERTESTPROJECT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DX9PIXELSHADERTESTPROJECT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Stocke le handle d'instance dans la variable globale

	RECT desktop;

	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();

	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);

	UPS.L = 1600;
	UPS.H = 1000;

	UPS.div = 1;
	UPS.divL = 0;
	UPS.divH = 0;

	UPS.fileFormatSelector = 0; // BMP

	hWnd = CreateWindow(szWindowClass, L"FractalDX9", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,// | WS_HSCROLL | WS_VSCROLL,
	                    10, 10, desktop.bottom - 50, int(float(desktop.bottom - 50) / (float(UPS.L) / float(UPS.H))), NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	MENUITEMINFO menuInfo;
	menuInfo.cbSize		= sizeof(MENUITEMINFO);
	menuInfo.fMask		= MIIM_STATE;
	menuInfo.fState		= MFS_CHECKED;

	switch (message)
	{
	case WM_CREATE:
		SetMenuItemInfo(GetMenu(hWnd), ID_1600_1000, FALSE, &menuInfo);
		SetMenuItemInfo(GetMenu(hWnd), ID_CONFIGURATION_IMAGE, FALSE, &menuInfo);
		SetMenuItemInfo(GetMenu(hWnd), ID_FORMATDEFICHIER_BMP, FALSE, &menuInfo);
		break;

	case WM_COMMAND:

		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Menu selection
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		case ID_RENDUBITMAP:
			RunRendering(hWnd);
			//MessageBox(hWnd, L"Done!", L"FractalDX9", NULL);
			break;

		case ID_FORMATDEFICHIER_BMP:
			menuInfo.fState		= MFS_UNCHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_FORMATDEFICHIER_JPEG, FALSE, &menuInfo);
			menuInfo.fState		= MFS_CHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_FORMATDEFICHIER_BMP, FALSE, &menuInfo);
			UPS.fileFormatSelector = 0; // BMP
			break;

		case ID_FORMATDEFICHIER_JPEG:
			menuInfo.fState		= MFS_UNCHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_FORMATDEFICHIER_BMP, FALSE, &menuInfo);
			menuInfo.fState		= MFS_CHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_FORMATDEFICHIER_JPEG, FALSE, &menuInfo);
			UPS.fileFormatSelector = 1; // JPEG
			break;

		case ID_CONFIGURATION_IMAGE:
			menuInfo.fState		= MFS_UNCHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_3, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_5, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_10, FALSE, &menuInfo);
			menuInfo.fState		= MFS_CHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_CONFIGURATION_IMAGE, FALSE, &menuInfo);
			UPS.div = 1;
			break;

		case ID_COMPOSITION_3:
			menuInfo.fState		= MFS_UNCHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_CONFIGURATION_IMAGE, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_5, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_10, FALSE, &menuInfo);
			menuInfo.fState		= MFS_CHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_3, FALSE, &menuInfo);
			UPS.div = 3;
			break;

		case ID_COMPOSITION_5:
			menuInfo.fState		= MFS_UNCHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_CONFIGURATION_IMAGE, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_3, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_10, FALSE, &menuInfo);
			menuInfo.fState		= MFS_CHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_5, FALSE, &menuInfo);
			UPS.div = 5;
			break;

		case ID_COMPOSITION_10:
			menuInfo.fState		= MFS_UNCHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_CONFIGURATION_IMAGE, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_5, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_3, FALSE, &menuInfo);
			menuInfo.fState		= MFS_CHECKED;
			SetMenuItemInfo(GetMenu(hWnd), ID_COMPOSITION_10, FALSE, &menuInfo);
			UPS.div = 10;
			break;

		default:
			RECT rEct;
			GetClientRect(hWnd, &rEct);

			menuInfo.cbSize		= sizeof(MENUITEMINFO);
			menuInfo.fMask		= MIIM_STATE;
			menuInfo.fState		= MFS_UNCHECKED;

			SetMenuItemInfo(GetMenu(hWnd), ID_500_500, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_1000_1000, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_1600_1000, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_3200_2000, FALSE, &menuInfo);
			SetMenuItemInfo(GetMenu(hWnd), ID_4096_4096, FALSE, &menuInfo);

			menuInfo.cbSize		= sizeof(MENUITEMINFO);
			menuInfo.fMask		= MIIM_STATE;
			menuInfo.fState		= MFS_CHECKED;

			MENUITEMINFO mi_state;
			mi_state.cbSize		= sizeof(MENUITEMINFO);
			mi_state.fMask		= MIIM_STRING;

			switch (wmId)
			{
			case ID_500_500:
				SetMenuItemInfo(GetMenu(hWnd), ID_500_500, FALSE, &menuInfo);
				mi_state.dwTypeData		= L"Resolution : 500 * 500";
				SetMenuItemInfo(GetMenu(hWnd), ID_STATUT, FALSE, &mi_state);
				UPS.L = 500;
				UPS.H = 500;
				SetWindowPos(hWnd, HWND_TOP, 0, 0, rEct.right, rEct.right, SWP_NOMOVE);
				break;

			case ID_1000_1000:
				SetMenuItemInfo(GetMenu(hWnd), ID_1000_1000, FALSE, &menuInfo);
				mi_state.dwTypeData		= L"Resolution : 1000 * 1000";
				SetMenuItemInfo(GetMenu(hWnd), ID_STATUT, FALSE, &mi_state);
				UPS.L = 1000;
				UPS.H = 1000;
				SetWindowPos(hWnd, HWND_TOP, 0, 0, rEct.right, rEct.right, SWP_NOMOVE);
				break;

			case ID_1600_1000:
				SetMenuItemInfo(GetMenu(hWnd), ID_1600_1000, FALSE, &menuInfo);
				mi_state.dwTypeData		= L"Resolution : 1600 * 1000";
				SetMenuItemInfo(GetMenu(hWnd), ID_STATUT, FALSE, &mi_state);
				UPS.L = 1600;
				UPS.H = 1000;
				SetWindowPos(hWnd, HWND_TOP, 0, 0, rEct.right, int(float(rEct.right) / 1.6f), SWP_NOMOVE);
				break;

			case ID_3200_2000:
				SetMenuItemInfo(GetMenu(hWnd), ID_3200_2000, FALSE, &menuInfo);
				mi_state.dwTypeData		= L"Resolution : 3200 * 2000";
				SetMenuItemInfo(GetMenu(hWnd), ID_STATUT, FALSE, &mi_state);
				UPS.L = 3200;
				UPS.H = 2000;
				SetWindowPos(hWnd, HWND_TOP, 0, 0, rEct.right, int(float(rEct.right) / 1.6f), SWP_NOMOVE);
				break;

			case ID_4096_4096:
				SetMenuItemInfo(GetMenu(hWnd), ID_4096_4096, FALSE, &menuInfo);
				mi_state.dwTypeData		= L"Resolution : 4096 * 4096";
				SetMenuItemInfo(GetMenu(hWnd), ID_STATUT, FALSE, &mi_state);
				UPS.L = 4096;
				UPS.H = 4096;
				SetWindowPos(hWnd, HWND_TOP, 0, 0, rEct.right, rEct.right, SWP_NOMOVE);
				break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			break;
		}

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		UPS.Cleanup();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

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

void RunRendering(HWND hWnd)
{
	for (int i = 0; i < UPS.div; i++)
	{
		for (int j = 0; j < UPS.div; j++)
		{
			UPS.divL = i;
			UPS.divH = j;

			UPS.Cleanup();
			UPS.Run(hWnd);
		}
	}
}