#include "kernelUtil.h"

extern "C" void _start(BootInfo* bootInfo){

	KernelInfo kernelInfo = InitializeKernel(bootInfo);
	PageTableManager* pageTableManager = kernelInfo.pageTableManager;

	GlobalRenderer->Print("Kernel Initialized Successfully");
	GlobalRenderer->Next();

	while(true){
		ProcessMousePacket();
	}

    while(true);
}