#include <string.h>
#include "../../factory.h"

#include "../hi_comm_cfg.h"
#include "hi_3531dv100_cfg.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "hi_comm_vb.h"

namespace CXS{
    class FactoryForHi531DV00 : public Factory
    {
        FactoryForHi531DV00(){
        }
        ~FactoryForHi531DV00(){
        }

    public:
        DECLARE_FACTORY_GET_INSTANCE(FactoryForHi531DV00)

    };
    static int init()
    {
        HI_S32 s32Ret = HI_FAILURE;
        VB_CONF_S stVbConf;
        memset(&stVbConf,0,sizeof(VB_CONF_S));
        HI_U32 u32Width  = CEILING_2_POWER(CXS_HI_3531D_VIDEO_MAX_WIDTH, CXS_HI_3531D_SYS_ALIGN_WIDTH);
        HI_U32 u32Height  = CEILING_2_POWER(CXS_HI_3531D_VIDEO_MAX_HEIGHT, CXS_HI_3531D_SYS_ALIGN_WIDTH);
        HI_U32 u32BlkSize = u32Width * u32Height * 2; //统一按照PIXEL_FORMAT_YUV_SEMIPLANAR_422的规格处理
        HI_U32 headSize = VB_HEADER_STRIDE * (u32Height) * 2;
        u32BlkSize += headSize;

         stVbConf.u32MaxPoolCnt = VB_MAX_POOLS; // 128
         /* video buffer*/
         //todo: vb=15

         stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
         stVbConf.astCommPool[0].u32BlkCnt = VI_CHN_CNT * 5 ;

         stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
         stVbConf.astCommPool[1].u32BlkCnt = VI_CHN_CNT * 5 ;


        HI_MPI_SYS_Exit();

            for(HI_S32 i=0;i<VB_MAX_USER;i++)
            {
                 HI_MPI_VB_ExitModCommPool((VB_UID_E)i);
            }
            for(HI_S32 i=0; i<VB_MAX_POOLS; i++)
            {
                 HI_MPI_VB_DestroyPool(i);
            }
            HI_MPI_VB_Exit();

            s32Ret = HI_MPI_VB_SetConf(&stVbConf);
            if (HI_SUCCESS != s32Ret)
            {
                CXS_PRT("HI_MPI_VB_SetConf failed!\n");
                return HI_FAILURE;
            }

            s32Ret = HI_MPI_VB_Init();
            if (HI_SUCCESS != s32Ret)
            {
                CXS_PRT("HI_MPI_VB_Init failed!\n");
                return HI_FAILURE;
            }

            MPP_SYS_CONF_S stSysConf = {0};
            stSysConf.u32AlignWidth = CXS_HI_3531D_SYS_ALIGN_WIDTH;
            s32Ret = HI_MPI_SYS_SetConf(&stSysConf);
            if (HI_SUCCESS != s32Ret)
            {
                CXS_PRT("HI_MPI_SYS_SetConf failed\n");
                return HI_FAILURE;
            }

            s32Ret = HI_MPI_SYS_Init();
            if (HI_SUCCESS != s32Ret)
            {
                CXS_PRT("HI_MPI_SYS_Init failed!\n");
                return HI_FAILURE;
            }

        return HI_SUCCESS == s32Ret ? 0:-1;
    }
    static void deInit()
    {
        HI_MPI_SYS_Exit();
    }
    static struct FactoryDescriptor factoryDescriptor  = {
        "hi3531dv100",
         init,
        deInit,
        FactoryForHi531DV00::getInstance
    };
//    static FactoryDescriptor factoryDescriptor  {
//        .factoryName = "hi3531dv100",
//        .init = init,
//        .deInit = deInit,
//        .getFactoryInstance = FactoryForHi3798MV200::getInstance
//    };
    REGISTER_FACTORY(&factoryDescriptor);

}
