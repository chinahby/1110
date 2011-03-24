/* zi8end.h
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */

/* This file provides a way for causing the compiler to restore its normal
** allocation of constant data for the target system.
** This is normally done through a compiler pragma.
*/

#ifdef ZI8_CHANGE_CONSTSEG
#pragma memory=default
#endif 
