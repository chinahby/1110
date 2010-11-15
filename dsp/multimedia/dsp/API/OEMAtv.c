#include "AEE_OEMComdef.h"
#include "AEEShell.h"
#include "OEMCameraEx.h"
#include "AEECameraEx.h"
//#include "OEMCameraDMSS.h"
#include "AEECamera.bid"
#include "OEMAtv.h"
#include "hs_mb6550.h"
#include "snddev.h"
#include "snd.h"
#include "OEMClassIDs.h"
#ifdef USE_ANALOGTV_TLG1120



#define OEMATV_DEBUG_SWITCH
#ifdef OEMATV_DEBUG_SWITCH
#define OEMATV_PRINT(fmt,x,y,z) MSG_FATAL(fmt,x,y,z)
#else
#define OEMATV_PRINT(fmt,x,y,z) 
#endif

//#include "time_secure.h"

static struct TlgAtv_ScanInfo g_OEMAtv;
static uint16 Tv_fbuffer[320*240] = {0};


static const AEEVTBL(ITlgAtv) gMMITlgAtvfuncs=
{
    OEMTLGAtv_AddRef,
    OEMTLGAtv_Release,
    OEMTLGAtv_QueryInterface,
#ifdef tv_ver2     
    OEMTLGAtv_Create,
#endif
    OEMTLGAtv_Start,
    OEMTLGAtv_Stop,    
    OEMTLGAtv_GetFrame,
    OEMTLGAtv_RegisterNotify,
    OEMTLGAtv_SetChn,
    OEMTLGAtv_SetParam,
    OEMTLGAtv_GetParam,
    OEMTLGAtv_Update,
};
//

void OEMTLGAtv_Init(void)
{
   // Empty now
}


int OEMTLGAtv_New(IShell *pIShell,AEECLSID ClsID,OEMINSTANCE* ppInterface)
{
    int ret = 1;
    ITlgAtv *pMe;
   // IDIB *pDib;
#ifndef tv_ver2   
    uint16 pBitmapSize = 0;
#endif
//    unsigned long  time;
       
 //   time_secure_get_local_time_ms(&time);
 //   OEMATV_PRINT("OEMTLGAtv_New Enter time=%u\n",time, 0, 0);
  MSG_FATAL("---------------------------------------------OEMTLGAtv_New--return",0,0,0);
   // rex_sleep(100);
    OEMATV_PRINT("OEMTLGAtv_New Enter",0,0,0);
    if (pIShell==NULL||ClsID!=AEECLSID_TLG_ATV||ppInterface==NULL)
    {
        OEMATV_PRINT("OEMAtv_New params ClsID = %d,failed",0,0,0);
        return EBADPARM;
    }
     MSG_FATAL("---------------------------------------------1",0,0,0);
    pMe = (ITlgAtv*)MALLOC(sizeof(ITlgAtv));
    
    if (pMe != NULL)
    { 
#ifndef tv_ver2    
  
        ret = OEMCameraEx_New(AEECLSID_CAMERA3, (IBitmap *)pMe->pBitmap, pBitmapSize, (OEMINSTANCE *)&pMe->pCamera);  
        if (ret != SUCCESS)
        {
         MSG_FATAL("---------------------------------------------3",0,0,0);
            OEMATV_PRINT("OEMCamera_New  failed ret= %d",ret,0,0);
            return ret;
        } 
#endif        
        
 //       pMe->bTvStart = FALSE;
        MEMSET(pMe,0x00,sizeof(ITlgAtv));
        pMe->pvt = &gMMITlgAtvfuncs;                
        pMe->nRefs = 1;
        *ppInterface = (OEMINSTANCE)pMe;
        ret = SUCCESS;
        //test
       // ERR("SND_DEVICE_SPEAKER_FM = %d",SND_DEVICE_SPEAKER_FM,0,0);
       
       // g_OEMAtv = pMe;
    }
    else
    {    
    MSG_FATAL("---------------------------------------------2",0,0,0);
        OEMATV_PRINT("OEMAtv_New MALLOC failed",0,0,0);
        ret= EFAILED;
    }
 //   time_secure_get_local_time_ms(&time);
  //  OEMATV_PRINT("OEMTLGAtv_New Leave time=%u\n",time, 0, 0);
    OEMATV_PRINT("OEMTLGAtv_New Leave ret = %d",ret,0,0);
    return ret;
}


#ifdef tv_ver2
static int OEMTLGAtv_Create(OEMINSTANCE h)
{   
    ITlgAtv *pMe = (ITlgAtv *)h;
    uint16 pBitmapSize = 0;
    int ret = EFAILED;
    
    if (pMe != NULL)
    {
        ret = OEMCameraEx_New(AEECLSID_CAMERA3, (IBitmap *)pMe->pBitmap, pBitmapSize, (OEMINSTANCE *)&pMe->pCamera);  
        if (ret != SUCCESS)
        {
            OEMATV_PRINT("OEMCamera_New  failed ret= %d",ret,0,0);
          //  ((((FrameBitmap *)pMe->pBitmap)->dib).pvt)->Release((IBitmap *)pMe->pBitmap);
            //FREE(pMe);
            return ret;
        } 
    } 
    
    return ret;
}
#endif


static int OEMTLGAtv_Delete(void *h)
{
    ITlgAtv *pMe = (ITlgAtv *)h;

    if (pMe == NULL||pMe->pCamera == NULL)
    {
        OEMATV_PRINT("param error",0,0,0);    
        return EFAILED;
    }
    OEMATV_PRINT("OEMTLGAtv_Delete invoke",0,0,0); 
    OEMCamera_Delete(pMe->pCamera);    
    if (pMe->pBitmap !=NULL)
    {
     //   ((((FrameBitmap *)pMe->pBitmap)->dib).pvt)->Release((IBitmap *)pMe->pBitmap);    //调用cameraframe的release函数释放
    }
    FREE(pMe);
    g_OEMAtv.pAtv = NULL;
    //g_OEMAtv = NULL;
    return SUCCESS;
}


static uint32 OEMTLGAtv_AddRef(ITlgAtv* pIAtv)
{   
    OEMATV_PRINT("OEMTLGAtv_IBase_AddRef invoke",0,0,0);
    if (pIAtv == NULL)
    {
        OEMATV_PRINT("param error",0,0,0);    
        return EFAILED;
    }
    pIAtv->nRefs++;

    return pIAtv->nRefs;
}


static uint32 OEMTLGAtv_Release(ITlgAtv* pIAtv)
{
    OEMATV_PRINT("OEMTLGAtv_IBase_Release invoke",0,0,0);    
    pIAtv->nRefs--;
    if(pIAtv->nRefs == 0)
    {
        OEMTLGAtv_Delete(pIAtv);
		return SUCCESS;
    }
    return pIAtv->nRefs;
}


static int  OEMTLGAtv_GetFrame(OEMINSTANCE  h, IBitmap ** ppFrame)
{
    ITlgAtv *pMe = (ITlgAtv *)h;
    OEMCamera *pCameraMe = NULL;
        
  //  OEMATV_PRINT("OEMTLGAtv_GetFrame invoke",0,0,0);
    //这里为了释放display buffer计数
 //   ERR("OEMTLGAtv_GetFrame",0,0,0);
    if (h == NULL)
    {
        OEMATV_PRINT("param error",0,0,0);    
        return EFAILED;
    }
    //pCameraMe = (OEMCamera *)pMe->pCamera;
    OEMCamera_ReleaseFrame(h);
   // if (ppFrame != NULL ) //|| pCameraMe->pFrame != NULL
   // {
   //     ((((FrameBitmap *)pCameraMe->pFrame)->dib).pvt)->AddRef((IBitmap *)pCameraMe->pFrame);//添加pframe的引用
   //     *ppFrame = (IBitmap *)pCameraMe->pFrame;
  //      return SUCCESS;
  //  }    
    
    return EFAILED;
}


static int OEMTLGAtv_QueryInterface(ITlgAtv* pIAtv, AEECLSID ClsID, void** ppInterface)
{
    OEMATV_PRINT("OEMTLGAtv_IBase_QueryInterface invoke",0,0,0);
	MSG_FATAL("OEMTLGAtv_IBase_QueryInterface invoke",0,0,0);
	*ppInterface = NULL;

    if(ClsID == AEECLSID_QUERYINTERFACE || ClsID == AEECLSID_TLG_ATV)
    {
        ;//IBASE_COPY_IPTR(*ppInterface,pIAtv); 
    }

    return SUCCESS;
}


static int OEMTLGAtv_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam)
{
   
    ITlgAtv *pMe = (ITlgAtv *)h;
    int ret = EFAILED;

    if (pMe == NULL||pMe->pCamera == NULL)
    {
        OEMATV_PRINT("param error",0,0,0);  
        return ret;
    }
    OEMATV_PRINT("OEMTLGAtv_Start invoke",0,0,0);
    ret = OEMCameraEx_Start( pMe->pCamera,  nMode,  dwParam);   
    
    
    return ret;
}



static int OEMTLGAtv_Stop(OEMINSTANCE h)
{
    int ret = EFAILED;
    ITlgAtv *pMe = (ITlgAtv *)h;
    
    OEMATV_PRINT("OEMTLGAtv_Stop invoke",0,0,0);

    if (pMe == NULL||pMe->pCamera == NULL)
    {
        OEMATV_PRINT("param error",0,0,0);  
        return ret;
    }
    ret = OEMCamera_Stop(pMe->pCamera);
    OEMATV_PRINT("OEMTLGAtv_Stop  ret =%d",ret,0,0);
    return ret;
}


static int OEMTLGAtv_RegisterNotify(OEMINSTANCE h, ATV_NOTIFY_HANDLE_ID_e Id,PFNATVNOTIFY pfnNotify, void * pUser)
{
    ITlgAtv *pMe = (ITlgAtv *)h;

    if (pMe == NULL||pMe->pCamera == NULL)
    {
        OEMATV_PRINT("param error",0,0,0);
        return EFAILED;
    }
    OEMATV_PRINT("OEMTLGAtv_RegisterNotify invoke",0,0,0);
    switch (Id)
    {
        case HANDLE_FOR_TV_MAIN:
        {
            return OEMCamera_RegisterNotify(pMe->pCamera,(PFNCAMERANOTIFY)pfnNotify, pUser);    
        }break;
        
        case HANDLE_FOR_TV_SETCHN:
        {
            pMe->pfnNotify = pfnNotify;
            pMe->pUser = pUser;
    
        }break;
    }

   return SUCCESS;

}




/*chn参数:从1到该地区具有的频道号*/

static int OEMTLGAtv_SetChn(OEMINSTANCE h,ATV_U16 chn,ATV_SET_CHN_TYPE_e type)
{
   // ITlgAtv *pMe = (ITlgAtv *)h;
   //   unsigned long time;
   // time_secure_get_local_time_ms(&time);
   // OEMATV_PRINT("OEMTLGAtv_SetChn Enter time=%u\n",time, 0, 0);
    OEMATV_PRINT("OEMTLGAtv_SetChn invoke",0,0,0);

    if (h == NULL)
    {
        OEMATV_PRINT("param error",0,0,0);
        return 0;
    }
    switch (type)
    {
        case ATV_AUTO_SCAN:
        {
            if (h != NULL)
            {
                g_OEMAtv.pAtv= h;
                g_OEMAtv.chn = chn;
                rex_set_sigs( &hs_tcb, HS_ATV_SET_CHN_SIG);
                return 0;
            }
        }break;
        
        case ATV_FAST_SET:
        {
            int ret = TLG1120_tv_set_fast_channel(chn);
  //          time_secure_get_local_time_ms(&time);
   //         OEMATV_PRINT("TLG1120_tv_set_fast_channel Leave time=%u\n",time, 0, 0);
            
            return ret;   
        }break;
        
        case ATV_SET_AV_STOP:
        {
            int ret = TLG1120_tv_set_channel_ext(chn);
 //           time_secure_get_local_time_ms(&time);
  //          OEMATV_PRINT("TLG1120_tv_set_channel_ext Leave time=%u\n",time, 0, 0);
        
            return ret;
        }break;
    }    

    return 0;
}


void hs_start_set_chn(void)
{
    int ret = SUCCESS;
    ITlgAtv *pMe =(ITlgAtv*)g_OEMAtv.pAtv;
    ATV_SetChn_Notify_t notify;
    
    OEMATV_PRINT("hs_start_set_chn notify chn",0,0,0);
    rex_clr_sigs(&hs_tcb, HS_ATV_SET_CHN_SIG);
    if (pMe == NULL)
    {
        OEMATV_PRINT("hs_start_set_chn param wrong",0,0,0);
        return;
    }
    ret = TLG1120_tv_set_channel(g_OEMAtv.chn);
    OEMATV_PRINT("hs_start_set_chn notify chn ret=%d",ret,0,0);
    if (ret == 0)                
    {
        notify.status = ATV_SET_CHN_FAILED;
    }
    else
    {            
        rex_sleep(250);   /*如果搜到节目则设置一个sleep，是用户可以看到画面*/
        rex_sleep(250);
        notify.status = ATV_SET_CHN_SUCCESS;
        notify.chn = ret;
    }
    if (g_OEMAtv.pAtv!=NULL&&pMe->pfnNotify!=NULL&&pMe->pUser!=NULL)
    {           
       OEMATV_PRINT("hs_start_set_chn notify chn ret=%d",ret,0,0);
       pMe->pfnNotify(pMe->pUser, (void *)(&notify));
    }
    OEMATV_PRINT("hs_start_set_chn notify chn ret=%d",ret,0,0);
}


static int OEMTLGAtv_SetParam(OEMINSTANCE h,ATV_SET_PARAM_e type, int hparam, int lparam)
{
   int ret = SUCCESS;
   ITlgAtv *pMe = (ITlgAtv *)h;
   
   OEMATV_PRINT("OEMTLGAtv_SetParam invoke type = %d",type,0,0);
   switch (type)
   {
        case CAM_SET_DISPLAY_SIZE:
        {
            AEESize size = *(AEESize *)hparam;

            OEMATV_PRINT("OEMTLGAtv_SetParam CAM_SET_DISPLAY_SIZE w=%d, h=%d",size.cx, size.cy,0);
            ret = OEMCamera_SetParm((ICameraEx *)pMe->pCamera, CAM_PARM_DISPLAY_SIZE, hparam, 0);             
        }break;

        case CAM_SET_SIZE:
        {
            AEESize size = *(AEESize *)hparam;

            OEMATV_PRINT("OEMTLGAtv_SetParam CAM_SET_SIZE w=%d, h=%d",size.cx, size.cy,0);            
            ret = OEMCamera_SetParm((ICameraEx *)pMe->pCamera, CAM_PARM_SIZE, hparam, 0);   
            if (ret != SUCCESS)
            {
                OEMATV_PRINT("OEMTLGAtv_SetParam OEMCamera_SetParm %d",ret,0,0);
                break;
            }

        }break;
  #ifdef tv_ver2
       case TLG_SET_REGION:
       {
            ret = TLGMMI_SetRegion((TLG_REGION_CODE)hparam);
            if (ret == 0&&(pMe->pCamera!=NULL))   /*如果电视没有创建怎不能对芯片操作*/
            {
                ret = TLG1120_tv_set_param(type,hparam,lparam);
            }
       }break;
  #endif
        default:     
        {
            ret = TLG1120_tv_set_param(type,hparam,lparam);
            
       }break;
   }


   OEMATV_PRINT("OEMTLGAtv_SetParam invoke ret = %d",ret,0,0);
   return ret;
}


static int OEMTLGAtv_GetParam(OEMINSTANCE h,ATV_GET_PARAM_e type, void * hparam, void * lparam)
{
    int ret = SUCCESS;

    ERR("OEMTLGAtv_GetParam type = %d",type,0,0);
    ret = TLG1120_tv_get_param(type,hparam,lparam);

    return ret;
}

static int OEMTLGAtv_Update(OEMINSTANCE h, uint32 dwParam)
{


	//OEMCamera * pme = (OEMCamera *)h;
    uint16* pbmp = (uint16*)dwParam;
	int i;
	//MEMSET(Tv_fbuffer, 0x0, 320*240);
	MSG_FATAL("OEMTLGAtv_Update--------------------------start",0,0,0);
	//for(i=0;i<320*240;i++)
	//{
	//	Tv_fbuffer[i] = 0xf800;
	//}
    MEMCPY(Tv_fbuffer, pbmp, 77440); //77440 = sizeof(g_fbuffer)
    AIT701_cam_update_osd(Tv_fbuffer,0,0,320,240);
    return SUCCESS;
}


#endif



