#pragma once
class MemoryBitmap
{
public:
	MemoryBitmap(RECT size);
	~MemoryBitmap(void);
	void DrawToHDC(HDC hdc, RECT size);
	void MoveTo(int x, int y);
	void BmpLineTo(int x, int y);
	void Clear(RECT size);
	HDC GetDC();
	void SetPenWidth(int penWidth);

private:
	HDC memDC;
	HBITMAP bmp;
	HBRUSH brush;
	HPEN pen;
};

