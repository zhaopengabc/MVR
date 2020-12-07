#include <string.h>
#include "../../factory.h"
#include "../hielement.h"
#include "../cxs_hi_comm.h"
#include "common/sample_comm.h"
#include"hi_config.h"
//#include"hi_common.h"
//#include"hi_comm_vpss.h"
#include "mpi_vpss.h"
//#include "hi3531dv100_lib.h"
#include "zstd.h"
namespace CXS{
    #define ELEMENT_CLASS_NAME  "vpss"
    class VPSSHi3531DV100 : public HiElement
    {
        VPSSHi3531DV100():HiElement(ELEMENT_CLASS_NAME){
//            mViPort = HI_UNF_VI_PORT0;
//            mAttr.bVirtual = HI_TRUE;
//            mAttr.u32BufNum = 6;
        }
    public:
        static Element* createInstance()
        {
            VPSSHi3531DV100* elem = new VPSSHi3531DV100();
            return elem;
        }
        int startSelf(){
            VPSS_GRP_ATTR_S stGrpAttr;
            mHandle = getAttr("VPSS-VO-CHN",0);
            memset(&stGrpAttr,0,sizeof(VPSS_GRP_ATTR_S));
            //if(m_mode == "HD1080"){
                stGrpAttr.u32MaxW = 1920;
                stGrpAttr.u32MaxH = 1080;
            //}

            stGrpAttr.enPixFmt = PIXEL_FORMAT;
            stGrpAttr.bNrEn = HI_FALSE;
            stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
            HI_S32 ret = HI_MPI_VPSS_CreateGrp(mHandle, &stGrpAttr);
            if (ret != HI_SUCCESS)
            {
                CXS_PRT("HI_MPI_VPSS_CreateGrp failed with %#x!\n", ret);
                return HI_FAILURE;
            }

            /*** set vpss param ***/
            VPSS_GRP_PARAM_S stVpssParam;
            memset(&stVpssParam,0,sizeof(VPSS_GRP_PARAM_S));
            ret = HI_MPI_VPSS_GetGrpParam(mHandle, &stVpssParam);
            if (ret != HI_SUCCESS)
            {
                CXS_PRT("failed with %#x!\n", ret);
                return HI_FAILURE;
            }

            stVpssParam.u32IeStrength = 0;
            ret = HI_MPI_VPSS_SetGrpParam(mHandle, &stVpssParam);
            if (ret != HI_SUCCESS)
            {
                CXS_PRT("failed with %#x!\n", ret);
                return HI_FAILURE;
            }
            VPSS_CHN VpssChn = 0;
            VPSS_CHN_ATTR_S stChnAttr;
            memset(&stChnAttr,0,sizeof(VPSS_CHN_ATTR_S));
            /* Set Vpss Chn attr */
            stChnAttr.bSpEn = HI_FALSE;
            stChnAttr.bUVInvert = HI_FALSE;
            stChnAttr.bBorderEn = HI_TRUE;
            stChnAttr.stBorder.u32Color = 0xffffff;
            stChnAttr.stBorder.u32LeftWidth = 2;
            stChnAttr.stBorder.u32RightWidth = 2;
            stChnAttr.stBorder.u32TopWidth = 2;
            stChnAttr.stBorder.u32BottomWidth = 2;

            ret = HI_MPI_VPSS_SetChnAttr(mHandle, VpssChn, &stChnAttr);
            if (ret != HI_SUCCESS)
            {
                CXS_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x\n", ret);
                return HI_FAILURE;
            }

            ret = HI_MPI_VPSS_EnableChn(mHandle, VpssChn);
            if (ret != HI_SUCCESS)
            {
                CXS_PRT("HI_MPI_VPSS_EnableChn failed with %#x\n", ret);
                return HI_FAILURE;
            }
            ret = HI_MPI_VPSS_StartGrp(mHandle);
            if (ret != HI_SUCCESS)
            {
                CXS_PRT("HI_MPI_VPSS_StartGrp failed with %#x\n", ret);
                return HI_FAILURE;
            }

            CXS_PRT("VPSS(%d) start ok\n",mHandle);
            return ret;
        }
        int linkTo(Element* elem){
            HiElement* hiElem = dynamic_cast<HiElement*>(elem);
            HI_S32 ret = HI_FAILURE;
            if(strcmp(hiElem->getClassName(),"vo") == 0)
            {
                HI_S32 voChn = getAttr("VPSS-VO-CHN",0);
                ret = SAMPLE_COMM_VO_BindVpss( hiElem->getHandle(),  voChn,  mHandle,0);
                if (HI_SUCCESS != ret)
                {
                    CXS_PRT("bind  Vpss and vo failed!\n");
                }
                else
                {
                    CXS_PRT("bind  Vpss and vo success!\n");
                }
            }
            return ret;
        }
    };
    static int init()
    {
        return HI_SUCCESS;
    }
    static void deInit()
    {

    }
    static ElementDescriptor elementDescriptors[] = {
        {
             "hi3531dv100",
             ELEMENT_CLASS_NAME,
             init,
             deInit,
             VPSSHi3531DV100::createInstance
        },
    };
    REGISTER_ELEMENT(elementDescriptors,sizeof(elementDescriptors)/sizeof(elementDescriptors[0]));
}
