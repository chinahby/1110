/*
  ========================================================================

  FILE:  AEEModel.h
  
  SERVICES: Defines the basic model interface

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEMODEL_H__
#define __AEEMODEL_H__

#include "AEE.h"

/* new definition */
#include "../../inc/AEEIModel.h"

/* added missing symbols */
#define AEEIID_MODEL AEEIID_IModel
#define IMODEL_AddListener IModel_AddListener
#define IMODEL_AddListenerEx IModel_AddListenerEx
#define IMODEL_AddRef IModel_AddRef
#define IMODEL_Notify IModel_Notify
#define IMODEL_QueryInterface IModel_QueryInterface
#define IMODEL_Release IModel_Release


#endif /* __AEEMODEL_H__ */
