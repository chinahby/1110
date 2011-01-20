// doxparse.h : Parser object
#ifndef DOXPARSE_H
#define DOXPARSE_H


#include "doxhtml.h"

//
//-- Parser functions
//

#define MAXSTACKSIZE   40

typedef struct DoxParse DoxParse;

typedef int8 DoxTag;

struct DoxParse {
   MLSt    st;
   MLSt    stData;       // default state for handling contents of this element (CDATA, RCDATA, PCDATA, NODATA)
   MLSt    stEnt;        // state to return to after processing entity
   DoxTag  nTag;         // elem ID of tag being parsed
   uint16  chEnt;
	char    cQuote;
   char *  pchOut;
   char *  pchOutMax;
   char *  pchElem;
   char *  pchEnt;
   char *  pchAttr;
   char *  pchTerm;
   DoxTag  anStack[MAXSTACKSIZE];    // stack of tag IDs
   int     cntStack;                 // number of elements in the stack
   Dox *   pdox;
};



//void DoxParse_Ctor(DoxParse *me, char *pchBuf, int cbBuf);
void DoxParse_Close(DoxParse *me);
void DoxParse_Open(DoxParse *me, int bSlashEnd);
void DoxParse_Write(DoxParse *me, const char *pc, int cb);
void DoxParse_Terminate(DoxParse *me);
void DoxParse_Dtor(DoxParse *me);
void DoxParse_Ctor(DoxParse *me, Dox *pdox);
void DoxParse_Resize(DoxParse *me, Dox *pdox);


void DumpControl(const char *pszHTML);


#endif  //DOXPARSE_H
