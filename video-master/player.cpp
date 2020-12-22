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

Player::Player()
{
    mElemHead1 = nullptr;
    mElemHead2 = nullptr;
}
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
Player *Player::getInstance(int chn1, int chn2, uint16_t port1, uint16_t port2, uint8_t protocol)
{
    static Player *instance = nullptr;
    for (int i = 0; i < MAX_LAYER_NUMS; i++)
    {
        layer_layout_tab[i] = i;
    }
    read_layout_conf();

    if (!instance)
    {
        cf_threadpool_run(ws_server_run, nullptr);
        CXS::Factory *factory = CXS::Factory::getFactoryInstanceByName("SSC339G");
        CXS::Factory *factory_common = CXS::Factory::getFactoryInstanceByName("common");
        cf_assert(factory != nullptr && factory_common != nullptr);
        instance = new Player;

        if (1 && chn1 >= 0)
        {

            instance->mElemHead1 = factory->createElementByName("vif");
            elemVps = factory->createElementByName("vpe");
            elemVenc0 = factory->createElementByName("venc");
            rtsp_server0 = factory_common->createElementByName("rtsp-server");

            cf_assert(rtsp_server0 != nullptr);
            rtsp_server0->setAttr("port", 554);
            // std::string payload = elemVenc0->getAttr("VENC-PAYLOAD","");
            rtsp_server0->setAttr("payload", "h264");

            instance->mElemHead1->setAttr("eSnrPad", "0");
            instance->mElemHead1->setAttr("vifDev", "0");
            instance->mElemHead1->setAttr("u32InputPort", "0");
            instance->mElemHead1->setAttr("vencChn", "0");
            instance->mElemHead1->setAttr("resolution", "4K");
            instance->mElemHead1->setAttr("mode", "regroup");

            dst_horizontal_blks = 8;
            dst_vertical_blks = 8;
            std::string horizontal = to_String(dst_horizontal_blks);
            instance->mElemHead1->setAttr("dst_horizontal_blks", horizontal);
            std::string vertical = to_String(dst_vertical_blks);
            instance->mElemHead1->setAttr("dst_vertical_blks", vertical);

            instance->mElemHead1->setAttr("layNum", "64");

            for (int i = 0; i < MAX_LAYER_NUMS; i++)
            {
                layer_layout_tab[i] = 63-i;
                if (layer_layout_tab[i] != -1)
                {
                    std::string tabId = "tabId";
                    std::string layer_tab = to_String(layer_layout_tab[i]);
                    std::string tab = tabId + to_String(i);
                    instance->mElemHead1->setAttr(tab.c_str(), layer_tab);
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
            instance->mElemHead1->link(elemVps); // vif ->vep


            // elemVps->setAttr("ReGropMode", "No-Regroup");

            elemVenc0->setAttr("vencChn", "0");
            elemVenc0->setAttr("resolution", "4K");
            elemVenc0->setAttr("compressionType", "H264");

            elemVps->link(elemVenc0); //vep -> venc


            elemVenc0->link(rtsp_server0); //venc -> rtsp

            // ws_server0 = factory_common->createElementByName("ws-server");
            // cf_assert(ws_server0 != nullptr );
            // ws_server0->setAttr("port",port1);
            // elemVenc0->link(ws_server0);

            instance->mElemHead2 = factory->createElementByName("vif");
            elemVpe1 = factory->createElementByName("vpe");
            elemVenc1 = factory->createElementByName("venc");
            rtsp_server1 = factory_common->createElementByName("rtsp-server");
            rtsp_server1->setAttr("port", 555);
            rtsp_server1->setAttr("payload", "h264");

            instance->mElemHead2->setAttr("eSnrPad", "1");
            instance->mElemHead2->setAttr("vifDev", "2");
            instance->mElemHead2->setAttr("u32InputPort", "0");
            instance->mElemHead2->setAttr("vencChn", "2");
            instance->mElemHead2->setAttr("resolution", "HD");
            instance->mElemHead2->link(elemVpe1); // vif ->vep

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
            elemVenc1->link(rtsp_server1); //venc -> rtsp

            // ws_server1 = factory_common->createElementByName("ws-server");
            // cf_assert(ws_server0 != nullptr );
            // ws_server1->setAttr("port",port1);
            // elemVenc1->link(ws_server1);

            // elemVenc0->pushData(&dstFrame);
            /*
            cf_assert(instance->mElemHead1 != nullptr );
            instance->mElemHead1->setAttr("VI-CHN",chn1*8);
            // instance->mElemHead1->setAttr("ON-NEW-FRAME",(uint64_t)new_frame_handler);
#if WORK_MODE0   == WORK_MODE_HD
            instance->mElemHead1->setAttr("VI-MODE","HD1080");
#elif WORK_MODE0   == WORK_MODE_4K
            instance->mElemHead1->setAttr("VI-MODE","UHD4K");
#endif



            elemVenc0 =  factory->createElementByName("venc");
            cf_assert(elemVenc0 != nullptr );
            elemVenc0->setAttr("VENC-CHN",0);
#if WORK_MODE0   == WORK_MODE_HD
            elemVenc0->setAttr("VENC-PAYLOAD","h264");
            elemVenc0->setAttr("VENC-SRC-FRAMERATE",60);
            elemVenc0->setAttr("VENC-MODE","HD1080");//HD720 HD1080 UHD4K
#elif WORK_MODE0   == WORK_MODE_4K
            elemVenc0->setAttr("VENC-PAYLOAD","h264");
            elemVenc0->setAttr("VENC-SRC-FRAMERATE",15);
            global.conf.json().get("mvr_mode",std::string("unfixed")) == "fixed")
                elemVenc0->setAttr("VENC-MODE","UHD1920X2160");//HD720 HD1080 WQXGA UHD1920X2160 UHD4K
            else
                elemVenc0->setAttr("VENC-MODE","HD1080");//HD720 HD1080 WQXGA UHD1920X2160 UHD4K
#endif
            elemVenc0->setAttr("VENC-FRAMERATE",15);//MIN_FRAME_RATE,MAX_FRAME_RATE

            if(1 && protocol == 0) // websocket
            {
                ws_server0 = factory_common->createElementByName("ws-server");
                cf_assert(ws_server0 != nullptr );
                ws_server0->setAttr("port",port1);
                elemVenc0->link(ws_server0);
            }
            if(1)
            {
                rtsp_server0 = factory_common->createElementByName("rtsp-server");
                cf_assert(rtsp_server0 != nullptr );
                rtsp_server0->setAttr("port",port1+10);
                std::string payload = elemVenc0->getAttr("VENC-PAYLOAD","");
                rtsp_server0->setAttr("payload",payload.data());
                elemVenc0->link(rtsp_server0);
            }
        }

        if(1 && chn2 >= 0){
            if(chn1 == chn2)
                instance->mElemHead2 = instance->mElemHead1;
            else{
               instance->mElemHead2 =  factory->createElementByName("vi");
               
               cf_assert(instance->mElemHead2 != nullptr );
               instance->mElemHead2->setAttr("VI-CHN",chn2*8);
#if WORK_MODE1   == WORK_MODE_HD
               instance->mElemHead2->setAttr("VI-MODE","HD1080");
#elif WORK_MODE1   == WORK_MODE_4K
               instance->mElemHead2->setAttr("VI-MODE","UHD4K");
#endif
            }

            elemVenc1 =  factory->createElementByName("venc");
            cf_assert(elemVenc1 != nullptr );
            elemVenc1->setAttr("VENC-CHN",1);
#if WORK_MODE1   == WORK_MODE_HD
            elemVenc1->setAttr("VENC-PAYLOAD","h264");
            elemVenc1->setAttr("VENC-MODE","HD720");
#elif WORK_MODE1   == WORK_MODE_4K
            elemVenc1->setAttr("VENC-PAYLOAD","h265");
            elemVenc1->setAttr("VENC-MODE","UHD4K");
#endif

            instance->mElemHead2->link(elemVenc1);
            if(protocol == 0) // websocket
            {
                ws_server1 = factory_common->createElementByName("ws-server");
                cf_assert(ws_server1 != nullptr );
                ws_server1->setAttr("port",port2);
                elemVenc1->link(ws_server1);
                ws_server1->setAttr("max_cli_pending_changed",(uint64_t)ws_cli_max_pending_changed_handler);
                ws_server1->setAttr("max_cli_pending_user_data",(uint64_t)elemVenc1);
            }
            if(1)
            {
                rtsp_server1 = factory_common->createElementByName("rtsp-server");
                cf_assert(rtsp_server1 != nullptr );
                rtsp_server1->setAttr("port",port2+10);
                std::string payload = elemVenc1->getAttr("VENC-PAYLOAD","");
                rtsp_server1->setAttr("payload",payload.data());
                elemVenc1->link(rtsp_server1);
            }
        }
        char cmd[1024];
        //if(isH15()){
        if(global.conf.json().get("mvr_mode",std::string("unfixed")) == "fixed"){
            system("sed -i 's/var row = .*;/var row = 16;/' ./mount-origin/static/video/test_zx.html");
            system("sed -i 's/vm1.setFrameSize(.*);/vm1.setFrameSize(1920,2160);/' ./mount-origin/static/video/test_zx.html");

        }
        else
        {
            system("sed -i 's/var row = .*;/var row = 8;/' ./mount-origin/static/video/test_zx.html");
            system("sed -i 's/vm1.setFrameSize(.*);/vm1.setFrameSize(1920,1080);/' ./mount-origin/static/video/test_zx.html");
        }
    */

            // //sprintf(cmd,"sed -i 's/var vm1.*/var vm1 = new VideoModule(document.domain,%d,8,8,1920,1080,0,  1 )/' ./mount-origin/static/video/test_zx.html",port1);
            // sprintf(cmd,"sed -i 's/vm1.openService*;/vm1.openService(document.domain, %d, 8, 8);/' ./mount-origin/static/video/test_zx.html",port1);
            // system(cmd);
            // //sprintf(cmd,"sed -i 's/var vm2.*/var vm2 = new VideoModule(document.domain,%d,1,1,1280,720,0,1)/' ./mount-origin/static/video/test_zx.html",port2);
            // sprintf(cmd,"sed -i 's/vm2.openService*;/vm2.openService(document.domain, %d, 1, 1);/' ./mount-origin/static/video/test_zx.html",port2);
            // system(cmd);
        }
    }
    return instance;
}
int Player::start()
{
    int res = 0;
    // if(elemVenc0 )
    //     elemVenc0->start();

    if (mElemHead1)
        res = mElemHead1->start();
    if (res == 0 && mElemHead2 && mElemHead1 != mElemHead2)
        // if(mElemHead2)
        res = mElemHead2->start();

    return res;
}
