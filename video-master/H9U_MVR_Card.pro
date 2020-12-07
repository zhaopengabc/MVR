TEMPLATE = app

QT += core  websockets
QT -= gui

CONFIG += console c++11 debug
CONFIG -= app_bundle
#CONFIG -= qt
QMAKE_CFLAGS +=  -std=gnu99  -Wno-sign-compare
#QMAKE=/home/zc/cross/v500/qt5/bin/qmake

#INCLUDEPATH+= /opt/bsp/hi3798/HiSTBLinuxV100R005C00SPC050/source/msp/include/
#INCLUDEPATH+= /opt/bsp/hi3798/HiSTBLinuxV100R005C00SPC050/source/common/include/
#INCLUDEPATH+= /opt/bsp/hi3798/HiSTBLinuxV100R005C00SPC050/sample/common/

HI_SDK=../../build/Hi3531D_SDK_V1.0.3.0
MPP_DIR=$$HI_SDK/mpp
INCLUDEPATH+=./cFWK/include/
INCLUDEPATH+=./libcommon/
INCLUDEPATH+=./include/
INCLUDEPATH+=./include/liveMedia
INCLUDEPATH+=./include/groupsock
INCLUDEPATH+=./include/UsageEnvironment
INCLUDEPATH+=./include/BasicUsageEnvironment
LIBS+=-L./lib/ -lwebsockets -lssl -lcrypto  -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment#      -lliveMedia

INCLUDEPATH+=$$MPP_DIR/include

#MPP_DIR=/home/linkpi/work/3531D/SDK/Hi3531D_SDK_V1.0.3.0/mpp

MPP_LIB_DIR = $$MPP_DIR/lib
LIBS += -ldl -lpthread
LIBS +=  $$MPP_LIB_DIR/libmpi.a $$MPP_LIB_DIR/libupvqe.a $$MPP_LIB_DIR/libdnvqe.a $$MPP_LIB_DIR/libVoiceEngine.a $$MPP_LIB_DIR/libjpeg.a $$MPP_LIB_DIR/libhdmi.a

INCLUDEPATH+= $$MPP_DIR/extdrv/tlv320aic31/ \
                $$MPP_DIR/extdrv/nvp6134_ex/ \
                $$MPP_DIR/extdrv/tp2853c/    \
                $$MPP_DIR/sample/audio/adp/ \
                CXStreamer/common/

SOURCES += main.cpp \
    CXLog/CXLog.cpp \
    CXStreamer/factory.cpp \
    CXStreamer/element.cpp \
    CXStreamer/Hisilicon/Hi3531DV100/factoryforhi3531dv100.cpp \
    CXStreamer/Hisilicon/Hi3531DV100/vihi3531dv100.cpp \
    CXStreamer/Hisilicon/hielement.cpp \
    CXStreamer/Hisilicon/Hi3531DV100/hi3531dv100_lib.cpp \
    CXStreamer/Hisilicon/Hi3531DV100/vohi3531dv100.cpp \
    CXStreamer/Hisilicon/Hi3531DV100/venchi3531dv100.cpp \
    CXStreamer/Hisilicon/Hi3531DV100/vpsshi3531dv100.cpp \
    CXStreamer/Hisilicon/Hi3531DV100/common/loadbmp.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_audio.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_ivs.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_sys.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_vda.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_vdec.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_venc.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_vi.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_vo.c \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_vpss.c \
    CXStreamer/common/factorycommon.cpp \
    CXStreamer/common/websocketserver.cpp \
    CXStreamer/common/httpserver.cpp \
    CXStreamer/common/websocketserverimplqt.cpp \
    CXStreamer/common/websocketserverinf.cpp \
    CXStreamer/common/livertspserver.cpp \
    CXStreamer/common/FramedPipeSource.cpp \
    CXStreamer/common/RTSPH264PassiveServer.cpp \
    CXStreamer/common/RTSPServer.cpp \
    CXStreamer/common/RTSPOnDemandServer.cpp \
    CXStreamer/common/VideoPipeServerMediaSubsession.cpp \ 
    CXStreamer/common/websocketserverimplcfwk.cpp \
    player.cpp

HEADERS += \
    CXStreamer/factory.h \
    CXStreamer/element.h \
    CXStreamer/cxstreamer.h \
    CXStreamer/Hisilicon/hielement.h \
    CXStreamer/Hisilicon/hi_comm_cfg.h \
    CXStreamer/Hisilicon/Hi3531DV100/hi_3531dv100_cfg.h \
    CXStreamer/Hisilicon/cxs_hi_comm.h \
    CXStreamer/Hisilicon/Hi3531DV100/hi3531dv100_lib.h \
    CXStreamer/Hisilicon/Hi3531DV100/common/loadbmp.h \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm_ivs.h \
    CXStreamer/Hisilicon/Hi3531DV100/common/sample_comm.h \
    CXStreamer/common/httpserver.h \
    CXStreamer/common/websocketserverimplqt.h \
    CXStreamer/common/websocketserverinf.h \
    CXStreamer/common/FramedPipeSource.h \
    ./libcommon/zstd.h \
    CXLog/CXLog.h \
    CXStreamer/common/RTSPH264PassiveServer.h \
    CXStreamer/common/RTSPServer.h \
    CXStreamer/common/RTSPOnDemandServer.h \
    CXStreamer/common/VideoPipeServerMediaSubsession.h \
    CXStreamer/Hisilicon/Hi3531DV100/hi_config.h \
    CXStreamer/common/websocketserverimplcfwk.h \
    player.h \
    configer.h \
    global.h

#DISTFILES +=
