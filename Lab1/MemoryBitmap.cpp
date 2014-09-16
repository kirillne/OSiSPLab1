#include "stdafx.h"
#include "MemoryBitmap.h"



MemoryBitmap::MemoryBitmap(RECT size)
{
	brush=static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	pen=CreatePen(PS_SOLID, 0, RGB(0,0,0)); 
	DeleteObject(SelectObject(memDC, pen)); 


	memDC = CreateCompatibleDC(NULL);
	Clear(size);
	DeleteObject(SelectObject(memDC, static_cast<HBRUSH>(WHITE_BRUSH))); 

	PatBlt(memDC, 0,0, size.right, size.bottom,PATCOPY);

	DeleteObject(SelectObject(memDC,pen));
	DeleteObject(SelectObject(memDC,brush));

}


void MemoryBitmap::SetPenWidth(int penWidth)
{
	DeleteObject(pen);
	pen=CreatePen(PS_SOLID, penWidth,  RGB(0,0,0)); 
	DeleteObject(SelectObject(memDC, pen)); 
}


HDC MemoryBitmap::GetDC()
{
	return memDC;
}

void MemoryBitmap::Clear( RECT size)
{
	DeleteBitmap(bmp);
	bmp = CreateCompatibleBitmap(memDC,size.right,size.bottom);
	DeleteObject(SelectObject(memDC,bmp));
}

void MemoryBitmap::DrawToHDC(HDC hdc, RECT size)
{	 
	BitBlt( hdc, 0, 0, size.right,size.bottom, memDC, 0, 0,SRCCOPY );
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
