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

HDC tempDC=0;
HDC mainBitmapDC=0;
HBRUSH hBrush;
HPEN hPen;
RECT rect;
HBITMAP tempBitmap,mainBitmap;




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
	return static_cast<int>(msg.wParam);
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


void SaveFile(HWND hWnd)
{
	OPENFILENAME openfilename;
	wchar_t fullpath[256],filename[256];
	fullpath[0] = '\0';
	filename[0] = '\0';

	openfilename.lStructSize=sizeof(OPENFILENAME);
	openfilename.hwndOwner=hWnd;
	openfilename.lpstrDefExt = L"emf";
	openfilename.hInstance=hInst;
	openfilename.lpstrFilter=L"Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
	openfilename.nFilterIndex=1;
	openfilename.lpstrCustomFilter = NULL;
	openfilename.lpstrFile=fullpath;
	openfilename.nMaxFile=sizeof(fullpath)/sizeof(*fullpath);
	openfilename.lpstrFileTitle=filename;
	openfilename.nMaxFileTitle=sizeof(filename);
	openfilename.lpstrInitialDir=NULL;
	openfilename.lpstrTitle=L"Open file...";
	openfilename.Flags=OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_EXPLORER;
	if(GetSaveFileName(&openfilename))
	{
		// Determine the picture frame dimensions.  
		// iWidthMM is the display width in millimeters.  
		// iHeightMM is the display height in millimeters.  
		// iWidthPels is the display width in pixels.  
		// iHeightPels is the display height in pixels  
		HDC hdcRef = GetDC(hWnd); 
		int iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE); 
		int iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE); 
		int iWidthPels = GetDeviceCaps(hdcRef, HORZRES); 
		int iHeightPels = GetDeviceCaps(hdcRef, VERTRES); 

		RECT rect;
		GetClientRect(hWnd,&rect);

		rect.left = (rect.left * iWidthMM * 100)/iWidthPels; 
		rect.top = (rect.top * iHeightMM * 100)/iHeightPels; 
		rect.right = (rect.right * iWidthMM * 100)/iWidthPels; 
		rect.bottom = (rect.bottom * iHeightMM * 100)/iHeightPels; 

		HDC metaFileDC =  CreateEnhMetaFile(NULL,fullpath,&rect, NULL );
		GetClientRect(hWnd,&rect);
		PatBlt(metaFileDC, 0,0, rect.right, rect.bottom,PATCOPY);
		mainBmp->DrawToHDC(metaFileDC,rect );
	  	CloseEnhMetaFile(metaFileDC);
		ReleaseDC(hWnd, hdcRef);
	}
}

void NewFile(HWND hWnd)
{
	HDC hdc=GetDC(hWnd);
	isDrawingModeEnabled = false;
	GetClientRect(hWnd,&rect);
	hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
	SelectObject(hdc,hBrush);
	mainBmp = new MemoryBitmap(rect);
	tempBmp = new MemoryBitmap(rect);
	InvalidateRect(hWnd,NULL,FALSE);
	UpdateWindow(hWnd);
}


void PrintFile(HWND hWnd)
{
	PRINTDLG printDlg;
	ZeroMemory(&printDlg, sizeof(printDlg));
	printDlg.lStructSize = sizeof(printDlg);
	printDlg.hwndOwner   = hWnd;
	printDlg.hDevMode    = NULL; 
	printDlg.hDevNames   = NULL; 
	printDlg.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	printDlg.nCopies     = 1;
	printDlg.nFromPage   = 0xFFFF;
	printDlg.nToPage     = 0xFFFF;
	printDlg.nMinPage    = 1;
	printDlg.nMaxPage    = 0xFFFF;
	
	if (PrintDlg(&printDlg))
	{
		/*int Rx = GetDeviceCaps(printDlg.hDC, LOGPIXELSX);
		int Ry = GetDeviceCaps(printDlg.hDC, LOGPIXELSY);
		int Rx1 = GetDeviceCaps(hdcMem, LOGPIXELSX);
		int Ry1 = GetDeviceCaps(hdcMem, LOGPIXELSY);*/
		DOCINFO docInfo;
		docInfo.cbSize = sizeof(docInfo);
		docInfo.lpszDocName=L"Print Picture";
		docInfo.fwType=NULL;
		docInfo.lpszDatatype=NULL;
		docInfo.lpszOutput=NULL;
	
		StartDoc(printDlg.hDC, &docInfo);
		StartPage(printDlg.hDC);

		GetClientRect(hWnd,&rect);
		PatBlt(printDlg.hDC, 0,0, rect.right, rect.bottom,PATCOPY);
		mainBmp->DrawToHDC(printDlg.hDC,rect );

		EndPage(printDlg.hDC);
		EndDoc(printDlg.hDC);
		DeleteDC(printDlg.hDC);
	}


}

void OpenImageFile(HWND hWnd)
{
	OPENFILENAME openfilename;
	wchar_t fullpath[256],filename[256];
	fullpath[0] = '\0';
	filename[0] = '\0';

	openfilename.lStructSize=sizeof(OPENFILENAME);
	openfilename.hwndOwner=hWnd;
	openfilename.lpstrDefExt = L"emf";
	openfilename.hInstance=hInst;
	openfilename.lpstrFilter=L"Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
	openfilename.nFilterIndex=1;
	openfilename.lpstrCustomFilter = NULL;
	openfilename.lpstrFile=fullpath;
	openfilename.nMaxFile=sizeof(fullpath)/sizeof(*fullpath);
	openfilename.lpstrFileTitle=filename;
	openfilename.nMaxFileTitle=sizeof(filename);
	openfilename.lpstrInitialDir=NULL;
	openfilename.lpstrTitle=L"Save file as...";
	openfilename.Flags=OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_EXPLORER;

	// Display the Open dialog box. 

	if (GetOpenFileName(&openfilename))
	{
		NewFile(hWnd);
		RECT rect;
		GetClientRect(hWnd,&rect);
		HENHMETAFILE metafile =  GetEnhMetaFile(fullpath);
		PlayEnhMetaFile(mainBmp->GetDC(), metafile,&rect );
		InvalidateRect(hWnd,NULL,FALSE);
		UpdateWindow(hWnd);
	}
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
		case ID_FILE_SAVE:
			SaveFile(hWnd);			
			break;
		case ID_FILE_NEW:
			NewFile(hWnd);
			break;
		case ID_FILE_OPEN:
			OpenImageFile(hWnd);
			break;
		case ID_FILE_PRINT:
			PrintFile(hWnd);
			break;
		case ID_WIDTH_1:
			mainBmp->SetPenWidth(1);
			tempBmp->SetPenWidth(1);
			break;
		case ID_WIDTH_2:
			mainBmp->SetPenWidth(2);
			tempBmp->SetPenWidth(2);
			break;
		case ID_WIDTH_3:
			mainBmp->SetPenWidth(3);
			tempBmp->SetPenWidth(3);
			break;		
		case ID_WIDTH_4:
			mainBmp->SetPenWidth(4);
			tempBmp->SetPenWidth(4);
			break;
		case ID_WIDTH_5:
			mainBmp->SetPenWidth(5);
			tempBmp->SetPenWidth(5);
			break;
		case ID_WIDTH_6:
			mainBmp->SetPenWidth(6);
			tempBmp->SetPenWidth(6);
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
		    isDrawingModeEnabled = false;
		}
		else
		{
			mainBmp->DrawToHDC(hdc, rect);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		mouseDownX = GET_X_LPARAM(lParam); 
		mouseDownY = GET_Y_LPARAM(lParam); 
		mainBmp->MoveTo(mouseDownX, mouseDownY);
		tempBmp->MoveTo(mouseDownX, mouseDownY);
		break;
	case WM_LBUTTONUP:
		GetClientRect(hWnd,&rect);
		tempBmp->Clear(rect);
		BitBlt(tempBmp->GetDC(),0,0,rect.right,rect.bottom,mainBmp->GetDC(),0,0,SRCCOPY);
		switch (selectedPaintTool)
		{
		case PTl_Line:
			mainBmp->MoveTo(mouseDownX,mouseDownY);
			mainBmp->BmpLineTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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

			switch (selectedPaintTool)
			{
			case PTl_Pencil:
				mainBmp->BmpLineTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			case PTl_Line:
				tempBmp->MoveTo(mouseDownX, mouseDownY);
				tempBmp->BmpLineTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
