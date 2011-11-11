#include "StdAfx.h"
#include "Phonebook.h"


//****************************************
//	class TPhoneBookItemEx
//****************************************

bool TPhoneBookItemEx::operator==(const TPhoneBookItemEx& item) const
{
	return	strName == item.strName &&
			strMobilePhone == item.strMobilePhone &&
			strHomePhone == item.strHomePhone &&
			strOffice == item.strOffice &&
			strEmail == item.strEmail;
}

TPhoneBookItemEx::TPhoneBookItemEx()
{
	nID = 0;
	strTypeName.Empty();
	strBusiness.Empty();
	strEmail.Empty();
	strCountry.Empty();
	strHomePhone.Empty();
	strFax.Empty();
	strOffice.Empty();
	strOfficePhone.Empty();
}

BOOL TPhoneBookItemEx::operator=(const TPhoneBookItemEx& item)
{
	strName = item.strName ;
	strMobilePhone = item.strMobilePhone ;
	strHomePhone = item.strHomePhone ;
	strOffice = item.strOffice ;
	strEmail = item.strEmail ;
	nID = item.nID ;
	nParentID = item.nParentID ;
	strTypeName = item.strTypeName ;
	strBusiness = item.strBusiness ;
	strCountry = item.strCountry ;
	strFax = item.strFax ;
	strOfficePhone = item.strOfficePhone;

	return TRUE;
}