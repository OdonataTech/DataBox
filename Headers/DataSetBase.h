/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#ifndef DataSetBase_h
#define DataSetBase_h

#include "DataBox.h"
#include "dbDataTime.h"
#include "Utilites.h"


using namespace std;

// ******************************************************************************************
// dataSet Types
// ******************************************************************************************
enum dataSetTypes {
    dataTypeElement = 100,
    dataTypeNumber = 1,
    dataTypeText = 2,
    dataTypeDate = 3,
    dataTypeTime = 4,
    dataTypeTimestamp = 5,
    dataTypeContainer = 6
};

int dataSetTypeFromStr(string typestr);
string dataSetTypeFromNum(int typenum);


// ********************************************************************************************
// DATASET CLASSES
// main classes for store user's data
//
// dataSetBase Bases class with virtual functions
// ********************************************************************************************
class dataSetBase {
protected:
    
    unsigned long size;
    int type;
    string name;
    
    vector<unsigned long> order;
    
public:
    
    dataSetBase(){
        size = 0;
    }
    
    virtual ~dataSetBase(){}
    
    unsigned long lenght(){ return size; }
    string getName(){ return name;}
    int getType(){ return type; }
    void rename(string newName){name = newName; }
    
    bool operator == (string nameToCompare){
        return (name == nameToCompare);
    }
    
    unsigned long getRec(unsigned long idx){
        return order[idx];
    }
    
    long getIdx(unsigned long idx){
        
        auto itr = std::find( order.begin(), order.end(), idx );
        if (itr == order.end()) {
            return -1;
        } else {
            return distance( order.begin(), itr) ;
        }
        
    }
    
    string dysplayOrder(){
        if ( size==0 ) return "";
        ostringstream ost;
        for (unsigned long i=0; i< size; ++i) {
            ost << order[i] ;
            if (i < size) ost << "\n";
        }
       return ost.str();
    }
    
    vector<unsigned long> getOrder(){
       return order;
    }
    
    unsigned long setOrder( vector<unsigned long> &newOrder ){
        unsigned long sz;
        if ( size < newOrder.size() ) sz = size;
        else sz = newOrder.size();
        for (unsigned long i=0; i<sz; ++i) {
            order[i] = newOrder[i];
        }
        return sz;
    }
    
    dataSetBase* getELementPtr() {return this;}
    virtual dataSetBase* getELementPtrByPath (deque<dataSetPath> path );
    virtual dataSetBase* getELementPtrByIdx (unsigned long idx) {return this; }
    
    virtual int pushback(double element) = 0;
    virtual int pushback(string element) = 0;
    virtual int pushback(unique_ptr<dataSetBase> element) = 0;
    virtual int pushback(dbDataTime element) = 0;
    
    virtual dataSetBase* addElement(string elementName, int elementType) = 0;
    
    virtual int setValue(double element, unsigned long index )= 0;
    virtual int setValue(string element, unsigned long index ) = 0;
    virtual int setValue(dbDataTime element, unsigned long index ) = 0;
    
    virtual unsigned long addFromString (const string& str, string delimetr, string mask ) = 0;
    virtual unsigned long addFromString (const string& str, string delimetr ) = 0;
    
    virtual int getValue(double &el, unsigned long index) = 0;
    virtual int getValue(string &el, unsigned long index) = 0;
    virtual int getValue(dbDataTime &el, unsigned long index) = 0;
    
    virtual string getValueAsString(unsigned long index, string mask) = 0;
    
    virtual string dysplay(unsigned long start, unsigned long end, string delimetr) = 0;
    virtual string dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn ) = 0;
	virtual string saveTableToFile(string filePath, unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn ) ;
    virtual string dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level = 0 ) = 0;
    
    virtual long find(string value) = 0;
    
    virtual int deleteByIndex( unsigned long index, int releasePtrSign=0 ) = 0;
    virtual int deleteByName( string name, int releasePtrSign=0 ) = 0;
    
    virtual  long  getIdxByValue( double value,  long start ) = 0;
    virtual  long  getIdxByValue( string value,  long start ) = 0;
    virtual  long  getIdxByValue( dbDataTime value,  long start ) = 0;
    
    virtual long uniq(dataSetBase *ds ) = 0;
    virtual long sort(int direction) = 0 ;
    virtual long sort(string bycolumns, string direction) { return sort ( atoi(direction.c_str()) ); }
    virtual long group(string column, dataSetBase *newDS  ) = 0 ;
    
    virtual double sum(unsigned long start, unsigned long end) = 0;
    virtual double max(unsigned long start, unsigned long end) = 0;
    virtual double min(unsigned long start, unsigned long end) = 0;
    
    virtual dbDataTime* maxDataTime(unsigned long start, unsigned long end) = 0;
    virtual dbDataTime* minDataTime(unsigned long start, unsigned long end) = 0;
    
    virtual string expressionPrepare (const string & expression, unsigned long index) = 0;
    
    virtual string getValueAsCell(unsigned long index, map<string, string>& parameters, string &prefix, string &prefixRow, string jsID, int colNum, dataSetBase *parentPtr );
    virtual string getAsHTMLTable(unsigned long start, unsigned long end, map<string, string>& parametersMap, string &prefix, string &prefixRow, string &css, string &js);

};









#endif // DataSetBase_h
