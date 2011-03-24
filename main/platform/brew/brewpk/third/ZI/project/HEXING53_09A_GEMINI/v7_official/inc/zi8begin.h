/* zi8begin.h
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */

/* This file provides a way for causing the compiler to allocate large tables
** in an appropriate portion of the address space of the target system.
** This is normally done through a compiler pragma.
*/

#ifdef ZI8_CHANGE_CONSTSEG
#pragma memory=constseg(ZI8_HUGECONST):huge
#endif 
