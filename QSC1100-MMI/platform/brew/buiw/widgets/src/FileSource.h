/*
   ========================================================================
   
   FILE:       FileSource.h
   
   SERVICES:   IFile wrapper to implement ISource sub-sections of a file.
   
   GENERAL DESCRIPTION:
   
   Since this is a private interface, this file mixes together 
   interface definition and implementation. 

   Defines a private IFileStream interface that acts as a wrapper for an
   IFile, allowing a client to specify a stream size and then provide 
   end-of-stream 
   
   
    
   
   ========================================================================
   ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
   ========================================================================
   ========================================================================
*/
#ifndef __FILESOURCE_H__
#define __FILESOURCE_H__


int FileSource_New(ISource **ppo, IFile *piFile, uint32 dwFileOffset, uint32 dwStreamSize);



#endif   //__FILESOURCE_H__

   


