#include "ahci.h"
#include "../BasicRenderer.h"
#include "../paging/PageTableManager.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"

namespace AHCI {

    // HBA port Statuses. 
    #define HBA_PORT_DEV_PRESENT 0x3
    #define HBA_PORT_IPM_ACTIVE 0x1

    // ATA Type Signatures.
    #define SATA_SIG_ATAPI 0xEB140101
    #define SATA_SIG_ATA 0x00000101
    #define SATA_SIG_SEMB 0xC33C0101
    #define SATA_SIG_PM 0x96690101

    // Bitmask for HBA command status.
    #define HBA_PxCMD_CR 0x8000
    #define HBA_PxCMD_FRE 0x0010
    #define HBA_PxCMD_ST 0x0001
    #define HBA_PxCMD_FR 0x4000

    // Initializes AHCI Driver at pciBaseAdress.
    AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) {
        this->PCIBaseAddress = pciBaseAddress;
        GlobalRenderer->Print("AHCI Driver Initialized.");
        GlobalRenderer->Next();

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;

        g_PageTableManager.MapMemory(ABAR, ABAR);
        ProbePorts();

        for(int i = 0; i < portCount; i++){
            Port* port = ports[i];
            port->Configure();
        }
    }

    // Deconstructor for AHCI Driver.
    AHCIDriver::~AHCIDriver() {

    }

    // Probes all ports.
    void AHCIDriver::ProbePorts() {
        uint32_t portsImplemented = ABAR->portsImplemented;
        for(int i = 0; i < 32; i++) {
            if(portsImplemented & (1 << i)) {
                PortType portType = CheckPortType(&ABAR->ports[i]);

                if(portType == PortType::SATA || portType == PortType::SATAPI){
                    ports[portCount] = new Port();
                    ports[portCount]->portType = portType;
                    ports[portCount]->hbaPort = &ABAR->ports[i];
                    ports[portCount]->portNumber = portCount;
                    portCount++;
                }
            }
        }
    }

    // Returns PortType of port.
    PortType CheckPortType(HBAPort* port) {
        uint32_t sataStatus = port->sataStatus;

        uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
        uint8_t deviceDetection = sataStatus & 0b111;

        if(deviceDetection != HBA_PORT_DEV_PRESENT) return PortType::None;
        if(interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortType::None;

        switch(port->signature){
            case SATA_SIG_ATAPI:
                return PortType::SATAPI;
            case SATA_SIG_ATA:
                return PortType::SATA;
            case SATA_SIG_PM:
                return PortType::PM;
            case SATA_SIG_SEMB:
                return PortType::SEMB;
            default:
                PortType::None;
        }
    }

    // Configures port.
    void Port::Configure() {
        StopCMD();

        void* newBase = GlobalAllocator.RequestPage();
        hbaPort->commandListBase = (uint32_t)(uint64_t)newBase;
        hbaPort->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
        memset((void*)(hbaPort->commandListBase), 0, 1024);

        void* fisBase = GlobalAllocator.RequestPage();
        hbaPort->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
        hbaPort->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
        memset(fisBase, 0 , 256);

        HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)hbaPort->commandListBase + ((uint64_t)hbaPort->commandListBaseUpper << 32));

        for(int i = 0; i < 32; i++){
            cmdHeader[i].prdtLength = 8;

            void* cmdTableAddress = GlobalAllocator.RequestPage();
            uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
            cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t)address;
            cmdHeader[i].commandTableBaseAddress = (uint32_t)((uint64_t)address >> 32);
            memset(cmdTableAddress, 0, 256);
        }

        StartCMD();
    }

    // Starts a command.
    void Port::StartCMD() {
        while(hbaPort->cmdSts & HBA_PxCMD_CR);

        hbaPort->cmdSts |= HBA_PxCMD_FRE;
        hbaPort->cmdSts |= HBA_PxCMD_ST;
    }

    // Ends a command.
    void Port::StopCMD() {
        hbaPort->cmdSts &= ~HBA_PxCMD_ST;
        hbaPort->cmdSts &= ~HBA_PxCMD_FRE;

        while (true) {
            if(hbaPort->cmdSts & HBA_PxCMD_FR) continue;
            if(hbaPort->cmdSts & HBA_PxCMD_CR) continue;

            break;
        }
    }
}