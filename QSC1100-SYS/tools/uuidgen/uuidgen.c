/* ======================================================================
DESCRIPTION
   Creates UUID's and formats them to the command line.  Uses the
   Win32 COM API to generate the value.

Copyright (c) 2000, 2002 by QUALCOMM Incorporated.  All Rights Reserved.
========================================================================== */

/* =======================================================================
                           Edit History

$Header: //depot/asic/msm6100/tools/uuidgen/uuidgen.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/02   jct     Added command line option to output w/o hyphens
06/02/00   jct     Created
=========================================================================== */
#include <stdio.h>
#include <string.h>
#include <objbase.h>


/* ==================================================================
FUNCTION MAIN
DESCRIPTION
   Creates UUID's and formats them to the command line.  Uses the
   Win32 COM API to generate the value.
===================================================================== */
int main( int argc, char *argv[] )
{
   GUID guid;
   char guid_str[ 64 ];
   int  st = 0;

   if( argc > 1 )
   {
      if( strcmp( "-st", argv[1] ) == 0 )
      {
         st = 1;
      }
      else
      {
         printf( "uuidgen.exe\n" );
         printf( "Copyright (c) 2000,2001,2002 QUALCOMM Inc.\n" );
         printf( "All rights reserved.\n" );
         printf( "\n" );
         printf( "Generates unique id\n" );
         printf( "Usage:  uuidgen [-nh]\n" );
         return 0;
      }
   }

   (void)memset( (void*)guid_str, 0x00, 64 );

   CoInitialize(NULL);
   CoCreateGuid(&guid);

   if( st == 0 )
   {
      (void)sprintf( 
         guid_str, 
         "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
         guid.Data1, 
         guid.Data2, 
         guid.Data3,
         guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
         guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
      );
   }
   else
   {
      (void)sprintf( 
         guid_str, 
         "0x%08x,0x%04x%04x,0x%02x%02x%02x%02x,0x%02x%02x%02x%02x", 
         guid.Data1, 
         guid.Data2, 
         guid.Data3,
         guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
         guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
      );
   }

   printf( "%s\n", guid_str );

   CoUninitialize();   

   return 0;
}
