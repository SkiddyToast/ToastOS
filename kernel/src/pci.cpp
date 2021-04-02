#include "pci.h"
#include "ahci/ahci.h"
#include "memory/heap.h"

namespace PCI {

    // Sets up Functions
    void EnumerateFunction(uint64_t deviceAddress, uint64_t function)
    {
        uint64_t offset = function << 12;

        uint64_t functionAddress = deviceAddress + offset;
        g_PageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);

        PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)functionAddress;

        if(pciDeviceHeader->DeviceID == 0) return;
        if(pciDeviceHeader->DeviceID == 0xFFFF) return;

        switch(pciDeviceHeader->Class){
            case 0x01:
                switch(pciDeviceHeader->Subclass){
                    case 0x06:
                        switch(pciDeviceHeader->ProgIF){
                            case 0x01:
                                new AHCI::AHCIDriver(pciDeviceHeader);
                        }
                }
        }
    }

    // Enumerates through all Functions on specfied device.
    void EnumerateDevice(uint64_t busAddress, uint64_t device)
    {
        uint64_t offset = device << 15;

        uint64_t deviceAddress = busAddress + offset;
        g_PageTableManager.MapMemory((void*)deviceAddress, (void*)deviceAddress);

        PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)deviceAddress;

        if(pciDeviceHeader->DeviceID == 0) return;
        if(pciDeviceHeader->DeviceID == 0xFFFF) return;

        for(uint64_t function = 0; function < 8; function++){
            EnumerateFunction(deviceAddress, function);
        }
    }

    // Enumerates through all devices on specified bus.
    void EnumerateBus(uint64_t baseAddress, uint64_t bus)
    {
        uint64_t offset = bus << 20;

        uint64_t busAddress = baseAddress + offset;
        g_PageTableManager.MapMemory((void*)busAddress, (void*)busAddress);

        PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)busAddress;

        if(pciDeviceHeader->DeviceID == 0) return;
        if(pciDeviceHeader->DeviceID == 0xFFFF) return;

        for(uint64_t device = 0; device < 32; device++){
            EnumerateDevice(busAddress, device);
        }
    }

    // Enumerates through all busses on PCI.
    void EnumeratePCI(ACPI::MCFGHeader* mcfg)
    {
        int entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
        for(int t = 0; t < entries; t++)
        {
            ACPI::DeviceConfig* newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));
            for(uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++){
                EnumerateBus(newDeviceConfig->BaseAddress, bus);
            }
        }
    }

}