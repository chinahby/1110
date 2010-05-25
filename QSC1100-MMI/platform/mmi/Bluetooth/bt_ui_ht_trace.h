
#ifndef _BTUI_TRACE_H_
#define _BTUI_TRACE_H_

#include "bt_ui_platform.h"

//#define	_BTUI_USE_TRACE_ 
#ifdef	_BTUI_USE_TRACE_


/* define traces for Application */                        

#define BT_UI_APPL_TRACE_ERROR0(m)                       DBGPRINTF_ERROR(m)                    
#define BT_UI_APPL_TRACE_ERROR1(m,p1)                    DBGPRINTF_ERROR(m,p1)                 
#define BT_UI_APPL_TRACE_ERROR2(m,p1,p2)                 DBGPRINTF_ERROR(m,p1,p2)              
#define BT_UI_APPL_TRACE_ERROR3(m,p1,p2,p3)              DBGPRINTF_ERROR(m,p1,p2,p3)           
#define BT_UI_APPL_TRACE_ERROR4(m,p1,p2,p3,p4)           DBGPRINTF_ERROR(m,p1,p2,p3,p4)        
#define BT_UI_APPL_TRACE_ERROR5(m,p1,p2,p3,p4,p5)        DBGPRINTF_ERROR(m,p1,p2,p3,p4,p5)     
#define BT_UI_APPL_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)     DBGPRINTF_ERROR(m,p1,p2,p3,p4,p5,p6)  
        
#define BT_UI_APPL_TRACE_WARNING0(m)                     DBGPRINTF_LOW(m)                  
#define BT_UI_APPL_TRACE_WARNING1(m,p1)                  DBGPRINTF_LOW(m,p1)               
#define BT_UI_APPL_TRACE_WARNING2(m,p1,p2)               DBGPRINTF_LOW(m,p1,p2)            
#define BT_UI_APPL_TRACE_WARNING3(m,p1,p2,p3)            DBGPRINTF_LOW(m,p1,p2,p3)         
#define BT_UI_APPL_TRACE_WARNING4(m,p1,p2,p3,p4)         DBGPRINTF_LOW(m,p1,p2,p3,p4)      
#define BT_UI_APPL_TRACE_WARNING5(m,p1,p2,p3,p4,p5)      DBGPRINTF_LOW(m,p1,p2,p3,p4,p5)   
#define BT_UI_APPL_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)   DBGPRINTF_LOW(m,p1,p2,p3,p4,p5,p6)
        
#define BT_UI_APPL_TRACE_API0(m)                         DBGPRINTF(m)                      
#define BT_UI_APPL_TRACE_API1(m,p1)                      DBGPRINTF(m,p1)                   
#define BT_UI_APPL_TRACE_API2(m,p1,p2)                   DBGPRINTF(m,p1,p2)                
#define BT_UI_APPL_TRACE_API3(m,p1,p2,p3)                DBGPRINTF(m,p1,p2,p3)             
#define BT_UI_APPL_TRACE_API4(m,p1,p2,p3,p4)             DBGPRINTF(m,p1,p2,p3,p4)          
#define BT_UI_APPL_TRACE_API5(m,p1,p2,p3,p4,p5)          DBGPRINTF(m,p1,p2,p3,p4,p5)       
#define BT_UI_APPL_TRACE_API6(m,p1,p2,p3,p4,p5,p6)       DBGPRINTF(m,p1,p2,p3,p4,p5,p6)    
        
#define BT_UI_APPL_TRACE_EVENT0(m)                       DBGPRINTF(m)                    
#define BT_UI_APPL_TRACE_EVENT1(m,p1)                    DBGPRINTF(m,p1)                 
#define BT_UI_APPL_TRACE_EVENT2(m,p1,p2)                 DBGPRINTF(m,p1,p2)              
#define BT_UI_APPL_TRACE_EVENT3(m,p1,p2,p3)              DBGPRINTF(m,p1,p2,p3)           
#define BT_UI_APPL_TRACE_EVENT4(m,p1,p2,p3,p4)           DBGPRINTF(m,p1,p2,p3,p4)        
#define BT_UI_APPL_TRACE_EVENT5(m,p1,p2,p3,p4,p5)        DBGPRINTF(m,p1,p2,p3,p4,p5)     
#define BT_UI_APPL_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)     DBGPRINTF(m,p1,p2,p3,p4,p5,p6)  
        
#define BT_UI_APPL_TRACE_DEBUG0(m)                       DBGPRINTF(m)                    
#define BT_UI_APPL_TRACE_DEBUG1(m,p1)                    DBGPRINTF(m,p1)                 
#define BT_UI_APPL_TRACE_DEBUG2(m,p1,p2)                 DBGPRINTF(m,p1,p2)              
#define BT_UI_APPL_TRACE_DEBUG3(m,p1,p2,p3)              DBGPRINTF(m,p1,p2,p3)           
#define BT_UI_APPL_TRACE_DEBUG4(m,p1,p2,p3,p4)           DBGPRINTF(m,p1,p2,p3,p4)        
#define BT_UI_APPL_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)        DBGPRINTF(m,p1,p2,p3,p4,p5)     
#define BT_UI_APPL_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6)     DBGPRINTF(m,p1,p2,p3,p4,p5,p6)      
        
#else   
        
/* definBT_UI_e traces for application */
#define BT_UI_APPL_TRACE_ERROR0(m)								
#define BT_UI_APPL_TRACE_ERROR1(m,p1)                    
#define BT_UI_APPL_TRACE_ERROR2(m,p1,p2)                 
#define BT_UI_APPL_TRACE_ERROR3(m,p1,p2,p3)              
#define BT_UI_APPL_TRACE_ERROR4(m,p1,p2,p3,p4)                                                                                             
#define BT_UI_APPL_TRACE_ERROR5(m,p1,p2,p3,p4,p5)                                                                                             
#define BT_UI_APPL_TRACE_ERROR6(m,p1,p2,p3,p4,p5,p6)             
        
#define BT_UI_APPL_TRACE_WARNING0(m)                     
#define BT_UI_APPL_TRACE_WARNING1(m,p1)                                                                                         
#define BT_UI_APPL_TRACE_WARNING2(m,p1,p2)                                                                                         
#define BT_UI_APPL_TRACE_WARNING3(m,p1,p2,p3)                                                                                         
#define BT_UI_APPL_TRACE_WARNING4(m,p1,p2,p3,p4)                                                                                         
#define BT_UI_APPL_TRACE_WARNING5(m,p1,p2,p3,p4,p5)                                                                                         
#define BT_UI_APPL_TRACE_WARNING6(m,p1,p2,p3,p4,p5,p6)   
        
#define BT_UI_APPL_TRACE_API0(m)                         
#define BT_UI_APPL_TRACE_API1(m,p1)                                                                                
#define BT_UI_APPL_TRACE_API2(m,p1,p2)                                                                                
#define BT_UI_APPL_TRACE_API3(m,p1,p2,p3)                                                                                             
#define BT_UI_APPL_TRACE_API4(m,p1,p2,p3,p4)                                                                                          
#define BT_UI_APPL_TRACE_API5(m,p1,p2,p3,p4,p5)                                                                                       
#define BT_UI_APPL_TRACE_API6(m,p1,p2,p3,p4,p5,p6)       
        
#define BT_UI_APPL_TRACE_EVENT0(m)                       
#define BT_UI_APPL_TRACE_EVENT1(m,p1)                                                                                                                
#define BT_UI_APPL_TRACE_EVENT2(m,p1,p2)                                                                                                             
#define BT_UI_APPL_TRACE_EVENT3(m,p1,p2,p3)                                                                                                          
#define BT_UI_APPL_TRACE_EVENT4(m,p1,p2,p3,p4)                                                                                                       
#define BT_UI_APPL_TRACE_EVENT5(m,p1,p2,p3,p4,p5)                                                                                                    
#define BT_UI_APPL_TRACE_EVENT6(m,p1,p2,p3,p4,p5,p6)     
        
#define BT_UI_APPL_TRACE_DEBUG0(m)                       
#define BT_UI_APPL_TRACE_DEBUG1(m,p1)                                                                                                                
#define BT_UI_APPL_TRACE_DEBUG2(m,p1,p2)                                                                                                             
#define BT_UI_APPL_TRACE_DEBUG3(m,p1,p2,p3)                                                                                                          
#define BT_UI_APPL_TRACE_DEBUG4(m,p1,p2,p3,p4)                                                                                                       
#define BT_UI_APPL_TRACE_DEBUG5(m,p1,p2,p3,p4,p5)                                                                                                    
#define BT_UI_APPL_TRACE_DEBUG6(m,p1,p2,p3,p4,p5,p6) 

#endif	// _BTUI_USE_TRACE_



#endif	// _BTUI_TRACE_H_