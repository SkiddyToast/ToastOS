#include "kernelUtil.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

// Kernel Main
extern "C" void _start(BootInfo* bootInfo){
	KernelInfo kernelInfo = InitializeKernel(bootInfo);

	char* name = "ToastOS ";
	char* version = "v0.1";

	GlobalRenderer->Print(concat(concat(name, " "), version));
	GlobalRenderer->Next();



    while(true){
		asm ("hlt");
	}
}