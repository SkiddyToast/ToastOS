#include <stddef.h>
#include "BasicRenderer.h"
#include "cstr.h"

extern "C" void _start(Framebuffer* framebuffer, PSF1_FONT* psf1_font){
	BasicRenderer newRenderer = BasicRenderer(framebuffer, psf1_font);
	newRenderer.Print(to_string((uint64_t) 1234976));
	newRenderer.CursorPosition = {0, 16};
	newRenderer.Print(to_string((int64_t)-1234976));
	newRenderer.CursorPosition = {0, 32};
	newRenderer.Print(to_string((double)13.14, 2));
    return;
}