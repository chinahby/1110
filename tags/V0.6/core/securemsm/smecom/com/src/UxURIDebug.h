
#ifndef __GENERIC_H__
#define __GENERIC_H__

#ifdef DEBUG
#error code not present
#endif

#ifdef UXURI_DEBUG
#define DEBUG_PRINT printf("[%s][%d]\n",__FUNCTION__,__LINE__)
#define DEBUG_PRINT_START printf("[%s] - Start\n",__FUNCTION__)
#define DEBUG_PRINT_END printf("[%s] - End\n",__FUNCTION__)

#define PRINT_ERROR(a) printf("\n##Error : [%s][%d] ",__FUNCTION__,__LINE__);printf(a)
#define PRINT_WARN(a) printf("\n#Warning : [%s][%d] ",__FUNCTION__,__LINE__);printf(a)
#define PRINT_INFO(a) printf("\nInfo : [%s][%d] ",__FUNCTION__,__LINE__);printf(a)
#else
#define DEBUG_PRINT ;
#define DEBUG_PRINT_START ;
#define DEBUG_PRINT_END ;

#define PRINT_ERROR(a) ;
#define PRINT_WARN(a) ;
#define PRINT_INFO(a) ;
#endif

#endif//__GENERIC_H__
