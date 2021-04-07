#include "acpi.h"

namespace ACPI {

    void* FindTable(XSDT* xsdt, char* signature)
    {
        int xsdtEntries = (xsdt->Header.Length - sizeof(xsdt->Header)) / 8;
        for(int i = 0; i < xsdtEntries; i++) {
            ACPI::SDTHeader* h = (ACPI::SDTHeader *)xsdt->Entries[i];
            for (int i = 0; i < 4; i++){
                if (h->Signature[i] != signature[i])
                {
                    break;
                }
                if (i == 3) return (ACPI::SDTHeader *)h;
            }
        }

        return 0;
    }

}