// Lab1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lab1.h"
#include "MemoryBitmap.h"
#include "Enums.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
MemoryBitmap* mainBmp;							//the main (result) image
MemoryBitmap* tempBmp;							//the temp image, while drawing don't interrupt





int mouseDownX;									//coordinates where mouse was down
int mouseDownY;

PaintTools selectedPaintTool = PTl_Line;

bool isDrawingModeEnabled = false;



// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				CreateMemoryBitmap();


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	mainBmp->~MemoryBitmap();
	tempBmp->~MemoryBitmap();
	return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LAB1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}



//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   
   if (!hWnd)
   {
      return FALSE;
   }


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC tempDC=0;
	static HDC mainBitmapDC=0;
	static HBRUSH hBrush;
	static HPEN hPen;
	RECT rect;
	static HBITMAP tempBitmap,mainBitmap;


	int wmId, wmEvent;
	PAINTSTRUCT ps;
	 HDC hdc=GetDC(hWnd);

	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hWnd,&rect);

		
		hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
		SelectObject(hdc,hBrush);
		mainBmp = new MemoryBitmap(rect);
		tempBmp = new MemoryBitmap(rect);

		/*hPen=(HPEN)GetStockObject(BLACK_PEN);
				
		tempDC=CreateCompatibleDC(NULL);
		tempBitmap=CreateCompatibleBitmap(tempDC,rect.right,rect.bottom);

		mainBitmapDC=CreateCompatibleDC(NULL);
		mainBitmap=CreateCompatibleBitmap(mainBitmapDC,rect.right,rect.bottom);

		DeleteObject(SelectObject(tempDC,tempBitmap));
		DeleteObject(SelectObject(tempDC, (HBRUSH) WHITE_BRUSH)); 
		PatBlt(tempDC, 0, 0, rect.right, rect.bottom, PATCOPY);


		DeleteObject(SelectObject(mainBitmapDC,mainBitmap));
		DeleteObject(SelectObject(mainBitmapDC, (HBRUSH) WHITE_BRUSH)); 
		PatBlt(mainBitmapDC, 0, 0, rect.right, rect.bottom, PATCOPY);

		DeleteObject(SelectObject(tempDC,hPen));
		DeleteObject(SelectObject(tempDC,hBrush));
		DeleteObject(SelectObject(mainBitmapDC,hPen));
		DeleteObject(SelectObject(mainBitmapDC,hBrush));*/

		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_TOOL_PENCIL:
			selectedPaintTool = PTl_Pencil;
			break;
		case IDM_TOOL_LINE:
			selectedPaintTool = PTl_Line;
			InvalidateRect(hWnd,NULL,FALSE);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd,&rect);
		if(isDrawingModeEnabled)
		{
			tempBmp->DrawToHDC(hdc, rect);
		    //BitBlt(hdc, 0, 0,rect.right,rect.bottom,tempDC,0, 0, SRCCOPY);
			isDrawingModeEnabled = false;
		}
		else
		{
			mainBmp->DrawToHDC(hdc, rect);
			//BitBlt(hdc, 0, 0,rect.right,rect.bottom,mainBitmapDC,0, 0, SRCCOPY);

		}
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		mouseDownX = GET_X_LPARAM(lParam); 
		mouseDownY = GET_Y_LPARAM(lParam); 
		//MoveToEx(tempDC,mouseDownX,mouseDownY,NULL);
		//MoveToEx(mainBitmapDC,mouseDownX,mouseDownY,NULL);
		mainBmp->MoveTo(mouseDownX, mouseDownY);
		tempBmp->MoveTo(mouseDownX, mouseDownY);
		break;
	case WM_LBUTTONUP:
		GetClientRect(hWnd,&rect);
		/*tempBitmap=CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
		DeleteObject(SelectObject(tempDC,tempBitmap));
		BitBlt(tempDC,0,0,rect.right,rect.bottom,mainBitmapDC,0,0,SRCCOPY);*/
		tempBmp->Clear(rect);
		BitBlt(tempBmp->GetDC(),0,0,rect.right,rect.bottom,mainBmp->GetDC(),0,0,SRCCOPY);
		
		//mainBmp->DrawToHDC(tempBmp->GetDC(),rect);


		switch (selectedPaintTool)
		{
		case PTl_Line:
			mainBmp->MoveTo(mouseDownX,mouseDownY);
			mainBmp->BmpLineTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			/*MoveToEx(mainBitmapDC,mouseDownX,mouseDownY,NULL);
			LineTo(mainBitmapDC,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));*/
			break;
		}
		isDrawingModeEnabled = false;
		InvalidateRect(hWnd,NULL,FALSE);
		UpdateWindow(hWnd);

		break;
	case WM_MOUSEMOVE:
		if(wParam == MK_LBUTTON)
		{
			GetClientRect(hWnd,&rect);
			tempBmp->Clear(rect);
			BitBlt(tempBmp->GetDC(),0,0,rect.right,rect.bottom,mainBmp->GetDC(),0,0,SRCCOPY);

			//mainBmp->DrawToHDC(tempBmp->GetDC(),rect);
				/*		tempBitmap=CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
			DeleteObject(SelectObject(tempDC,tempBitmap));
			BitBlt(tempDC,0,0,rect.right,rect.bottom,mainBitmapDC,0,0,SRCCOPY);
			*/
			switch (selectedPaintTool)
			{
			case PTl_Pencil:
				//LineTo(mainBitmapDC,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				mainBmp->BmpLineTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			case PTl_Line:
				tempBmp->MoveTo(mouseDownX, mouseDownY);
				tempBmp->BmpLineTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				
				/*MoveToEx(tempDC,mouseDownX,mouseDownY,NULL);
			    LineTo(tempDC,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));*/

				break;
			case PTl_Rectlange:
				break;
			default:
				break;
			}
			isDrawingModeEnabled = true;
			InvalidateRect(hWnd, NULL, false);
			UpdateWindow(hWnd);
		}
		break;
	case WM_ERASEBKGND:
		GetClientRect(hWnd,&rect);
		FillRect(hdc,&rect,WHITE_BRUSH);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
