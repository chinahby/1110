#ifndef NF3APPSTATIC_H
#define NF3APPSTATIC_H

extern int CNetFront3_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int NF3_Load( IShell* ps, void* pHelpers, IModule** pMod );

extern int NF3_CreateInstance( AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj );

#endif //NF3APPSTATIC_H
