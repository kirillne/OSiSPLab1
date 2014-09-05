#include "stdafx.h"
#include "MemoryBitmap.h"

HDC memDC;
HBITMAP bmp;
HBRUSH brush;
int width, height;

MemoryBitmap::MemoryBitmap(int bmpWidth, int bmpHeight)
{
	width = bmpWidth;
	height = bmpHeight;
	brush = CreateSolidBrush(RGB(0xff, 0xff, 0xff));
	memDC = CreateCompatibleDC(NULL);
	bmp = CreateCompatibleBitmap(memDC, bmpWidth, bmpHeight);
	SelectObject(memDC, bmp);
	SelectObject(memDC, brush);
	PatBlt(memDC, 0,0, bmpWidth, bmpHeight,PATCOPY);

}

void MemoryBitmap::DrawToForm(HDC hdc)
{
	BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
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
}
