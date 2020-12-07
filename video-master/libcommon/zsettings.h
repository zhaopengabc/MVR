#ifndef ZSETTINGS_H
#define ZSETTINGS_H
#include<fstream>
#include<sstream>
#include<iostream>
#include<sstream>
#include <map>
#include <unistd.h>


class ZSettings{
    std::map<std::string,std::string> m_cxt;
    std::string m_file_path;
    bool m_update;
public:
    ZSettings(const char* file_path):m_file_path(file_path),m_update(false){
        std::ifstream  ifs;
        ifs.open (file_path, std::ios::in );
        if(ifs.is_open()){
            char buff[1024];
            while(ifs.getline(buff,sizeof(buff)))
            {
                std::string str(buff);
                size_t pos = str.find('=');
                if(pos == std::string::npos)
                    continue;

                std::string k = str.substr(0,pos);
                std::string v = str.substr(pos+1);
                printf("parse %s k=%s,v=%s\n",buff,k.data(),v.data());
                m_cxt[k] = v;


            }
            ifs.close();
        }
    }

    std::string getString(const char* key,const std::string& default_value = ""){
        if(m_cxt.count(key) == 0)
            return default_value;
        else
            return m_cxt[key];
    }
    bool getBoolean(const char* key,bool default_value = false){
        if(m_cxt.count(key) == 0)
            return default_value;
        else
            return m_cxt[key] == "true" ? true : false;
    }

    void setValue(const char* key,const char* val ){
        m_update = true;

        m_cxt[key] = val;
    }

    void setValue(const char* key,const std::string& val ){
        m_update = true;

        m_cxt[key] = val;
    }

    void setValue(const char* key, bool val ){
        m_update = true;

        m_cxt[key] = val == true ? "true":"false";
    }

    void sync(){
        std::ofstream  ofs;
        if(m_update == false)
            return;
        ofs.open (m_file_path, std::ios::in | std::ios::trunc );

        if(ofs.is_open())
        {
            for(auto it = m_cxt.begin();it != m_cxt.end();it++)
            {

                std::string line =  it->first+"="+it->second;
                ofs << line << std::endl;
            }
            ofs.close();
            ::sync();
        }
        else
        {
            std::cout<<"write "+m_file_path+" file failure!"<<std::endl;
        }
    }
};
#endif // ZSETTINGS_H
