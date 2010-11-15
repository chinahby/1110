#ifndef AEETV_H
#define AEETV_H
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEE.h"
#include "AEEMedia.h"
#include "AEEBitmap.h"
#include "AEE_OEMComdef.h"
#include "AEEIBitmap.h"
#define INHERIT_FROM_CAMERA
#ifdef INHERIT_FROM_CAMERA
#include "AEECameraEx.h"
#endif

typedef  unsigned short int ATV_U16;
typedef  unsigned char ATV_U8;

//#define TLG_TEST_SENSITIVE               //测试电视的灵敏度，提交测试时使用
#define TLG_MAX_PROGRAM_NUM 32

//#define TLG_CMD_SET_CHN  (CAM_CMD_BASE-1)             //给UI的回调添加cmd
//#define TLG_STATUS_SET_CHN_DONE  (CAM_STATUS_BASE-1)
//#define TLG_STATUS_SET_CHN_FAIL  (CAM_STATUS_BASE-2)

typedef	enum {
    TLG_REGION_START = -1,
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
	TLG_TV_625_LINES,
	TLG_TV_NONE
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

	ATV_U16 mHOffset,					// 水平偏移调节
	 	 mHReduce,					// 水平大小调节
		 mVOffset,					// 垂直偏移调节
		 mVReduce;					// 垂直大小调节
} TLG_TV_OPTIONS;


typedef struct {
	uint8 mVolume;					// 音量
} TLG_FM_OPTIONS;

typedef struct {
	TLG_TUNE_MODE mTuneMode;		// 当前调谐模式

	uint8 mIdxListCnt;				// 电台列表中有效索引个数
	uint8 mCurListIdx;				// 当前电台列表的索引
#if 0//FM_CUTOFF_AT_76MHZ
	uint16
#else /* FM_CUTOFF_AT_76MHZ */
	uint8
#endif /* FM_CUTOFF_AT_76MHZ */
		mCurChnIdx;				// 当前收听电台的索引
	struct {
#if 0//FM_CUTOFF_AT_76MHZ
	uint16
#else /* FM_CUTOFF_AT_76MHZ */
	uint8
#endif /* FM_CUTOFF_AT_76MHZ */
				ChnIdx;				// 电台索引
	} mIdxListBuf[32];				// 电台索引列表
} TLG_FM_STORAGE;


typedef enum
{
    TLG_SET_REGION,
    TLG_SET_VOLUME,
    TLG_SET_DEFINITION,      /*设置清晰度0~63*/    
    TLG_SET_BRIGHT,         /*设置亮度0~15*/
    TLG_SET_CONTRAST,       /*设置对比度0~15*/
    TLG_SET_SATURATION,     /*设置色彩饱和度0~255*/
    TLG_SET_DISPLAY_SIZE,   /*设置画面偏移先预留*/     
    TLG_SET_DISPLAY_OFFSET,    /*设置画幅偏移先预留*/
    CAM_SET_DISPLAY_SIZE,
    CAM_SET_SIZE,
}ATV_SET_PARAM_e;



typedef enum
{    
    TLG_GET_CHN_COUNT,
#ifdef TLG_TEST_SENSITIVE
    TLG_GET_TFCODE,
    TLG_GET_TTLGAIN,
    TLG_GET_CHROMLOCK
#endif
}ATV_GET_PARAM_e;


typedef enum
{
    ATV_AUTO_SCAN,               /*自动搜索模式*/
    ATV_FAST_SET,                /*快速设台*/
    ATV_SET_AV_STOP              /*设台时音视频静止*/   
}ATV_SET_CHN_TYPE_e;

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
    int chn;          //返回的物理频道号，如果是0表示失败
}ATV_SetChn_Notify_t;


typedef struct ITlgAtv ITlgAtv;

typedef void (*PFNATVNOTIFY)(void * pUser, void * pNotify);

#define tv_ver2
#ifndef tv_ver2 
#define INHERIT_ITlgAtv(iname)   \
        INHERIT_IQueryInterface(iname);   \
        int (*Start)(OEMINSTANCE p, int16 nMode, uint32 dwParam);  \
        int (*Stop)(OEMINSTANCE p);   \
        int (*GetFrame)(OEMINSTANCE h, IBitmap ** ppFrame);   \
        int (*RegisterNotify)(OEMINSTANCE h, ATV_NOTIFY_HANDLE_ID_e Id,PFNATVNOTIFY pfnNotify, void * pUser);  \
        int (*SetChn)(OEMINSTANCE p,ATV_U16 chn, ATV_SET_CHN_TYPE_e type);       \
        int (*SetParam)(OEMINSTANCE h,ATV_SET_PARAM_e type, int hparam, int lparam);  \
        int (*GetParam)(OEMINSTANCE h,ATV_GET_PARAM_e type, void * hparam, void * lparam); \
        int (*Update)(OEMINSTANCE h, uint32 dwParam)
#else
#define INHERIT_ITlgAtv(iname)   \
        INHERIT_IQueryInterface(iname);   \
        int (*Create)(OEMINSTANCE p);   \
        int (*Start)(OEMINSTANCE p, int16 nMode, uint32 dwParam);  \
        int (*Stop)(OEMINSTANCE p);   \
        int (*GetFrame)(OEMINSTANCE h, IBitmap ** ppFrame);   \
        int (*RegisterNotify)(OEMINSTANCE h, ATV_NOTIFY_HANDLE_ID_e Id,PFNATVNOTIFY pfnNotify, void * pUser);  \
        int (*SetChn)(OEMINSTANCE p,ATV_U16 chn, ATV_SET_CHN_TYPE_e type);       \
        int (*SetParam)(OEMINSTANCE h,ATV_SET_PARAM_e type, int hparam, int lparam);  \
        int (*GetParam)(OEMINSTANCE h,ATV_GET_PARAM_e type, void * hparam, void * lparam); \
        int (*Update)(OEMINSTANCE h, uint32 dwParam)
#endif 
AEEINTERFACE(ITlgAtv)
{
    INHERIT_ITlgAtv(ITlgAtv);
};


struct ITlgAtv
{
  const AEEVTBL(ITlgAtv) *pvt;
  OEMINSTANCE *pCamera;
  OEMINSTANCE pBitmap;
  PFNATVNOTIFY   pfnNotify;
  void *            pUser;
  int nRefs;
};


#ifdef tv_ver2
#define  ITV_Create(p)                p->pvt->Create(p)
#endif
/*这俩个函数同时使用camera和tv的接口*/
#define ITV_start(p,m,dw)                   p->pvt->Start(p,m,dw)
#define ITV_StartPreview(p)                 ITV_start(p,CAM_MODE_PREVIEW,0)
#define ITV_StopPreview(p)                 ITV_Stop(p)

#define ITV_updateimg(p,dw)                 p->pvt->Update(p,dw)


#define ITV_Stop(p)                         p->pvt->Stop(p)
/*这里看情况，先使用了tv的接口*/
#define ITV_AddRef(p)                       p->pvt->AddRef(p)
#define ITV_Release(p)                      p->pvt->Release(p)
#define ITV_QueryInterface(p, i, p2)        p->pvt->QueryInterface(p, i, p2)

#define ITV_RegisterNotify(p, pfn, pu)              p->pvt->RegisterNotify(p,HANDLE_FOR_TV_MAIN, (PFNATVNOTIFY)pfn, pu)
#define ITV_RegisterSetChnNotify(p, pfn, pu)        p->pvt->RegisterNotify(p,HANDLE_FOR_TV_SETCHN,(PFNATVNOTIFY)pfn, pu)

//#define ITV_RegisterNotify(p, pfn, pu)      AEE_GET_ATV_PVTBL(p)->RegisterNotify(p, pfn, pu)

/*这里是电视特定的接口*/
#define ITV_SetParam(p, c, p1, p2)           p->pvt->SetParam(p,c, p1, p2)
#define ITV_GetParam(p, c, p1, p2)           p->pvt->GetParam(p,c, p1, p2)

#define ITV_SetRegion(p, v)                 ITV_SetParam(p,TLG_SET_REGION,(TLG_REGION_CODE)v,0)               /*接口内做区域检查，如果不对返回0*/
#define ITV_SetVolume(p, v)                 ITV_SetParam(p,TLG_SET_VOLUME,(TLG_REGION_CODE)v,0)
#define ITV_SetContrast(p, v)               ITV_SetParam(p, TLG_SET_CONTRAST,(int32)(v),0)       /*设置对比度0~15*/
#define ITV_SetBrightness(p, v)             ITV_SetParam(p, TLG_SET_BRIGHT,(int32)(v),0)           /*设置亮度0~15*/
#define ITV_SetDefinition(p, v)             ITV_SetParam(p, TLG_SET_DEFINITION,(int32)(v),0)       /*设置清晰度0~63*/
#define ITV_SetSaturation(p, v)             ITV_SetParam(p, TLG_SET_SATURATION,(int32)(v),0)       /*设置色彩饱和度0~255*/
#define ITV_SetDisplay_Size(p, v,u)         ITV_SetParam(p, TLG_SET_DISPLAY_SIZE, (int32)(v),(u))  /*设置画面偏移先预留*/ 
#define ITV_SetDisplay_Offset(p, v,u)       ITV_SetParam(p, TLG_SET_DISPLAY_OFFSET, (int32)(v),(u))  /*设置画幅偏移先预留*/

#define ITV_SetDisplaySize(p,v)             ITV_SetParam(p, CAM_SET_DISPLAY_SIZE, (int32)(v),0)
#define ITV_SetSize(p,v)                    ITV_SetParam(p, CAM_SET_SIZE, (int32)(v),0)
#define ITV_GetChnCount(p,v)                ITV_GetParam(p,TLG_GET_CHN_COUNT,(void *)(v),(void *)NULL)        
#define ITV_GetFrame(p,v)                   p->pvt->GetFrame(p,v)
#define ITV_SetChn(p,v,y)                   p->pvt->SetChn(p,v,y)

#ifdef TLG_TEST_SENSITIVE
#define ITV_GetTF(p,v)                       ITV_GetParam(p,TLG_GET_TFCODE,(void *)(v),(void *)NULL)
#define ITV_GetTG(p,v)                       ITV_GetParam(p,TLG_GET_TTLGAIN,(void *)(v),(void *)NULL)  
#define ITV_GetChrLock(p,v)                  ITV_GetParam(p,TLG_GET_CHROMLOCK,(void *)(v),(void *)NULL)  
#endif

typedef struct ITlgFm ITlgFm;

#define INHERIT_ITlgFm(iname)   \
        INHERIT_IQueryInterface(iname);   \
        int (*Start)(OEMINSTANCE p, int16 nMode, uint32 dwParam);  \
        int (*Stop)(OEMINSTANCE p);   \
        int (*AutoScanFm)(TLG_FM_STORAGE *fm_storage);      \
        int (*SetFmChn)(ATV_U16 chn)


AEEINTERFACE(ITlgFm)
{
    INHERIT_ITlgFm(ITlgFm);
};


struct ITlgFm
{
  const AEEVTBL(ITlgFm) *pvt;
  int nRefs;
};

#define IFM_Start(p,m,dw)                AEEGETPVTBL(p)->pvt->Start(p,m,dw)
#define IFM_Stop(p)                      AEEGETPVTBL(p)->pvt->Stop(p)
#define IFM_SetChn(p,v)                  AEEGETPVTBL(p)->pvt->SetFmChn(v)
#define IFM_AutoScan(p,v)                AEEGETPVTBL(p)->pvt->AutoScanFm(v)
#endif

