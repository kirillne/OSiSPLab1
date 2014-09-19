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

double scale = 1;
int offsetX = 1000;
int offsetY = 1000;


int mouseDownX;									//coordinates where mouse was down
int mouseDownY;

int poligoneStartMouseDownX;									
int poligoneStartMouseDownY;


PaintTools selectedPaintTool = PTl_Line;

bool isDrawingModeEnabled = false;
bool isPolylineModeEnabled = false;
bool isTextModeEnabled = false;
bool isSaved = true;

std::wstring inputedText = L"";



// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				CreateMemoryBitmap();
void				DrawMainBMPToTempBMP(HWND hWnd);
INT_PTR CALLBACK    CloseDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);




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

	return static_cast<int>(msg.wParam);
}

RECT GetMainBMPRect()
{
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = 3000;
	rect.bottom = 3000;
	return rect;
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

		RECT rect = GetMainBMPRect();
		//GetClientRect(hWnd,&rect);

		rect.left = (rect.left * iWidthMM * 100)/iWidthPels; 
		rect.top = (rect.top * iHeightMM * 100)/iHeightPels; 
		rect.right = (rect.right * iWidthMM * 100)/iWidthPels; 
		rect.bottom = (rect.bottom * iHeightMM * 100)/iHeightPels; 

		HDC metaFileDC =  CreateEnhMetaFile(NULL,fullpath,&rect, NULL );
		//GetClientRect(hWnd,&rect);
		PatBlt(metaFileDC, 0,0, rect.right, rect.bottom,PATCOPY);
		mainBmp->DrawToHDC(metaFileDC,GetMainBMPRect(),GetMainBMPRect() );
	  	CloseEnhMetaFile(metaFileDC);
		ReleaseDC(hWnd, hdcRef);
		isSaved = true;
	}
}

void NewFile(HWND hWnd)
{
	HDC hdc=GetDC(hWnd);
	isDrawingModeEnabled = false;
	GetClientRect(hWnd,&rect);
	hBrush=(HBRUSH)GetStockObject(NULL_BRUSH);
	SelectObject(hdc,hBrush);
	if(tempBmp != NULL)
	{
		delete tempBmp;
		delete mainBmp;
	}
	tempBmp = new MemoryBitmap(rect,hdc);
	mainBmp = new MemoryBitmap(GetMainBMPRect(), hdc);
	offsetX = 1000;
	offsetY = 1000;
	scale = 1;
	Rectangle(mainBmp->GetDC(),1,1,GetMainBMPRect().right-2, GetMainBMPRect().bottom-2);
	InvalidateRect(hWnd,NULL,FALSE);
	UpdateWindow(hWnd);
	isSaved = true;
}

COLORREF GetColorFromUser(HWND hWnd)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 
	HBRUSH hbrush;                  // brush handle

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.Flags = CC_FULLOPEN ;
 
	if (ChooseColor(&cc)) 
	{
		return cc.rgbResult; 
	}
	return 0xFFFFFFFF; // It's unavalible value
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
		int Rx = GetDeviceCaps(printDlg.hDC, LOGPIXELSX);
		int Ry = GetDeviceCaps(printDlg.hDC, LOGPIXELSY);
		/*int Rx1 = GetDeviceCaps(hdcMem, LOGPIXELSX);
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
		rect.right*=Rx/100;
		rect.bottom*=Ry/100;
		PatBlt(printDlg.hDC, 0,0, rect.right, rect.bottom,PATCOPY);
		mainBmp->DrawToHDC(printDlg.hDC,GetMainBMPRect(),rect );

		EndPage(printDlg.hDC);
		EndDoc(printDlg.hDC);
		DeleteDC(printDlg.hDC);
		isSaved = true;
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
		RECT rect = GetMainBMPRect();
		
		HENHMETAFILE metafile =  GetEnhMetaFile(fullpath);
		PlayEnhMetaFile(mainBmp->GetDC(), metafile,&rect );
		InvalidateRect(hWnd,NULL,FALSE);
		UpdateWindow(hWnd);
	}
}

void SetPenColor(HWND hWnd)
{
	COLORREF color = GetColorFromUser(hWnd);
	if(color != 0xFFFFFFFF)
	{
		mainBmp->SetPenColor(color);
		tempBmp->SetPenColor(color);
	}
}

void SetBrushColor(HWND hWnd)
{
	COLORREF color = GetColorFromUser(hWnd);
	if(color != 0xFFFFFFFF)
	{
		mainBmp->SetBrushColor(color);
		tempBmp->SetBrushColor(color);
	}
}

void DrawLineOnMainBMP(int fromX, int fromY, int toX, int toY)
{
	mainBmp->MoveTo(fromX*scale + offsetX, fromY*scale + offsetY);
	mainBmp->BmpLineTo(toX*scale + offsetX, toY*scale + offsetY);
}

int OnClose(HWND hWnd)
{
	if(!isSaved)
	{
		switch (DialogBox(hInst, MAKEINTRESOURCE(IDD_CLOSE), hWnd, CloseDialog))
		{
		case SR_Save:
			SaveFile(hWnd);
			if(isSaved)
			{
				DestroyWindow(hWnd);
				return 0;
			}
			return 1;
		case SR_Exit:
			DestroyWindow(hWnd);
			return 0;
		case SR_Cancel:
			return 1;
		}
	}
	DestroyWindow(hWnd);
	return 0;
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
	RECT mainRect;
		int wmId, wmEvent;
	PAINTSTRUCT ps;
	 HDC hdc=GetDC(hWnd);

	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hWnd,&rect);
		NewFile(hWnd);
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
			OnClose(hWnd);
			break;
		case IDM_TOOL_PENCIL:
			selectedPaintTool = PTl_Pencil;
			break;
		case IDM_TOOL_LINE:
			selectedPaintTool = PTl_Line;
			break;
		case ID_TOOL_RECTLANGE:
			selectedPaintTool = PTl_Rectlange;
			break;
		case ID_TOOL_ELLIPSE:
			selectedPaintTool = PTl_Ellipse;
			break;
		case ID_TOOL_POLYLINE:
			selectedPaintTool = PTl_Polyline;
			break;
		case ID_TOOL_POLYGON:
			selectedPaintTool = PTl_Polygone;
			break;
		case ID_TOOL_TEXT:
			selectedPaintTool = PTl_Text;
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
		case ID_PEN_COLOR:
			SetPenColor(hWnd);
			break;
		case ID_BRUSH_COLOR:
			SetBrushColor(hWnd);
			break;
		case ID_BRUSH_NONE:
			mainBmp->SetEmptyBrush();
			tempBmp->SetEmptyBrush();
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
			tempBmp->DrawToHDC(hdc, rect,rect);
		    isDrawingModeEnabled = false;
		}
		else
		{
			//Draw MainBpm to form
			RECT mainRect = rect;
			mainRect.left = offsetX;
			mainRect.top = offsetY;
			mainRect.right*= scale;
			mainRect.bottom*= scale;
			mainBmp->DrawToHDC(hdc,mainRect, rect);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		if(!isPolylineModeEnabled)
		{
			//Save click position
			mouseDownX = GET_X_LPARAM(lParam); 
			mouseDownY = GET_Y_LPARAM(lParam); 
			mainBmp->MoveTo(mouseDownX*scale + offsetX, mouseDownY*scale + offsetY);
			tempBmp->MoveTo(mouseDownX, mouseDownY);
			if(selectedPaintTool == PTl_Polygone)
			{
				poligoneStartMouseDownX =  GET_X_LPARAM(lParam); 
				poligoneStartMouseDownY = GET_Y_LPARAM(lParam); 
			}
			else if (selectedPaintTool == PTl_Text)
			{
				isTextModeEnabled = true;
				inputedText.clear();
			}
		}
		else
		{
			isDrawingModeEnabled = true; //for Polilyne we continue drawing
		}
		break;
	case WM_LBUTTONUP:
		GetClientRect(hWnd,&rect);
		tempBmp->Clear(rect);
		mainRect = rect;
		mainRect.left = offsetX;
		mainRect.top = offsetY;
		mainRect.right*=scale;
		mainRect.bottom*=scale;
		mainBmp->DrawToHDC(tempBmp->GetDC(),mainRect,rect); //Draw main to temp
		isDrawingModeEnabled = false;
		switch (selectedPaintTool)
		{
		case PTl_Line:
			DrawLineOnMainBMP(mouseDownX, mouseDownY, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case PTl_Polygone:
		case PTl_Polyline:
			DrawLineOnMainBMP(mouseDownX, mouseDownY, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			mouseDownX = GET_X_LPARAM(lParam); 
			mouseDownY = GET_Y_LPARAM(lParam); 
			isPolylineModeEnabled = true;
			break;
		case PTl_Rectlange:
			Rectangle(mainBmp->GetDC(),mouseDownX*scale + offsetX,mouseDownY*scale + offsetY,GET_X_LPARAM(lParam)*scale + offsetX, GET_Y_LPARAM(lParam)*scale + offsetY);
			break;
		case PTl_Ellipse:
			Ellipse(mainBmp->GetDC(),mouseDownX*scale + offsetX,mouseDownY*scale + offsetY,GET_X_LPARAM(lParam)*scale + offsetX, GET_Y_LPARAM(lParam)*scale + offsetY);
			break;
		}
		InvalidateRect(hWnd,NULL,FALSE);
		UpdateWindow(hWnd);
		isSaved = false;
		break;
	case WM_RBUTTONUP:
		isDrawingModeEnabled = false;
		DrawLineOnMainBMP(mouseDownX, mouseDownY, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if(selectedPaintTool == PTl_Polygone && isPolylineModeEnabled)
		{
			DrawLineOnMainBMP( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), poligoneStartMouseDownX, poligoneStartMouseDownY);
		}
		isPolylineModeEnabled = false;
		InvalidateRect(hWnd,NULL,FALSE);
		UpdateWindow(hWnd);
		break;
	case WM_CHAR:
		if(isTextModeEnabled)
		{
			isDrawingModeEnabled = true;
			char c=(char)wParam;
			if(c == VK_RETURN)
			{
				isTextModeEnabled = false;
				isDrawingModeEnabled = false;
				TextOut(mainBmp->GetDC(),mouseDownX*scale + offsetX, mouseDownY*scale + offsetY, inputedText.c_str(),inputedText.length());
			}
			else if(c == VK_ESCAPE)
			{
				isTextModeEnabled = false;
				isDrawingModeEnabled = false;
			}
			else
			{
				DrawMainBMPToTempBMP(hWnd);
				inputedText += c;
				TextOut(tempBmp->GetDC(),mouseDownX, mouseDownY, inputedText.c_str(),inputedText.length());
			}
			InvalidateRect(hWnd,NULL,FALSE);
			UpdateWindow(hWnd);
		}
		break;
	case WM_MOUSEMOVE:
		if(wParam == MK_LBUTTON)
		{
			DrawMainBMPToTempBMP(hWnd);
			switch (selectedPaintTool)
			{
			case PTl_Pencil:
				mainBmp->BmpLineTo(GET_X_LPARAM(lParam)*scale + offsetX, GET_Y_LPARAM(lParam)*scale + offsetY);
				break;
			case PTl_Line:
			case PTl_Polygone:
			case PTl_Polyline:
				tempBmp->MoveTo(mouseDownX, mouseDownY);
				tempBmp->BmpLineTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			case PTl_Rectlange:
				Rectangle(tempBmp->GetDC(),mouseDownX,mouseDownY,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			case PTl_Ellipse:
				Ellipse(tempBmp->GetDC(),mouseDownX,mouseDownY,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			default:
				break;
			}
			isDrawingModeEnabled = true;
			InvalidateRect(hWnd, NULL, false);
			UpdateWindow(hWnd);
		}
		break;
	case WM_MOUSEWHEEL:
		if( GET_KEYSTATE_WPARAM(wParam) == MK_CONTROL)
		{
			offsetX += GET_WHEEL_DELTA_WPARAM(wParam);
		}
		else
		{
			offsetY += GET_WHEEL_DELTA_WPARAM(wParam);
		}
		if( GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
		{
			if(GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				scale /=1.25;
			}
			else
			{
				scale*=1.25;
			}
		}
		InvalidateRect(hWnd, NULL, true);
		UpdateWindow(hWnd);
		break;
	case WM_ERASEBKGND:
		GetClientRect(hWnd,&rect);
		FillRect(hdc,&rect,WHITE_BRUSH);
		break;
	case WM_CLOSE:
		return OnClose(hWnd);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



void DrawMainBMPToTempBMP(HWND hWnd)
{
	GetClientRect(hWnd,&rect);
	tempBmp->Clear(rect);
	RECT mainRect = rect;
	mainRect.left = offsetX;
	mainRect.top = offsetY;
	mainRect.right*=scale;
	mainRect.bottom*=scale;
	mainBmp->DrawToHDC(tempBmp->GetDC(),mainRect,rect);

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

// Message handler for close dialog.
INT_PTR CALLBACK CloseDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		int	wmId    = LOWORD(wParam);
		switch (wmId)
		{
		case IDSAVE:
			EndDialog(hDlg,SR_Save);
			break;
		case IDEXIT:
			EndDialog(hDlg,SR_Exit);
			break;
		case IDCANCEL:
			EndDialog(hDlg,SR_Cancel);
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
