#include "kernelUtil.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

extern "C" void _start(BootInfo* bootInfo){

	KernelInfo kernelInfo = InitializeKernel(bootInfo);
	PageTableManager* pageTableManager = kernelInfo.pageTableManager;

	PIT::SetDivisor(65535);

	GlobalRenderer->Print("ToastOS v0.1");
	GlobalRenderer->Next();

    while(true);
}