#include "BasicRenderer.h"

BasicRenderer* GlobalRenderer;

BasicRenderer::BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font)
{
	TargetFramebuffer = targetFramebuffer;
	PSF1_Font = psf1_Font;
	Color = 0xffffffff;
	CursorPosition = {0, 0};
}

void BasicRenderer::putPix(uint32_t x, uint32_t y, uint32_t color) {
	*(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4)) = color;
}

uint32_t BasicRenderer::getPix(uint32_t x, uint32_t y)
{
	return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4));
}

void BasicRenderer::ClearMouseCursor(uint8_t* mouseCursor, Point position)
{
	if(!MouseDrawn) return;
	int xMax = 16;
	int yMax = 16;
	int differenceX = TargetFramebuffer->Width - position.X;
	int differenceY = TargetFramebuffer->Height - position.Y;

	if(differenceX < 16) xMax = differenceX;
	if(differenceY < 16) yMax = differenceY;

	for(int y = 0; y < yMax; y++)
	{
		for(int x = 0; x < xMax; x++)
		{
			int bit = y * 16 + x;
			int byte = bit / 8;
			if((mouseCursor[byte] & (0b10000000 >> (x % 8))))
			{
				if(getPix(position.X + x, position.Y + y) == MouseBufferAfter[x + y * 16]){
					putPix(position.X + x, position.Y + y, MouseBuffer[x + y * 16]);
				}
			}
		}
	}
}

void BasicRenderer::DrawOverlayMouseCursor(uint8_t* mouseCursor, Point position, uint32_t color)
{
	int xMax = 16;
	int yMax = 16;
	int differenceX = TargetFramebuffer->Width - position.X;
	int differenceY = TargetFramebuffer->Height - position.Y;

	if(differenceX < 16) xMax = differenceX;
	if(differenceY < 16) yMax = differenceY;

	for(int y = 0; y < yMax; y++)
	{
		for(int x = 0; x < xMax; x++)
		{
			int bit = y * 16 + x;
			int byte = bit / 8;
			if((mouseCursor[byte] & (0b10000000 >> (x % 8))))
			{
				MouseBuffer[x + y * 16] = getPix(position.X + x, position.Y + y);
				putPix(position.X + x, position.Y + y, color);
				MouseBufferAfter[x + y * 16] = getPix(position.X + x, position.Y + y);
			}
		}
	}
	MouseDrawn = true;
}

unsigned int BasicRenderer::strLength(const char* str)
{
	unsigned int length = 0;
	while(*str++){
		++length;
	}
	return length;
}

unsigned int BasicRenderer::horizontalCenter(const char* str){
	return (unsigned int)(TargetFramebuffer->Width / 2) - ((strLength(str) * 8) / 2);
}

unsigned int BasicRenderer::verticalCenter(){
	return (unsigned int)(TargetFramebuffer->Height / 2);
}

void BasicRenderer::Clear()
{
	uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
	uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
	uint64_t fbHeight = TargetFramebuffer->Height;
	uint64_t fbSize = TargetFramebuffer->BufferSize;

	for(int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline++)
	{
		uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
		for(uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr++)
		{
			*pixPtr = ClearColor;
		}
	}
}

void BasicRenderer::clearChar()
{
	if(CursorPosition.X == 0){
		CursorPosition.X = TargetFramebuffer->Width;
		CursorPosition.Y -= 16;
		if(CursorPosition.Y < 0) CursorPosition.Y = 0;
	}

	unsigned int xOff = CursorPosition.X;
	unsigned int yOff = CursorPosition.Y;

	unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
	for (unsigned long y = yOff; y < yOff + 16; y++){
		for (unsigned long x = xOff - 8; x < xOff; x++){
			*(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = ClearColor;
		}
	}

	CursorPosition.X -= 8;
	if(CursorPosition.X < 0)
	{
		CursorPosition.X = TargetFramebuffer->Width;
		CursorPosition.Y -= 16;
		if(CursorPosition.Y < 0) CursorPosition.Y = 0;
	}
}

void BasicRenderer::Next()
{
	CursorPosition.X = 0;
	CursorPosition.Y += 16;
}

void BasicRenderer::putChar(char chr, unsigned int xOff, unsigned int yOff)
{
	unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
	char* fontPtr = (char*)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
	for (unsigned long y = yOff; y < yOff + 16; y++){
		for (unsigned long x = xOff; x < xOff + 8; x++){
			if((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
				*(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = Color;
			}
		}
		fontPtr++;
	}
}

void BasicRenderer::putChar(char chr){
	putChar(chr, CursorPosition.X, CursorPosition.Y);
	CursorPosition.X += 8;
	if(CursorPosition.X + 8 > TargetFramebuffer->Width)
	{
		CursorPosition.X = 0;
		CursorPosition.Y += 16;
	}
}

void BasicRenderer::Print(const char* str)
{
	char* chr = (char*)str;
	while(*chr != 0){
		putChar(*chr, CursorPosition.X, CursorPosition.Y);
		CursorPosition.X+=8;
		if(CursorPosition.X +8 > TargetFramebuffer->Width)
		{
			CursorPosition.X = 0;
			CursorPosition.Y += 16;
		}
		chr++;
	}
}