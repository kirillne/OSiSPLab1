#pragma once
class MemoryBitmap
{
public:
	MemoryBitmap(RECT size, HDC hDC);
	~MemoryBitmap(void);
	void DrawToHDC(HDC secHDC, RECT thisSize, RECT sourseSize );
	void MoveTo(int x, int y);
	void BmpLineTo(int x, int y);
	void Clear(RECT size);
	HDC GetDC();
	void SetPenWidth(int penWidth);
	void SetPenColor(COLORREF color);
	void SetBrushColor(COLORREF color);
	void SetEmptyBrush();
private:
	HDC memDC;
	HBITMAP bmp;
	HBRUSH brush;
	HPEN pen;
	int penWidth;
	COLORREF penColor;
	HDC hdc;

};

