#pragma once

typedef PVOID HDEVINFO;

#define SPDRP_DEVICEDESC                  (0x00000000)  // DeviceDesc (R/W)

#define DICS_FLAG_GLOBAL         0x00000001  // make change in all hardware profiles

#define DIGCF_PRESENT           0x00000002

#define DIGCF_DEVICEINTERFACE   0x00000010

#define DIF_PROPERTYCHANGE                  0x00000012

#define DICS_ENABLE      0x00000001
#define DICS_DISABLE     0x00000002

typedef UINT        DI_FUNCTION;    // Function type for device installer


/*typedef struct _SP_CLASSINSTALL_HEADER {
    DWORD       cbSize;
    DI_FUNCTION InstallFunction;
} SP_CLASSINSTALL_HEADER, *PSP_CLASSINSTALL_HEADER;

typedef struct _SP_PROPCHANGE_PARAMS {
    SP_CLASSINSTALL_HEADER ClassInstallHeader;
    DWORD                  StateChange;
    DWORD                  Scope;
    DWORD                  HwProfile;
} SP_PROPCHANGE_PARAMS, *PSP_PROPCHANGE_PARAMS;

typedef DWORD ULONG_PTR;

typedef struct _SP_DEVINFO_DATA {
    DWORD cbSize;
    GUID  ClassGuid;
    DWORD DevInst;    // DEVINST handle
    ULONG_PTR Reserved;
} SP_DEVINFO_DATA, *PSP_DEVINFO_DATA;
*/