// CWinAddrBook
//
// Created on: 12/22/2002 
// Written by Sam C
//
// Contributors Source code I found on the web don't know where, but I have included the full source in the ZIP and if
//				anyone knows the author please let me know, Hirosh from CodeProject and his article entitled "Extracting Email ID's from an address book using Memory mapped file"
//
// License: I grant anyone the right to use or distribute this source code for personal or commercial purposes. The only restriction
// is that you must keep the comments at top (the comment file you are reading now) intact, and no changes made to them whatsoever
//
// Copyright 2002 Stuffsoft
// http://www.stuffsoft.com info@stuffsoft.com
//

#pragma once

#include <wab.h>
#include <vector>
#include <string>
#include "data\phonebookpc.h"


using namespace std;

//Typedef the function pointer to WABOpen
typedef HRESULT (WINAPI *fWABOpen)( 
	LPADRBOOK* ,
    LPWABOBJECT* ,
    LPWAB_PARAM ,
    DWORD 
);

typedef struct __WABS {
	CString DisplayName; //char DisplayName[200];
	CString EmailAddr; //char EmailAddr[200];
	CString homePhone; // home phone number;
	CString mobilePhone; //mobile phone number;
	CString homeCountry;//home country
	CString homePostCode;//home post code
	CString homeProvince;//home province
	CString homeCity;//   home city
	CString homeStreetAddr;// home street address
	CString homeFax;//home fax
	CString company;//company
	CString officePhone; //office phone number;
	CString officeCountry;//company Country
	CString officePostCode;//company post code
	CString officeProvince;//company province
	CString officeCity;//company city
	CString officeStreetAddr;//company street address
    CString officeDuty;   //personal duty
	CString officeDepartment;//department
	CString officebureaux;//bureaux
	CString officeFax;//office fax
	CString officeCallnum;//Ñ°ºô»ú
	CString officeIPPhone;// office IP phone

} WABS, *LPWABS;

class CWinAddrBook
{
public:
	CWinAddrBook(void);
	~CWinAddrBook(void);
	// Module to library that is opened
	TCHAR PathToWAB[255];
	//If hModule is NULL then the library is not loaded yet
	HMODULE hModule;
	// Initialize the address book and load the values into memory (I hope) Return 0 if successful, anything else failed
	int InitAddrBook(void);
	vector<WABS> AddrMemBook;	

private:
	// Pointer to the IAddressBook interface
	LPADRBOOK lpAddrBook;
	//Pointer to the WAB object itself that we are using
	LPWABOBJECT lpWABObject;
	// Pointer to the function inside of the wab32.dll for the open procedure
	fWABOpen ssWABOpen;
	// Load email addresses into the vector
	
	void Loadcontainer(void* pAB);
	void LoadAddr(void* pAB);
	//void InsertAddr(void* pAB,const TPhoneBookItemEx &item);
	void InsertAddr(void* pAB, const CPhonebookPC& itemList);

	void LoadABContainer(void* pContainer);
	HRESULT TestAddress(LPMESSAGE pMsg);
	void LoadTable(void* pTable);
	void LoadRows(void* pRowset);
	void GetRowInfo(void* pRow);
	void LoadMailUser(void* lPropVal, WABS& info);
	void GetMailUserInfo(void* pUser, WABS& info);
public:
	void LoadWABs();	
	void InsertWABs(const CPhonebookPC& itemList);
	
	
public:
	// Test mostly for debug purposes so you can step through and ensure that all the information is present for the address book vector
	void TestAddrBook(void);
	// Tells if the class has already been initialized or it has not been initialized
	bool IsInit;
};
