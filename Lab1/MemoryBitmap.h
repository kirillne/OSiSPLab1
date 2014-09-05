#pragma once
class MemoryBitmap
{
public:
	MemoryBitmap(int bmpWidth, int bmpHeight);
	~MemoryBitmap(void);
	void DrawToForm(HDC hdc);
	void MoveTo(int x, int y);
	void BmpLineTo(int x, int y);
};

