/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _XML_H_
#define _XML_H_

#include <jos.h>

struct xml_elm;
typedef struct xml_elm *xml_elm_h;

jresult_t  xml_init  (void);
void      xml_uninit(void);

xml_elm_h xml_parse(char *buff, juint32_t size);
void      xml_free (xml_elm_h root_elm);

xml_elm_h xml_get_child_first(xml_elm_h parent_elm);
xml_elm_h xml_get_parent     (xml_elm_h elm);
xml_elm_h xml_get_next       (xml_elm_h elm);

char *xml_get_name(xml_elm_h elm);
char *xml_get_cdata(xml_elm_h elm);
char *xml_get_attr_data(xml_elm_h elm, char *attr_name);

#ifdef JDEBUG
void xml_dump(xml_elm_h root_elm);
#endif

#endif
