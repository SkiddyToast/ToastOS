#include <stdint.h>
#include "cstr.h"

namespace PCI {
    const char* DeviceClasses[] {
        "Unclassified",
        "Mass Storage Controller",
        "Network Controller",
        "Display Controller",
        "Multimedia Controller",
        "Memory Controller",
        "Bridge Device",
        "Simple Communication Controller",
        "Base System Peripheral",
        "Input Device Controller",
        "Docking Station", 
        "Processor",
        "Serial Bus Controller",
        "Wireless Controller",
        "Intelligent Controller",
        "Satellite Communication Controller",
        "Encryption Controller",
        "Signal Processing Controller",
        "Processing Accelerator",
        "Non Essential Instrumentation"
    };

    const char* GetVendorName(uint16_t vendorID){
        switch (vendorID) {
            case 0x8086:
                return "Intel Corp";
            case 0x1022:
                return "AMD";
            case 0x10DE:
                return "NVIDIA Corporation";
        }
        return to_hstring(vendorID);
    }

    const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID){
        switch (vendorID){
            case 0x8086:
                switch (deviceID){
                    case 0x29C0:
                        return "Express DRAM Controller";
                    case 0x2918:
                        return "LPC Interface Controller";
                    case 0x2922:
                        return "6 port SATA Controller [AHCI mode]";
                    case 0x2930:
                        return "SMBus Controller";
                }
        }
        return to_hstring(deviceID);
    }

    const char* MassStorageControllerSubclassName(uint8_t subclassCode){
        switch (subclassCode){
            case 0x00:
                return "SCSI Bus Controller";
            case 0x01:
                return "IDE Controller";
            case 0x02:
                return "Floppy Disk Controller";
            case 0x03:
                return "IPI Bus Controller";
            case 0x04:
                return "RAID Controller";
            case 0x05:
                return "ATA Controller";
            case 0x06:
                return "Serial ATA";
            case 0x07:
                return "Serial Attached SCSI";
            case 0x08:
                return "Non-Volatile Memory Controller";
            case 0x80:
                return "Other";
        }
        return to_hstring(subclassCode);
    }

    const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode){
        switch (classCode){
            case 0x01:
                return MassStorageControllerSubclassName(subclassCode);
        }
        return to_hstring(subclassCode);
    }

    const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF){
        switch (classCode){
            case 0x01:
                switch (subclassCode){
                    case 0x06:
                        switch (progIF){
                            case 0x00:
                                return "Vendor Specific Interface";
                            case 0x01:
                                return "AHCI 1.0";
                            case 0x02:
                                return "Serial Storage Bus";
                        }
                }
        }
        return to_hstring(progIF);
    }
}