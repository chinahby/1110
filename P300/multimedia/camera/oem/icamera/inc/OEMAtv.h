#ifndef __OEMATV_H__
#define __OEMATV_H__
#include "AEE.h"
#include "AEEComdef.h"
#include "AEE_OEMComdef.h"
#include "AEEInterface.h"
#include "AEEIBitmap.h"

#include "OEMCamera.h"
#include "AEECamera.h"


#include "tlgPlatform.h"

typedef  unsigned short int ATV_U16;
typedef  unsigned char ATV_U8;

/*include by tlgPlatform.h*/
#if 0
typedef	enum {
	TLG_REGION_USA,				// 美国
	TLG_REGION_CANADA,			// 加拿大
	TLG_REGION_KOREA,				// 韩国
	TLG_REGION_TAIWAN,			// 台湾
	TLG_REGION_MEXICO,			// 墨西哥
	TLG_REGION_CHILE,				// 智利
	TLG_REGION_VENEZUELA,			// 委内瑞拉
	TLG_REGION_PHILIPPINES,		// 菲律宾
	TLG_REGION_JAMAICA,			// 牙买加
	TLG_REGION_CHINA,				// 中国大陆
	TLG_REGION_SHENZHEN,			// 深圳
	TLG_REGION_VIETNAM,			// 越南
	TLG_REGION_WESTERNEUROPE,	// 西欧
	TLG_REGION_TURKEY,			// 土耳其
	TLG_REGION_UAE,				// 阿联酋
	TLG_REGION_AFGHANISTAN,		// 阿富汗
	TLG_REGION_SINGAPORE,			// 新加坡
	TLG_REGION_THAILAND,			// 泰国
	TLG_REGION_CAMBODIA,			// 柬埔寨
	TLG_REGION_INDONESIA,			// 印度尼西亚
	TLG_REGION_MALAYSIA,			// 马来西亚
	TLG_REGION_LAOS,				// 老挝
	TLG_REGION_PORTUGAL,			// 葡萄牙
	TLG_REGION_SWEDEN,			// 瑞典
	TLG_REGION_PAKISTAN,			// 巴基斯坦
	TLG_REGION_SPAIN,				// 西班牙
	TLG_REGION_JAPAN,				// 日本
	TLG_REGION_UK,					// 英国
	TLG_REGION_HONGKONG,			// 香港
	TLG_REGION_SOUTHAFRICA,		// 南非
	TLG_REGION_BRAZIL,				// 巴西
	TLG_REGION_ARGENTINA,		// 阿根廷
	TLG_REGION_INDIA,				// 印度
	TLG_REGION_BURMA,				// 缅甸
	TLG_REGION_SUDAN,				// 苏丹
	TLG_REGION_YEMEN,				// 也门
	TLG_REGION_PERU,				// 秘鲁
	TLG_REGION_BOLIVIA,			// 玻利维亚
	TLG_REGION_ECUADOR,			// 厄瓜多尔
	TLG_REGION_SURINAME, 			// 苏里南
	TLG_REGION_AUSTRALIA,			// 澳大利亚
	TLG_REGION_NEWZEALAND,		// 新西兰
#ifdef    TLG_1120
	TLG_REGION_NORTHKOREA, 	/*北朝鲜*/	
	TLG_REGION_MONGOLIA ,	/*	蒙古*/		
	TLG_REGION_AZERBAIJAN ,	/*阿塞拜疆*/	
	TLG_REGION_CZETH, 		/*捷克*/		
	TLG_REGION_POLAND, 		/*波兰*/		
	TLG_REGION_RUSSION,		/*俄罗斯*/	
	TLG_REGION_UKRAINE, 		/*乌克兰*/	
	TLG_REGIONI_TUNISIA, 	/*	突尼斯 */	
	TLG_REGION_EGYPT, 		/*埃及*/		
	TLG_REGION_MOROCCO, 		/*摩洛哥*/	
	TLG_REGION_IRAQ,		/*伊拉克*/	
	TLG_REGION_SAUDIARABIA,	/*沙特*/		
	TLG_REGION_GREECE,		/*希腊 */		
#endif
	TLG_REGION_TOTAL
} TLG_REGION_CODE;


typedef	enum {
	TLG_TV_525_LINES,
	TLG_TV_625_LINES
} TLG_TV_SCANLINES;

typedef	enum {
	TLG_SCRMD_WINDOW,
	TLG_SCRMD_FULLSCREEN
} TLG_SCREEN_MODE;

typedef enum {
	TLG_TUNEMD_AUTO,
	TLG_TUNEMD_MANUAL
} TLG_TUNE_MODE;


typedef struct {
	uint8 mBrightness;				// 亮度
	uint8 mContrast;					// 对比度
	uint8 mSaturation;				// 色饱和度
	uint8 mDefinition;				// 清晰度
	uint8 mVolume;					// 音量

	uint8 mHOffset,					// 水平偏移调节
	 	 mHReduce,					// 水平大小调节
		 mVOffset,					// 垂直偏移调节
		 mVReduce;					// 垂直大小调节
} TLG_TV_OPTIONS;

typedef struct {
	TLG_TUNE_MODE mTuneMode;		// 当前调谐模式
	TLG_SCREEN_MODE mScrMode;		// 当前屏幕模式
	TLG_REGION_CODE mRegion;		// 当前收视地区代码

	uint8 mIdxListCnt;				// 频道列表中有效索引个数
	char mCurListIdx;				// 当前频道列表的索引
	uint8 mCurChnIdx;				// 当前收视频道的索引
	struct {
		uint8 ChnIdx;					// 频道索引
		char ChnName[7*2+1];		// 频道名称
	} mIdxListBuf[TLG_MAX_PROGRAM_NUM];				// 频道索引列表
} TLG_TV_STORAGE;

typedef struct {
	uint8 mVolume;					// 音量
} TLG_FM_OPTIONS;

typedef struct {
	TLG_TUNE_MODE mTuneMode;		// 当前调谐模式

	uint8 mIdxListCnt;				// 电台列表中有效索引个数
	uint8 mCurListIdx;				// 当前电台列表的索引
#if 0 //FM_CUTOFF_AT_76MHZ
	uint16
#else /* 0 //FM_CUTOFF_AT_76MHZ */
	uint8
#endif /* 0 //FM_CUTOFF_AT_76MHZ */
		mCurChnIdx;				// 当前收听电台的索引
	struct {
#if 0 //FM_CUTOFF_AT_76MHZ
	uint16
#else /* 0 //FM_CUTOFF_AT_76MHZ */
	uint8
#endif /* 0 //FM_CUTOFF_AT_76MHZ */
				ChnIdx;				// 电台索引
	} mIdxListBuf[32];				// 电台索引列表
} TLG_FM_STORAGE;
#endif



/******************************************************************************/



typedef enum
{
    HANDLE_FOR_TV_MAIN,
    HANDLE_FOR_TV_SETCHN
}ATV_NOTIFY_HANDLE_ID_e;


typedef enum
{
    ATV_SET_CHN_SUCCESS,
    ATV_SET_CHN_FAILED
}SET_CHN_STATUS_e;

typedef struct TVSETCHNNOTIFY
{
    SET_CHN_STATUS_e status;
    int chn;          //返回的频道号，如果是0表示失败
}ATV_SetChn_Notify_t;

typedef struct ITlgAtv ITlgAtv;

typedef void (*PFNATVNOTIFY)(void * pUser, void * pNotify);

#define tv_ver2
#ifndef tv_ver2 
#define INHERIT_ITlgAtv(iname)   \
        INHERIT_IQueryInterface(iname);   \
        int (*Start)(OEMINSTANCE p, int16 nMode, uint32 dwParam);  \
        int (*Stop)(OEMINSTANCE p);   \
        int (*GetFrame)(OEMINSTANCE p, IBitmap ** ppFrame);   \
        int (*RegisterNotify)(OEMINSTANCE h, ATV_NOTIFY_HANDLE_ID_e Id,PFNATVNOTIFY pfnNotify, void * pUser);  \
        int (*SetChn)(OEMINSTANCE p,ATV_U16 chn, ATV_SET_CHN_TYPE_e type);      \
        int (*SetParam)(OEMINSTANCE h,ATV_SET_PARAM_e type, int hparam, int lparam);  \
        int (*GetParam)(OEMINSTANCE h,ATV_GET_PARAM_e type, void * hparam, void * lparam);  \
        int (*Update)(OEMINSTANCE h, uint32 dwParam);
#else 
#define INHERIT_ITlgAtv(iname)   \
        INHERIT_IQueryInterface(iname);   \
        int (*Create)(OEMINSTANCE p);   \
        int (*Start)(OEMINSTANCE p, int16 nMode, uint32 dwParam);  \
        int (*Stop)(OEMINSTANCE p);   \
        int (*GetFrame)(OEMINSTANCE p, IBitmap ** ppFrame);   \
        int (*RegisterNotify)(OEMINSTANCE h, ATV_NOTIFY_HANDLE_ID_e Id,PFNATVNOTIFY pfnNotify, void * pUser);  \
        int (*SetChn)(OEMINSTANCE p,ATV_U16 chn, ATV_SET_CHN_TYPE_e type);      \
        int (*SetParam)(OEMINSTANCE h,ATV_SET_PARAM_e type, int hparam, int lparam);  \
        int (*GetParam)(OEMINSTANCE h,ATV_GET_PARAM_e type, void * hparam, void * lparam);  \
        int (*Update)(OEMINSTANCE h, uint32 dwParam);
#endif  
// int (*Update)(OEMINSTANCE h, uint32 dwParam);
	// int (*Update)(OEMINSTANCE h, uint32 dwParam);

AEEINTERFACE(ITlgAtv)
{
    INHERIT_ITlgAtv(ITlgAtv)
};


struct ITlgAtv
{
  const AEEVTBL(ITlgAtv) *pvt;
  OEMINSTANCE pCamera;
  OEMINSTANCE pBitmap;
  PFNATVNOTIFY   pfnNotify;
  void *            pUser;
  int nRefs;
 // boolean bTvStart;
};

struct TlgAtv_ScanInfo
{    
    OEMINSTANCE pAtv;
    int chn;  
};

/******************************************************************************/
 int OEMTLGAtv_New(IShell *pIShell,AEECLSID ClsID,void** ppInterface);
#ifdef tv_ver2
static int OEMTLGAtv_Create(OEMINSTANCE h);
#endif
static int OEMTLGAtv_Delete(void *h);
static int OEMTLGAtv_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam);
static int OEMTLGAtv_Stop(OEMINSTANCE h);
static int  OEMTLGAtv_GetFrame(OEMINSTANCE h, IBitmap ** ppFrame);
static int OEMTLGAtv_RegisterNotify(OEMINSTANCE h, ATV_NOTIFY_HANDLE_ID_e Id,PFNATVNOTIFY pfnNotify, void * pUser);
static uint32 OEMTLGAtv_AddRef(ITlgAtv* pIAtv);
static uint32 OEMTLGAtv_Release(ITlgAtv* pIAtv);
static int OEMTLGAtv_QueryInterface(ITlgAtv* pIAtv, AEECLSID ClsID, void** ppInterface);
static int OEMTLGAtv_SetChn(OEMINSTANCE h,ATV_U16 chn,ATV_SET_CHN_TYPE_e type);

static int OEMTLGAtv_SetParam(OEMINSTANCE h,ATV_SET_PARAM_e type, int hparam, int lparam);
static int OEMTLGAtv_GetParam(OEMINSTANCE h,ATV_GET_PARAM_e type, void * hparam, void * lparam);
static int OEMTLGAtv_Update(OEMINSTANCE h, uint32 dwParam);








/******************************************************************************/
typedef struct ITlgFm ITlgFm;

#define INHERIT_ITlgFm(iname)   \
        INHERIT_IQueryInterface(iname);   \
        int (*Start)(OEMINSTANCE p, int16 nMode, uint32 dwParam);  \
        int (*Stop)(OEMINSTANCE p);   \
        int (*AutoScanFm)(TLG_FM_STORAGE *fm_storage);      \
        int (*SetFmChn)(ATV_U16 chn);       


AEEINTERFACE(ITlgFm)
{
    INHERIT_ITlgFm(ITlgFm)
};


struct ITlgFm
{
  const AEEVTBL(ITlgFm) *pvt;
  int nRefs;
};



extern int OEMTLGFm_New(IShell *pIShell,AEECLSID ClsID,void** ppInterface);
extern int OEMTLGFm_Delete(void *h);
extern int OEMTLGFm_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam);
extern int OEMTLGFm_Stop(OEMINSTANCE h);
extern uint32 OEMTLGFm_AddRef(ITlgFm* pfm);
extern uint32 OEMTLGFm_Release(ITlgFm* pfm);
extern int OEMTLGFm_QueryInterface(ITlgFm* pfm, AEECLSID ClsID, void** ppInterface);

extern int OEMTLGFm_AutoScan(TLG_FM_STORAGE *fm_storage);
extern int OEMTLGFm_SetChn(ATV_U16 chn);


#endif


