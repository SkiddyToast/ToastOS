#include "panic.h"
#include "BasicRenderer.h"

void Panic(const char* panicMessgage)
{
    GlobalRenderer->ClearColor = 0x008a0000;
    GlobalRenderer->Clear();
    GlobalRenderer->CursorPosition = {GlobalRenderer->horizontalCenter("Kernel Panic"), GlobalRenderer->verticalCenter() - 8};
    GlobalRenderer->Color = 0;
    GlobalRenderer->Print("Kernel Panic");
    GlobalRenderer->Next();
    GlobalRenderer->CursorPosition = {GlobalRenderer->horizontalCenter(panicMessgage), GlobalRenderer->CursorPosition.Y};
    GlobalRenderer->Print(panicMessgage);
}