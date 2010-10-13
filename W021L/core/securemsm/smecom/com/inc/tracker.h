#ifndef __RESTRACKER_H__
#define __RESTRACKER_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H E A D E R   F O R
                 R E S T R A C K E R    M O D U L E

GENERAL DESCRIPTION
  This module provides a simple resource tracking mechanism for things that 
  are allocated, opened, or otherwise used, where the termination of the 
  object is required.  A new trackerlist can be made for any type of resource,
  and when needed, the restrack_dump() function can be called to show all
  currently used objects.  At the end of a program, this would show you what
  was left open/allocated/etc.

  See useage of functions in header file.

EXTERNALIZED FUNCTIONS
  restrack_init
  restrack_add
  restrack_delete
  restrack_summary
  restrack_dump
  restrack_clear

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: 

when         who     what, where, why
--------     ---     --------------------------------------------------------
05/17/-7     avm     Fixed Compiler Warnings.
03/28/07      je     Initial Revision.

===========================================================================*/

#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  definitions and constants
===========================================================================*/
#define RESTRACKER_FILETASK_MAX 128

/*===========================================================================
  type declarations
===========================================================================*/
typedef struct t_trackrec {
   unsigned long __ptr;
   unsigned long __size;
   unsigned long __count;
   char __file[RESTRACKER_FILETASK_MAX];
   char __task[RESTRACKER_FILETASK_MAX];
   int __line;
   struct t_trackrec *__prev;
   struct t_trackrec *__next;
} trackerrec;

typedef struct t_trackerlist {
   trackerrec *__head;
   trackerrec *__tail;
   int __count;
   int __total;
   int __maxcount;
   int __maxtotal;
   char __name[RESTRACKER_FILETASK_MAX];
   rex_crit_sect_type crit_sect;
} trackerlist;

/*===========================================================================
  external function declarations
===========================================================================*/
void restrack_init( trackerlist *list, const char *name );
void restrack_add( trackerlist *list, trackerrec *ipr );
void restrack_delete( trackerlist *list, unsigned long ptr );
void restrack_summary( const trackerlist *list );
void restrack_dump( const trackerlist *list );
void restrack_clear( trackerlist *list );

/*===========================================================================
  global variable declarations 
===========================================================================*/
extern trackerlist g_timerlist;   // track timers
extern trackerlist g_heaplist;    // track memory allocations
extern boolean g_heaplistinit;

// rex_init_crit_sect, rex_enter_crit_sect and rex_leave_crit_sect.

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __RESTRACKER_H__

