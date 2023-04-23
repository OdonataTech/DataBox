/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#include "Utilites.h"
#include "DataSetBase.h"
#include <stdio.h>
#include <stdarg.h>


// ********************************************s********************************
// TextAsUTF8String()  convert fmx::Text to std::string
// ****************************************************************************
std::string TextAsUTF8String ( const fmx::Text& fmx_text )
{
    
    std::string result;
    
    FMX_UInt32 text_size = ( 4 * ( fmx_text.GetSize() ) ) + 1;
    char * text = new char [ text_size ]();
    fmx_text.GetBytes ( text, text_size, 0, (FMX_UInt32)fmx::Text::kSize_End, fmx::Text::kEncoding_UTF8 );
    result.assign ( text );
    delete [] text;
    
    return result;
    
}; // TextAsString


// ****************************************************************************
// stringToVector() Split string to vector by delimeter
// ****************************************************************************
unsigned long stringToVector(const std::string &str, vector<string> &dest, std::string delimeter){
    if (str.empty()) return 0;
    
    if (delimeter.empty()) {
        dest.push_back(str);
        
    }else{
        size_t pos1 = 0, pos2 = 0;
        std::string el;
        while (pos2 != std::string::npos) {
            pos2 = str.find(delimeter, pos1);
            el = str.substr(pos1, pos2 - pos1);
            dest.push_back(el);
            pos1 = pos2+ delimeter.length();
        }
    }
    
    return dest.size();
}; // stringToVector



// ****************************************************************************
// stringToElementsPath() Split element's path string to deque elementAddress
// ****************************************************************************
int stringToElementsPath(const string &str, deque<dataSetPath> &path ){
    if (str.empty())
        return -2;

    size_t end = str.find("[", 0);
    string delimetr = "$";
    size_t pos1 = 0, pos2 = 0;
    while (pos2 < end) {
        pos2 = str.find(delimetr, pos1);
        if (pos2 == string::npos) pos2 = end;
        dataSetPath el(str.substr(pos1, pos2 - pos1));
        path.push_back(el);
        pos1 = pos2+ delimetr.length();
    }
    
    unsigned long idx = 0;
    size_t idxstart = str.find("[", 0);
    size_t idxend = str.find("]", 0);
    if ( idxstart != string::npos && idxend != string::npos ) {
        string idxstr = str.substr(idxstart+1, idxend-1);
        idx = (unsigned long)atol(idxstr.c_str());
        if (idx > 0) {
            path[path.size()-1].index = idx-1;
            path[path.size()-1].signIndex = 1;
        }else{
            return -1;
        }
    }
    
    return  1;
};

// ********************************************************************************************
// dataSetTypeFromStr()
// ********************************************************************************************
int dataSetTypeFromStr(string typestr){
    if(typestr == "number") return dataTypeNumber ;
    else if(typestr == "text") return dataTypeText ;
    else if(typestr == "date") return dataTypeDate ;
    else if(typestr == "time") return dataTypeTime ;
    else if(typestr == "timestamp") return dataTypeTimestamp ;
    else if(typestr == "container") return dataTypeContainer ;
    else if(typestr == "dataset") return dataTypeElement ;
    else return 0;
}


// ********************************************************************************************
// dataSetTypeFromNum()
// ********************************************************************************************
string dataSetTypeFromNum(int typenum){
    switch (typenum) {
        case dataTypeNumber: return "number";
        case dataTypeText: return "text";
        case dataTypeDate: return "date";
        case dataTypeTime: return "time";
        case dataTypeTimestamp: return "timestamp";
        case dataTypeContainer: return "container";
        case dataTypeElement: return "dataset";
    }
    return "";
}

// ********************************************************************************************
// substitute() replce all subsrings in string by value
// ********************************************************************************************
long substitute( string &str, const string &sub, const string &value ){
    
    if (str.empty()) return 0;
    
    size_t pos = str.find(sub);
    while (pos != string::npos) {
        str.replace(pos, sub.length() , value);
        pos = str.find(sub);
    }
    return 1;
};


// ********************************************************************************************
// simpleFormat() get string by format like printf
// ********************************************************************************************
string simpleFormat( string format, double value ){
    
    int bufSize = snprintf(NULL, 0, format.c_str(), value);
    char buf[bufSize+1];
    snprintf(buf, sizeof(buf), format.c_str(), value);
    std::string str = "";
    
    return buf;
}


std::random_device rd; // random device engine, usually based on /dev/random on UNIX-like systems
// initialize Mersennes' twister using rd to generate the seed
std::mt19937 rng{rd()};

int mersenne_rand_int()
{
    static std::uniform_int_distribution<int> uid(1,32767); // random dice
    return uid(rng); // use rng as a generator
}
