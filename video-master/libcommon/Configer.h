#ifndef CONFIGER_H
#define CONFIGER_H

#include<fstream>
#include<sstream>
#include<iostream>
#include<sstream>
#include "json_util.h"
#include <stdio.h>
#define VIDEO_CONFIG    "/tmp/video_configs.json"
class Configer{
    Json::Value mCxt;
    std::string m_fileName;
public:
    Configer(const char* fileName){

        std::ifstream  ifs;

        m_fileName = fileName;
        ifs.open (m_fileName, std::ios::in );
        if(!ifs.is_open())
            return;
        std::ostringstream oss;

        oss << ifs.rdbuf();
        ifs.close();

        Json::CharReaderBuilder readerBuilder;
        std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
        if (!JsonUtil::ParseJson(oss.str().c_str(),&mCxt))
        {
            std::cout << "parse " << oss.str() << " err."<<std::endl;
        }

    }

        static Configer* const getVideoInstance(){
            static Configer* const m = new Configer(VIDEO_CONFIG);
            return m;
        }

        int sync(){
            std::ofstream  ofs;
            ofs.open (m_fileName, std::ios::in | std::ios::trunc );
            std::string jsonStr;
            std::ostringstream os;
            Json::StreamWriterBuilder writerBuilder;
            std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
            JsonUtil::PrintJson(mCxt);
            jsonWriter->write(mCxt, &os);
            jsonStr = os.str();
            ofs<<jsonStr;
            ofs.close();
            return 0;

        }
        const Json::Value getParam(const char* paramName){
            if(!mCxt.isMember(paramName))
            {
                mCxt[paramName] = Json::Value();
            }
            return mCxt[paramName];
        }
        void clear()
        {
            mCxt.clear();
        }
        const void setParam(const char* paramName,Json::Value val){
            if( mCxt[paramName] != val ){
                mCxt[paramName] = val;
                sync();
            }

        }
};
#endif //CONFIGER_H
