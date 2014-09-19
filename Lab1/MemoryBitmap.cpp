#include "stdafx.h"
#include "MemoryBitmap.h"



MemoryBitmap::MemoryBitmap(RECT size, HDC hDC)
{
	brush=static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));

	this->hdc = hDC;
	penWidth = 1;
	penColor = RGB(0,0,0);
	pen=CreatePen(PS_SOLID, penWidth, penColor); 

	memDC = CreateCompatibleDC(hDC);
	Clear(size);
	DeleteObject(SelectObject(hDC, static_cast<HBRUSH>(WHITE_BRUSH))); 
	PatBlt(memDC, 0,0, size.right, size.bottom,PATCOPY);

	DeleteObject(SelectObject(memDC,pen));
	DeleteObject(SelectObject(memDC,brush));

}


void MemoryBitmap::SetPenWidth(int penWidth)
{
	this->penWidth = penWidth;
	pen=CreatePen(PS_SOLID, penWidth,  penColor); 
	DeleteObject(SelectObject(memDC, pen));
}

void MemoryBitmap::SetPenColor(COLORREF color)
{
	this->penColor = color;
	pen=CreatePen(PS_SOLID, penWidth,  penColor); 
	DeleteObject(SelectObject(memDC, pen));
}

void MemoryBitmap::SetBrushColor(COLORREF color)
{
	brush= CreateSolidBrush(color); 
	DeleteObject(SelectObject(memDC, brush));
}

void MemoryBitmap::SetEmptyBrush()
{
	brush= static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	DeleteObject(SelectObject(memDC, brush));
}

HDC MemoryBitmap::GetDC()
{
	return memDC;
}

void MemoryBitmap::Clear( RECT size)
{
	bmp = CreateCompatibleBitmap(hdc,size.right,size.bottom);
	DeleteObject(SelectObject(memDC,bmp));
}


void MemoryBitmap::BmpLineTo(int x,  int y)
{
	LineTo(memDC, x, y);
}

void MemoryBitmap::MoveTo(int x,  int y)
{
	MoveToEx(memDC, x, y, NULL);
}

MemoryBitmap::~MemoryBitmap(void)
{
	DeleteObject(bmp);
	DeleteObject(memDC);
	DeleteObject(brush);
	DeleteObject(pen);
}

void MemoryBitmap::DrawToHDC(HDC secHDC, RECT thisSize, RECT sourseSize)
{
	StretchBlt(secHDC,sourseSize.left, sourseSize.top,sourseSize.right, sourseSize.bottom, memDC,thisSize.left, thisSize.top,thisSize.right, thisSize.bottom,SRCCOPY );
}