#pragma once
#include "math.h"
#include "Framebuffer.h"
#include "simpleFonts.h"
#include <stdint.h>

class BasicRenderer{
    public:
    BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font);
    Point CursorPosition;
    Framebuffer* TargetFramebuffer;
    PSF1_FONT* PSF1_Font;
    uint32_t MouseBuffer[16 * 16];
    uint32_t MouseBufferAfter[16 * 16];
    unsigned int Color;
    uint32_t ClearColor;
    void Print(const char* str);
    void putPix(uint32_t x, uint32_t y, uint32_t color);
    void putChar(char chr, unsigned int xOff, unsigned int yOff);
    void putChar(char chr);
    uint32_t getPix(uint32_t x, uint32_t y);
    void clearChar();
    void Clear();
    void Next();
    void ClearMouseCursor(uint8_t* mouseCursor, Point position);
    void DrawOverlayMouseCursor(uint8_t* mouseCursor, Point position, uint32_t color);
    unsigned int strLength(const char* str);
    unsigned int horizontalCenter(const char* str);
    unsigned int verticalCenter();
    bool MouseDrawn;
};

extern BasicRenderer* GlobalRenderer;