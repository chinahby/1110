/*=====================================================================
FILE:  StaticExtensionInc.h

SERVICES:  typedefs for Static Extension DLLs 

DESCRIPTION:  
	The following ifdef block is the standard way of creating macros which make 
	exporting from Static Extension DLL simpler. All files within this DLL are 
	compiled with the BREWSTATICEXTENSION_EXPORTS symbol defined on the command 
	line. 

	BREW Simulator is capable of loading Static Extension DLLs that are located
	in .\Modules directory relative to it's location. It looks for the following
	exported function from each of the Static Extension DLLs :
		const AEEStaticClass* GetStaticClassEntries(void)

	By defining the function as :
		BREWSTATICEXTENSION_API const AEEStaticClass* GetStaticClassEntries(void)
	ensures that it is exposed from the Static Extension dll to be loaded by
	BREW Simulator. See DemoExtension.c for details 
        
        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/


#ifdef BREWSTATICEXTENSION_EXPORTS
#define BREWSTATICEXTENSION_API __declspec(dllexport)
#else
#define BREWSTATICEXTENSION_API __declspec(dllimport)
#endif

