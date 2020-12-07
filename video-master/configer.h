#ifndef CONFIGER_H
#define CONFIGER_H
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
class Configer{
    std::string mFileName;
    boost::property_tree::ptree mPtree;
public :
    Configer(const char* fileName):mFileName(fileName) {
        try{
            boost::property_tree::json_parser::read_json(mFileName, mPtree);
        }
        catch(...){
        }
    }

    boost::property_tree::ptree& json(){
            return mPtree;
    }
    void save(void){
        boost::property_tree::json_parser::write_json(mFileName,mPtree);
    }

};
#endif // CONFIGER_H
