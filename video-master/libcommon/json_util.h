#ifndef JSON_UTIL_H
#define JSON_UTIL_H
#include"json/json.h"
#include <string.h>
#include <memory>
#include <iostream>
#include <sstream>
class JsonUtil{
public:
    static bool ParseJson(const char*str,Json::Value* root){
        Json::CharReaderBuilder readerBuilder;
        JSONCPP_STRING errs;
        bool res;
        std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
        res = reader->parse(str,str+strlen(str), root,&errs);
        if (!res || !errs.empty())
        {
            std::cout << "parseJson err: "<<str<<"(" << errs <<")."<< std::endl;
        }
        return res;
    }
    static std::string JsonToString(const Json::Value& jsonObj){
        std::ostringstream os;
        Json::StreamWriterBuilder writerBuilder;
        std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
        jsonWriter->write(jsonObj, &os);

        return os.str();
    }
    static void PrintJson(const Json::Value& root){
        std::cout<< "json: "<<root.toStyledString()<<std::endl;
    }
};



#endif // JSON_UTIL_H
