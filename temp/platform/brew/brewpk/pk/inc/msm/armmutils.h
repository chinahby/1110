#ifndef __ARMUTILS_H__
#define __ARMUTILS_H__


enum
{
APERM_NONE=0,
APERM_READONLY=1,
APERM_READWRITE=2,
APERM_UNPREDICTABLE=3
} MMU_ACCESSPERMISSIONS;


enum
{
DOMAIN_NOACCESS=0,
DOMAIN_CLIENT=1,
DOMAIN_RESERVED=2,
DOMAIN_MANAGER=3
}MMU_DOMAINS;

#define B_0_0 0
#define B_0_1 1
#define B_1_0 2
#define B_1_1 3


unsigned long GetFirstLevelDescriptor(unsigned long virtualAddress);
unsigned long TranslateSection(unsigned long firstdesc, unsigned long virtualAddress, int *pAP, int *pDomain);
unsigned long TranslateCoarsePageTable(unsigned long firstdesc, unsigned long virtualAddress, int*pAP, int *pDomain);
unsigned long TranslateFinePageTable(unsigned long firstdesc, unsigned long virtualAddress,int*AP, int *dom );
unsigned long SlowTranslateVirtualAddress( unsigned long virtualAddress, int *AP, int *Domain);
unsigned long GetStringLength(char * str, unsigned long);
void SetSRBits(int S, int R);

#endif

