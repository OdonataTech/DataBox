/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */


#ifndef Utilites_h
#define Utilites_h

#include "DataBox.h"


using namespace std;

// ******************************************************************************************
// dataSet Path.  deque<dataSetPath> use for store path to dataSet element
// ******************************************************************************************
struct dataSetPath {
    string name;
    unsigned long index;
    int signIndex;
    
    dataSetPath( string nm ){
        name = nm;
        index = 0 ;
        signIndex = 0;
    };
};


std::string TextAsUTF8String ( const fmx::Text& fmx_text );

unsigned long stringToVector(const string &str, vector<string> &dest, string delimetr );

int stringToElementsPath(const string &str, deque<dataSetPath> &path );

long substitute( string &str, const string &sub, const string &value );

string simpleFormat( string format, double value );

int mersenne_rand_int();








// ******************************************************************************************
//  FSM parameters proceccing
// ******************************************************************************************
enum fsmParamsState {
    waitFirstKeySymbol = 1,
    waitNextKeySymbol = 2,
    foundSpaceWaitEqual = 3,
    foundEqualWaitApostrophe = 4,
    foundAapostropheWaitNextValueSymbol = 5,
    foundAapostropheWaitSemicolon = 6
    
};


class fsmParams {
protected:
    int state;
    string buferKey, buferValue, currentSymbol, previousSymbol;
    
    int _waitFirstKeySymbol(){
        //ignnored symbol
        if (currentSymbol == " " || currentSymbol == "\n" || currentSymbol == "\t") return 1;
        
        // unallowable symbol
        if (currentSymbol == ":" || currentSymbol == "=" || currentSymbol == ";" || currentSymbol == "'" || currentSymbol == "\"") return -1;
        
        //find first symbol of key
        buferKey = buferKey + currentSymbol;
        state = waitNextKeySymbol;
        return 1;
    }
    
    int _waitNextKeySymbol(){
        //ignnored symbol
        // none
        
        // unallowable symbol
        if ( currentSymbol == ";" || currentSymbol == "'" || currentSymbol == "\"") return -1;
        
        if ( currentSymbol == " "){
            state = foundSpaceWaitEqual;
            return 1;
        }
        if ( currentSymbol == "="){
            state = foundEqualWaitApostrophe;
            return 1;
        }
        //continue and store symbols to key
        buferKey = buferKey + currentSymbol;
        return 1;
    }
    
    int _foundSpaceWaitEqual(){
        //ignnored symbol
        if (currentSymbol == " " || currentSymbol == "\n" || currentSymbol == "\t") return 1;
        
        // unallowable symbol
        if (currentSymbol != "=" ) return -1;
        
        //find = symbol
        state = foundEqualWaitApostrophe;
        return 1;
    }
    
    int _foundEqualWaitApostrophe(){
        //ignnored symbol
        if (currentSymbol == " " || currentSymbol == "\n" || currentSymbol == "\t") return 1;
        
        // unallowable symbol
        if (currentSymbol != "'" ) return -1;
        
        //find opened ' symbol
        state = foundAapostropheWaitNextValueSymbol;
        return 1;
    }
    
    int _foundAapostropheWaitNextValueSymbol(){
        //ignnored symbol
        // none
        
        //find closed ' symbol
        if (currentSymbol == "'" && previousSymbol != "\\" ) {
            state = foundAapostropheWaitSemicolon;
            return 1;
        }
        
        //continue and store symbols to value
        buferValue = buferValue + currentSymbol;
        return 1;
    }
    
    int _foundAapostropheWaitSemicolon(){
        //ignnored symbol
        if (currentSymbol == " " || currentSymbol == "\n" || currentSymbol == "\t") return 1;
        
        // unallowable symbol
        if (currentSymbol != ";" ) return -1;
        
        //find ; symbol, save parameters and swith to new loop
        parametersMap.insert(pair<string, string>(buferKey, buferValue) );
        buferKey = "";
        buferValue = "";
        state = waitFirstKeySymbol;
        return 1;
    }
    
    
public:
    map<string, string> parametersMap;
    
    long setParametersMap(string &paramsString ){
        
        if( paramsString.empty() ) return 0;
        
        state = waitFirstKeySymbol;
        buferKey = "";
        buferValue = "";
        currentSymbol = "";
        previousSymbol = "";
        unsigned long i=0;
        int res = 1;
        
        while (i < paramsString.length() && res == 1) {
            previousSymbol = currentSymbol;
            currentSymbol = paramsString.substr(i, 1);
            switch (state) {
                case waitFirstKeySymbol:
                    res = _waitFirstKeySymbol();
                    break;
                case waitNextKeySymbol:
                    res = _waitNextKeySymbol();
                    break;
                case foundSpaceWaitEqual:
                    res = _foundSpaceWaitEqual();
                    break;
                case foundEqualWaitApostrophe:
                    res = _foundEqualWaitApostrophe();
                    break;
                case foundAapostropheWaitNextValueSymbol:
                    res = _foundAapostropheWaitNextValueSymbol();
                    break;
                case foundAapostropheWaitSemicolon:
                    res = _foundAapostropheWaitSemicolon();
                    break;
                default:
                    break;
            }
            ++i;
        }
        
        if (res == 1)
            return (long)i;
        else
            return res;
    }
    
};




#endif /* Utilites_hpp */

