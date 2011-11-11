// SMS7Bit.cpp: implementation of the CSMS7Bit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sunnycat.h"
#include "SMS7Bit.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static T7BitASCII Item[] =
{
	{ 0,0 , 64   },{ 0,1 , 163  },{ 0,2 , 36   },{ 0,3 , 165  },       
	{ 0,4 , 232  },{ 0,5 , 233  },{ 0,6 , 249  }, { 0,7 , 236  },       
	{ 0,8 , 242  },{ 0, 9, 199 },{ 0,10 , 10   },{ 0,11 , 216  },     
	{ 0,12 , 248  },{ 0,13 , 13   },{ 0,14 , 197  },{ 0,15 , 229  } ,    
	{ 0,16 , 0x394    },{ 0,17 , 95   },{ 0,18 , 0x3a6    },{ 0,19 , 0x393   },     
	{ 0,20 , 0x39b   },{ 0,21 , 0x3a9    },{ 0,22 , 0x3a0    },{ 0,23 , 0x3a8   },     
	{ 0,24 , 0x3a3    },{ 0,25 , 0x398    },{ 0,26 , 0x39e    },{ 0,27 , 0    },     
	{27,10 , 12   },{27,20 , 94   },{27,40 , 123   },{27,41 , 125   },   
	{27,47 , 92   },{27,60 , 91   },{27,61 , 126   },{27,62 , 93   },    
	{27,64 , 124   },{27,101 , 0x20ac   },{ 0,28 , 198  },{ 0,29 , 230  },     
	{ 0,30 , 223  },{ 0,31 , 201  },{ 0,32 , 32   },{ 0,33 , 33   },
	{ 0,34 , 34   },{ 0,35 , 35   },{ 0,36 , 164  },{ 0,37 , 37   },
	{ 0,38 , 38   },{ 0,39 , 39   },{ 0,40 , 40   },{ 0,41 , 41   },
	{ 0,42 , 42   },{ 0,43 , 43   },{ 0,44 , 44   },{ 0,45 , 45   },
	{ 0,46 , 46   },{ 0,47 , 47   },{ 0,48 , 48   },{ 0,49 , 49   },
	{ 0,50 , 50   },{ 0,51 , 51   },{ 0,52 , 52   },{ 0,53 , 53   },
	{ 0,54 , 54   },{ 0,55 , 55   },{ 0,56 , 56   },{ 0,57 , 57   },
	{ 0,58 , 58   },{ 0,59 , 59   },{ 0,60 , 60   },{ 0,61 , 61   },
	{ 0,62 , 62   } ,{ 0,63 , 63   } ,{ 0,64 , 161  },{ 0,65 , 65   },
	{ 0,66 , 66   },{ 0,67 , 67   },{ 0,68 , 68   },{ 0,69 , 69   },
	{ 0,70 , 70   },{ 0,71 , 71   },{ 0,72 , 72   },{ 0,73 , 73   },
	{ 0,74 , 74   },{ 0,75 , 75   },{ 0,76 , 76   },{ 0,77 , 77   },
	{ 0,78 , 78   },{ 0,79 , 79   },{ 0,80 , 80   },{ 0,81 , 81   },
	{ 0,82 , 82   },{ 0,83 , 83   },{ 0,84 , 84   },{ 0,85 , 85   },
	{ 0,86 , 86   },{ 0,87 , 87   },{ 0,88 , 88   },{ 0,89 , 89   },
	{ 0,90 , 90   },{ 0,91 , 196  },{ 0,92 , 214  },{ 0,93 , 209  },
	{ 0,94 , 220  },{ 0,95 , 167  },{ 0,96 , 191  },{ 0,97 , 97   },
	{ 0,98 , 98   },{ 0,99 , 99   },{ 0,100 , 100   },{ 0,101 , 101   },
	{ 0,102 , 102   },{ 0,103 , 103   },{ 0,104 , 104   },{ 0,105 , 105   },
	{ 0,106 , 106   },{ 0,107 , 107   },{ 0,108 , 108   },{ 0,109 , 109   },
	{ 0,110 , 110   } ,{ 0,111 , 111   },{ 0,112 , 112   },{ 0,113 , 113   },
	{ 0,114 , 114   },{ 0,115 , 115   },{ 0,116 , 116   },{ 0,117 , 117   },
	{ 0,118 , 118   },{ 0,119 , 119   },{ 0,120 , 120   },{ 0,121 , 121   },
	{ 0,122 , 122   } ,{ 0,123 , 228   } ,{ 0,124,124   },{ 0,125,125   },
	{ 0,126,126   }  
};

CSMS7Bit::CSMS7Bit()
{
	SetInitValue();
}

CSMS7Bit::~CSMS7Bit()
{
	ClearValue();
}

void CSMS7Bit::SetInitValue()
{
	size_t nsize = 	sizeof(Item)/sizeof(Item[0]);
	for(size_t i=0;i<nsize;i++)
	{
		T7BitASCII *pItem = new T7BitASCII();
		pItem->b7Bit1 = Item[i].b7Bit1;
		pItem->b7Bit2 = Item[i].b7Bit2;
		pItem->bASCII = Item[i].bASCII;

		m_7Bit.push_back(pItem);
	}	
}

void CSMS7Bit::ClearValue()
{
	for(size_t i=0;i<m_7Bit.size();i++)
	{
		delete m_7Bit[i];
	}
	m_7Bit.clear();
}

class ASCIIMatch
{
private:
	BYTE m_bASCII;
public:
	ASCIIMatch(BYTE bASCII)
		:m_bASCII(bASCII)
	{

	}
	bool operator()(const T7BitASCII* pItem) const
	{
		return (pItem->bASCII == m_bASCII);
	}
};

class BYTE2Match
{
private:
	BYTE m_b1;
	BYTE m_b2;
public:
	BYTE2Match(BYTE b1,BYTE b2)
		:m_b1(b1),m_b2(b2)
	{

	}
	bool operator()(const T7BitASCII* pItem) const
	{
		return ((pItem->b7Bit1 == m_b1)&&(pItem->b7Bit2 == m_b2));
	}
};

bool CSMS7Bit::Encode(BYTE& b1,BYTE& b2, BYTE bASCII)
{
	std::vector<T7BitASCII*>::iterator result;
	result = std::find_if(m_7Bit.begin(),m_7Bit.end(),ASCIIMatch(bASCII));
	if(result != m_7Bit.end())
	{
		b1 = (*result)->b7Bit1;
		b2 = (*result)->b7Bit2;

		return true;
	}

	return false;
}

bool CSMS7Bit::Decode(BYTE b1,BYTE b2,TCHAR& bASCII)
{
	std::vector<T7BitASCII*>::iterator result;
	result = std::find_if(m_7Bit.begin(),m_7Bit.end(),BYTE2Match(b1,b2));
	if(result != m_7Bit.end())
	{
		bASCII = (*result)->bASCII;
		return true;
	}

	return false;
}