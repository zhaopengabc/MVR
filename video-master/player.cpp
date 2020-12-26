#include "player.h"
#include <stdlib.h>
#include "cf_logger/cf_logger.h"
#include "cf_threadpool/cf_threadpool.h"
#include "cf_websocket/cf_websocket_server.h"
#include "cf_std.h"
#include "cf_json/cf_json.h"
#include "global.h"
#include <string.h>
#include <fstream>
#include <iostream>
#define STREAM_CHN1_ON 1
#define STREAM_CHN2_ON 1

struct Buffer
{
    uint8_t *buffer;
    size_t len;
};
Player::Player()
{
    mElemHead1 = nullptr;
    mElemHead2 = nullptr;
}
CXS::Element *RTSPElement_0;
CXS::Element *RTSPElement_1;


extern "C"
{
#define MAX_FRAME_RATE 15
#define MIN_FRAME_RATE 5
    static CXS::Element *elemVenc0 = NULL;
    static CXS::Element *elemVps = NULL;
    static CXS::Element *rtsp_server0 = NULL;
    static CXS::Element *rtsp_server1 = NULL;
    static CXS::Element *ws_server0 = NULL;
    static CXS::Element *ws_server1 = NULL;

    static CXS::Element *elemVenc1 = NULL;
    static CXS::Element *elemVpe1 = NULL;
    static std::string to_String(int n)
    {
        int m = n;
        char s[100];
        char ss[100];
        int i = 0, j = 0;
        if (n < 0) // 处理负数
        {
            m = 0 - m;
            j = 1;
            ss[0] = '-';
        }
        while (m > 0)
        {
            s[i++] = m % 10 + '0';
            m /= 10;
        }
        s[i] = '\0';
        i = i - 1;
        while (i >= 0)
        {
            ss[j++] = s[i--];
        }
        ss[j] = '\0';
        return ss;
    }
    static void ws_cli_max_pending_changed_handler(void *user_data, void *event_para)
    {
        CXS::Element *elem = static_cast<CXS::Element *>(user_data);
        int max_pendings = reinterpret_cast<int>(event_para);
        //printf("max pending = %d,venc_handle = %d\n",max_pendings,elem->getAttr("VENC-CHN",0));

        int frame_rate = elem->getAttr("VENC-FRAMERATE", 15);
        //printf("frame_rate = %d\n",frame_rate);
        if (max_pendings > 8)
        {
            int t_rate = frame_rate * 9 / 10;
            if (frame_rate == t_rate)
                frame_rate = t_rate - 1;
            else
                frame_rate = t_rate;
        }
        else if (max_pendings < 2)
        {
            int t_rate = frame_rate * 11 / 10;
            if (frame_rate == t_rate)
                frame_rate = t_rate + 1;
            else
                frame_rate = t_rate;
        }

        if (frame_rate > MAX_FRAME_RATE)
            frame_rate = MAX_FRAME_RATE;
        else if (frame_rate < MIN_FRAME_RATE)
            frame_rate = MIN_FRAME_RATE;
        //printf("frame_rate2 = %d\n",frame_rate);

        if (elem == elemVps)
        {
            elem->setAttr("VENC-FRAMERATE", frame_rate);
        }
        else if (rtsp_server0)
        {
            bool isConnected = rtsp_server0->getAttr("isConnected", false);
            if (isConnected)
                elem->setAttr("VENC-FRAMERATE", MIN_FRAME_RATE);
            else
                elem->setAttr("VENC-FRAMERATE", frame_rate);
        }
        else
            elem->setAttr("VENC-FRAMERATE", frame_rate);
    }
}

#define MAX_LAYER_NUMS 128

#define WORK_MODE_HD 0
#define WORK_MODE_4K 1
#define WORK_MODE0 WORK_MODE_4K
#define WORK_MODE1 WORK_MODE_HD
static int vi_horizontal_blks = 8;
static int vi_vertical_blks = 8;
static int dst_horizontal_blks = 8;
static int dst_vertical_blks = 8;
static int layer_layout_tab[MAX_LAYER_NUMS];
//static void ws_layout_cfg_handler(void* ,void* event_para){
//    uint8_t* layout = (uint8_t*)event_para;
//    dst_horizontal_blks = layout[0];
//    dst_vertical_blks = layout[1];
//    for(int i = 0;i < dst_horizontal_blks*dst_vertical_blks;i++){
//        layer_layout_tab[i] = layout[2+i];
//    }
//}

#if 0
static bool isH15(){
    static int slot_id = -1;
    if(slot_id == -1){
        std::ifstream f("/dev/slot_id");
        if(f.is_open()){
            f >> slot_id;
            f.close();
        }
    }
    return slot_id == 41 ? true : false;
}


static void conf_for_hseries_compatible_old(){
    if(isH15() ){
        vi_horizontal_blks = 2;
        vi_vertical_blks = 1;
        dst_horizontal_blks = 1;
        dst_vertical_blks = 1;
    }
    else{
        vi_horizontal_blks = 2;
        vi_vertical_blks = 2;
        dst_horizontal_blks = 1;
        dst_vertical_blks = 1;
    }
}
#endif

static void read_layout_conf()
{
    boost::property_tree::ptree &confPt = global.conf.json();
    if (confPt.get("mvr_mode", std::string("unfixed")) == "fixed")
    {
        vi_horizontal_blks = 2;
        vi_vertical_blks = 1;
        dst_horizontal_blks = 1;
        dst_vertical_blks = 1;

        for (int i = 0; i < MAX_LAYER_NUMS; i++)
        {
            layer_layout_tab[i] = i;
        }
    }
    else
    {
        vi_horizontal_blks = 8;
        vi_vertical_blks = 8;

        dst_vertical_blks = 8;
        dst_horizontal_blks = 8;
        if (confPt.count("layout") < 1)
        {
            for (int i = 0; i < MAX_LAYER_NUMS; i++)
            {
                layer_layout_tab[i] = i;
            }
        }
        else
        {
            boost::property_tree::ptree layout = confPt.get_child("layout");
            int val = layout.get<int>("rows", -1);
            if (val != -1)
                dst_vertical_blks = val;
            val = layout.get<int>("cols", -1);
            if (val != -1)
                dst_horizontal_blks = val;

            boost::property_tree::ptree sources = layout.get_child("sources");

            for (int i = 0; i < MAX_LAYER_NUMS; i++)
            {
                layer_layout_tab[i] = -1;
            }
            for (auto it = sources.begin(); it != sources.end(); it++)
            {
                boost::property_tree::ptree &source = it->second;
                int row = source.get<int>("rowNo", -1);
                int col = source.get<int>("colNo", -1);
                int inputId = source.get<int>("inputId", -1);
                if (row != -1 && col != -1 && inputId != -1)
                {
                    layer_layout_tab[row * dst_horizontal_blks + col] = inputId;
                }
                else
                {
                }
            }
        }
    }
}
static void write_layout_conf()
{
    boost::property_tree::ptree &confPt = global.conf.json();
    boost::property_tree::ptree layout;
    if (confPt.count("layout") > 1)
    {
        layout = confPt.get_child("layout");
    }
    layout.put("rows", dst_vertical_blks);
    layout.put("cols", dst_horizontal_blks);
    boost::property_tree::ptree sources;
    for (int i = 0; i < MAX_LAYER_NUMS; i++)
    {
        if (layer_layout_tab[i] != -1)
        {
            boost::property_tree::ptree source;
            source.put("rowNo", i / dst_horizontal_blks);
            source.put("colNo", i % dst_horizontal_blks);
            source.put("inputId", layer_layout_tab[i]);
            sources.push_back(std::make_pair("", source));
        }
    }
    layout.put_child("sources", sources);
    confPt.put_child("layout", layout);
    global.conf.save();
}

static void send_layout_cfg(cf_websocket *cli)
{
    cf_json *root = cf_json_create_object();
    cf_json_add_string_to_object(root, "topic", "layout_cfg");
    cf_json *layout = cf_json_create_object();
    boost::property_tree::ptree &conf = global.conf.json();
    std::string mvr_mode = conf.get<std::string>("mvr_mode", "unfixed");
    cf_json_add_string_to_object(root, "mvr_mode", mvr_mode.c_str());
    int _dst_vertical_blks = 16;
    int _dst_horizontal_blks = 8;
    if (mvr_mode == "unfixed")
    {
        _dst_vertical_blks = dst_vertical_blks;
        _dst_horizontal_blks = dst_horizontal_blks;
    }
    else
    { // "fixed"
        for (int i = 0; i < _dst_vertical_blks * _dst_horizontal_blks; i++)
        {
            layer_layout_tab[i] = i;
        }
    }

    cf_json_add_int_to_object(layout, "rows", _dst_vertical_blks);
    cf_json_add_int_to_object(layout, "cols", _dst_horizontal_blks);

    cf_json_add_item_to_object(root, "layout", layout);
    cf_json *pos = cf_json_create_array();
    for (int i = 0; i < _dst_vertical_blks * _dst_horizontal_blks; i++)
    {
        if (layer_layout_tab[i] == -1)
            continue;
        cf_json *tmp = cf_json_create_object();
        if (0 && i == 32)
        {
            cf_json_add_int_to_object(tmp, "rowNo", i / _dst_horizontal_blks);
            cf_json_add_int_to_object(tmp, "colNo", i % _dst_horizontal_blks);
            cf_json_add_int_to_object(tmp, "inputId", 18);
        }
        else if (0 && i == 18)
        {
            cf_json_add_int_to_object(tmp, "rowNo", i / _dst_horizontal_blks);
            cf_json_add_int_to_object(tmp, "colNo", i % _dst_horizontal_blks);
            cf_json_add_int_to_object(tmp, "inputId", 32);
        }
        else
        {
            cf_json_add_int_to_object(tmp, "rowNo", i / _dst_horizontal_blks);
            cf_json_add_int_to_object(tmp, "colNo", i % _dst_horizontal_blks);
            cf_json_add_int_to_object(tmp, "inputId", layer_layout_tab[i]);
        }
        cf_json_add_item_to_array(pos, tmp);
    }
    cf_json_add_item_to_object(root, "pos", pos);

    char buffer[1024 * 10] = {0};

    cf_json_print_preallocated(root, buffer, sizeof(buffer), 0);
    cf_json_destroy_object(root);
    cf_websocket_write_text(cli, buffer, strlen(buffer));

    std::cout << "send_layout_cfg        " << std::string(buffer, strlen(buffer)) << std::endl;
}
static void ws_on_cli_read_text(cf_websocket *cli, const char *buf, uint64_t n)
{
    cf_json *json = cf_json_load(buf);

    if (json)
        printf("xxxxxxxxxxxxxxxxx json=%s\n", cf_json_print(json));
    if (json && cf_json_contains(json, "topic") && strcmp(cf_json_get_string(json, "topic", nullptr), "query_cfg") == 0)
    {
        send_layout_cfg(cli);
    }
    else if (json && cf_json_contains(json, "topic") && strcmp(cf_json_get_string(json, "topic", nullptr), "layout_cfg") == 0)
    {
        if (global.conf.json().count("mvr_mode") > 0 && global.conf.json().get("mvr_mode", std::string("unfixed")) == "unfixed")
        {
            cf_json *layout = cf_json_get_item(json, "layout");
            dst_vertical_blks = cf_json_get_int(layout, "rows", NULL);
            dst_horizontal_blks = cf_json_get_int(layout, "cols", NULL);
            cf_json *pos = cf_json_get_item(json, "pos");
            int arr_size = cf_json_get_array_size(pos);
            for (int i = 0; i < MAX_LAYER_NUMS; i++)
            {
                layer_layout_tab[i] = -1;
            }
            for (int i = 0; i < arr_size; i++)
            {
                cf_json *item = cf_json_get_item_from_array(pos, i);
                int row = cf_json_get_int(item, "rowNo", NULL);
                int col = cf_json_get_int(item, "colNo", NULL);
                int inputId = cf_json_get_int(item, "inputId", NULL);
                layer_layout_tab[row * dst_horizontal_blks + col] = inputId;
            }
            write_layout_conf();
        }
        send_layout_cfg(cli);
    }
    cf_json_destroy_object(json);
}
static std::list<cf_websocket *> cliList;
void on_new_websocket(cf_websocket_server *, cf_websocket *cli)
{
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   new client connect  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    cliList.push_back(cli);
    send_layout_cfg(cli);
}
void on_disconnect_websocket(cf_websocket_server *, cf_websocket *cli)
{
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   client disconnect  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    cliList.remove(cli);
}

static void ws_server_run(void *)
{
    cf_websocket_server *server = cf_websocket_server_create(7999);
    cf_websocket_server_set_on_connect_callback(server, on_new_websocket);
    if (1)
    {
        cf_websocket_server_set_on_read_text_callback(server, ws_on_cli_read_text);
    }

    cf_websocket_server_run(server);
}
//arg == 1 for linkpi
int Player::RTSPCallBack_0(uint8_t *framedata, size_t datalen)
{
    struct Buffer data;
    data.len = datalen;
    data.buffer = framedata;
    // printf("\n\n\n\n =================RTSP call back ... datalen : %d \n",datalen);
    RTSPElement_0->pushData((void *)&data);
}
int Player::RTSPCallBack_1(uint8_t *framedata, size_t datalen)
{
    struct Buffer data;
    data.len = datalen;
    data.buffer = framedata;
    // printf("\n\n\n\n =================RTSP call back ... datalen : %d \n",datalen);
    RTSPElement_1->pushData((void *)&data);
}
Player *Player::getInstance(int chn1, int chn2, uint16_t port1, uint16_t port2, uint8_t protocol)
{
    Player *instance = nullptr;
    instance = new Player;

    Nova_EnocderCfg encodeCfg;
    encodeCfg.inputData.width = 1920;
    encodeCfg.inputData.height = 1080;
    encodeCfg.inputData.format = 0;

    encodeCfg.rateCtlData.RateCtrlMode = 1;
    encodeCfg.rateCtlData.FrameRate = 20;
    encodeCfg.rateCtlData.BitRate = 1024 * 1024 * 2;
    encodeCfg.rateCtlData.MaxBitRate = 1024 * 1024 * 2;
    encodeCfg.rateCtlData.SliceQP = -1;
    encodeCfg.rateCtlData.CPBSize = 1;
    encodeCfg.rateCtlData.InitialDelay = 0.5;
    encodeCfg.rateCtlData.ScnChgResilience = false;
    encodeCfg.rateCtlData.MaxPictureSize = 1000;

    encodeCfg.gopData.mode = 0;
    encodeCfg.gopData.lenght = 30;
    encodeCfg.gopData.numB = 0;

    encodeCfg.settingData.profile = 12;
    encodeCfg.settingData.level = 0;
    encodeCfg.settingData.tier = 0;
    encodeCfg.settingData.chromaMode = 0;
    encodeCfg.settingData.bitDepth = 8;
    encodeCfg.settingData.numSlices = 4;
    encodeCfg.settingData.QPCtrlMode = 1;
    encodeCfg.settingData.scalingList = 0;
    encodeCfg.settingData.entropyMode = 1;
    encodeCfg.settingData.loopFilter = true;
    encodeCfg.settingData.cacheLevel2 = true;

    instance->mElemHead1 = NovaEncoderInit(encodeCfg, RTSPCallBack_0);

    encodeCfg.inputData.width = 1280;
    encodeCfg.inputData.height = 720;
    encodeCfg.inputData.format = 0;

    encodeCfg.rateCtlData.RateCtrlMode = 1;
    encodeCfg.rateCtlData.FrameRate = 20;
    encodeCfg.rateCtlData.BitRate = 1024 * 1024 * 2;
    encodeCfg.rateCtlData.MaxBitRate = 1024 * 1024 * 2;
    encodeCfg.rateCtlData.SliceQP = -1;
    encodeCfg.rateCtlData.CPBSize = 1;
    encodeCfg.rateCtlData.InitialDelay = 0.5;
    encodeCfg.rateCtlData.ScnChgResilience = false;
    encodeCfg.rateCtlData.MaxPictureSize = 1000;

    encodeCfg.gopData.mode = 0;
    encodeCfg.gopData.lenght = 30;
    encodeCfg.gopData.numB = 0;

    encodeCfg.settingData.profile = 12;
    encodeCfg.settingData.level = 0;
    encodeCfg.settingData.tier = 0;
    encodeCfg.settingData.chromaMode = 0;
    encodeCfg.settingData.bitDepth = 8;
    encodeCfg.settingData.numSlices = 4;
    encodeCfg.settingData.QPCtrlMode = 1;
    encodeCfg.settingData.scalingList = 0;
    encodeCfg.settingData.entropyMode = 1;
    encodeCfg.settingData.loopFilter = true;
    encodeCfg.settingData.cacheLevel2 = true;

    instance->mElemHead2 = NovaEncoderInit(encodeCfg);

    return instance;
}
CXS::Element *Player::NovaEncoderInit(Nova_EnocderCfg nova_cfg)
{
    CXS::Factory *factory = CXS::Factory::getFactoryInstanceByName("SSC339G");
    CXS::Factory *factory_common = CXS::Factory::getFactoryInstanceByName("common");
    cf_assert(factory != nullptr && factory_common != nullptr);

    CXS::Element *mElemHead = factory->createElementByName("vif");
    elemVpe1 = factory->createElementByName("vpe");
    elemVenc1 = factory->createElementByName("venc");
    rtsp_server1 = factory_common->createElementByName("rtsp-server");
    rtsp_server1->setAttr("port", 555);
    rtsp_server1->setAttr("payload", "h264");

    mElemHead->setAttr("eSnrPad", "1");
    mElemHead->setAttr("vifDev", "2");
    mElemHead->setAttr("u32InputPort", "0");
    mElemHead->setAttr("vencChn", "2");
    mElemHead->setAttr("resolution", "HD");
    mElemHead->link(elemVpe1); // vif ->vep

    elemVpe1->setAttr("vpeChn", "2");
    elemVpe1->setAttr("vencChn", "2");
    elemVpe1->setAttr("pixelFormat", "44");
    elemVpe1->setAttr("sensorId", "1");
    elemVpe1->setAttr("resolution", "HD");
    elemVpe1->setAttr("ReGropMode", "No-Regroup");

    elemVpe1->link(elemVenc1); //vep -> venc

    elemVenc1->setAttr("vencChn", "2");
    elemVenc1->setAttr("resolution", "HD");
    elemVenc1->setAttr("compressionType", "H264");
    elemVenc1->setAttr("frameData", to_String(nova_cfg.rateCtlData.FrameRate));
    elemVenc1->setAttr("biteRate", to_String(nova_cfg.rateCtlData.BitRate));
    elemVenc1->setAttr("Gop", to_String(nova_cfg.gopData.lenght));

    elemVenc1->link(rtsp_server1); //venc -> rtsp

    mElemHead->mNextElems.push_back(elemVenc1);
    mElemHead->mNextElems.push_back(rtsp_server1);

    return mElemHead;
}

CXS::Element *Player::NovaEncoderInit(Nova_EnocderCfg nova_cfg,callback rtsp_push)
{
    // Player *instance = nullptr;
    for (int i = 0; i < MAX_LAYER_NUMS; i++)
    {
        layer_layout_tab[i] = i;
    }
    read_layout_conf();

    // if (!instance)
    // {
    cf_threadpool_run(ws_server_run, nullptr);
    CXS::Factory *factory = CXS::Factory::getFactoryInstanceByName("SSC339G");
    CXS::Factory *factory_common = CXS::Factory::getFactoryInstanceByName("common");
    cf_assert(factory != nullptr && factory_common != nullptr);

    CXS::Element *mElemHead = factory->createElementByName("vif");
    elemVps = factory->createElementByName("vpe");
    elemVenc0 = factory->createElementByName("venc");
    rtsp_server0 = factory_common->createElementByName("rtsp-server");

    cf_assert(rtsp_server0 != nullptr);
    rtsp_server0->setAttr("port", 554);
    // std::string payload = elemVenc0->getAttr("VENC-PAYLOAD","");
    rtsp_server0->setAttr("payload", "h264");

    mElemHead->setAttr("eSnrPad", "0");
    mElemHead->setAttr("vifDev", "0");
    mElemHead->setAttr("u32InputPort", "0");
    mElemHead->setAttr("vencChn", "0");
    mElemHead->setAttr("resolution", "4K");
    mElemHead->setAttr("mode", "regroup");

    dst_horizontal_blks = 8;
    dst_vertical_blks = 8;
    std::string horizontal = to_String(dst_horizontal_blks);
    mElemHead->setAttr("dst_horizontal_blks", horizontal);
    std::string vertical = to_String(dst_vertical_blks);
    mElemHead->setAttr("dst_vertical_blks", vertical);

    mElemHead->setAttr("layNum", "64");

    for (int i = 0; i < MAX_LAYER_NUMS; i++)
    {
        layer_layout_tab[i] = 63 - i;
        if (layer_layout_tab[i] != -1)
        {
            std::string tabId = "tabId";
            std::string layer_tab = to_String(layer_layout_tab[i]);
            std::string tab = tabId + to_String(i);
            mElemHead->setAttr(tab.c_str(), layer_tab);
        }
        else
        {
            std::string layNum = to_String(i + 1);

            break;
        }
    }
    elemVps->setAttr("vpeChn", "0");
    elemVps->setAttr("vencChn", "0");
    elemVps->setAttr("pixelFormat", "35");
    elemVps->setAttr("sensorId", "0");
    elemVps->setAttr("resolution", "4K");
    elemVps->setAttr("scale", "FHD");
    mElemHead->link(elemVps); // vif ->vep

    // elemVps->setAttr("ReGropMode", "No-Regroup");

    elemVenc0->setAttr("vencChn", "0");
    elemVenc0->setAttr("resolution", "FHD");
    elemVenc0->setAttr("compressionType", "H264");
    elemVenc0->setAttr("frameData", to_String(nova_cfg.rateCtlData.FrameRate));
    elemVenc0->setAttr("biteRate", to_String(nova_cfg.rateCtlData.BitRate));
    elemVenc0->setAttr("Gop", to_String(nova_cfg.gopData.lenght));

    elemVps->link(elemVenc0); //vep -> venc

    elemVenc0->link(rtsp_server0); //venc -> rtsp

    // ws_server0 = factory_common->createElementByName("ws-server");
    // cf_assert(ws_server0 != nullptr );
    // ws_server0->setAttr("port",port1);
    // elemVenc0->link(ws_server0);
    mElemHead->mNextElems.push_back(elemVenc0);
    mElemHead->mNextElems.push_back(rtsp_server0);

    return mElemHead;
}

int Player::start()
{
    int res = 0;
    // if(elemVenc0 )
    //     elemVenc0->start();
    struct Buffer data;

    if (mElemHead1)
    {
        res = mElemHead1->startSelf();
        printf("mElemHead1->mNextElems[0] size : %d \n", mElemHead1->mNextElems.size());
        printf(" mElemHead1->mNextElems[0] name : %s \n", mElemHead1->mNextElems[0]->getClassName());
        printf(" mElemHead1->mNextElems[1] name : %s \n", mElemHead1->mNextElems[1]->getClassName());
        printf(" mElemHead1->mNextElems[2] name : %s \n", mElemHead1->mNextElems[2]->getClassName());
        mElemHead1->mNextElems[0]->startSelf();
        mElemHead1->mNextElems[1]->startSelfData(RTSPCallBack_0);
        // mElemHead1->mNextElems[1]->startSelf();
        mElemHead1->mNextElems[2]->startSelf();
        RTSPElement_0 = mElemHead1->mNextElems[2];
        mElemHead1->mNextElems[1]->linkTo(mElemHead1->mNextElems[2]);
        mElemHead1->mNextElems[0]->linkTo(mElemHead1->mNextElems[1]);
        mElemHead1->linkTo(mElemHead1->mNextElems[0]);

        // getchar();
        // mElemHead1->mNextElems[1]->getData(&data);
        // printf("data len : %d \n", data.len);
    }

    // if (res == 0 && mElemHead2 && mElemHead1 != mElemHead2)
    {
        res = mElemHead2->startSelf();
        printf("mElemHead1->mNextElems[0] size : %d \n", mElemHead2->mNextElems.size());
        printf(" mElemHead1->mNextElems[0] name : %s \n", mElemHead2->mNextElems[0]->getClassName());
        printf(" mElemHead1->mNextElems[1] name : %s \n", mElemHead2->mNextElems[1]->getClassName());
        printf(" mElemHead1->mNextElems[2] name : %s \n", mElemHead2->mNextElems[2]->getClassName());
        mElemHead2->mNextElems[0]->startSelf();
        mElemHead2->mNextElems[1]->startSelfData(RTSPCallBack_1);
        // mElemHead2->mNextElems[1]->startSelf();

        mElemHead2->mNextElems[2]->startSelf();
        RTSPElement_1 = mElemHead2->mNextElems[2];
        mElemHead2->mNextElems[1]->linkTo(mElemHead1->mNextElems[2]);
        mElemHead2->mNextElems[0]->linkTo(mElemHead1->mNextElems[1]);
        mElemHead2->linkTo(mElemHead1->mNextElems[0]);
    }
    // res = mElemHead2->start();

    return res;
}
