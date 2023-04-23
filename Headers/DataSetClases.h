/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#ifndef DataSetCLases_h
#define DataSetCLases_h

#include "DataBox.h"
#include "Utilites.h"
#include "DataSetBase.h"
#include "dbDataTime.h"

using namespace std;



// ********************************************************************************************
// dataSetElement Class for store vector of other same objects
// ********************************************************************************************
class dataSetElement : public dataSetBase {
private:
    vector<unique_ptr<dataSetBase>> data;

public:
    
    dataSetElement(string elementName){
        name = elementName;
        type = dataTypeElement;
        size = 0;
    }
    
    ~dataSetElement(){}
    
    int pushback(double element){return 0;}
    int pushback(string element){return 0;}
    int pushback(unique_ptr<dataSetBase> element);
    int pushback(dbDataTime element){return 0;}
    
    dataSetBase* addElement(string elementName, int elementType);
    
    int setValue(double element, unsigned long index ){return 0;}
    int setValue(string element, unsigned long index ){return 0;}
    int setValue(dbDataTime element, unsigned long index ){return 0;}

    dataSetBase* getELementPtrByPath (deque<dataSetPath> path );
    dataSetBase* getELementPtrByIdx (unsigned long idx);
    
    int getValue(double &el, unsigned long index) {return 0;}
    int getValue(string &el, unsigned long index);
    int getValue(dbDataTime &el, unsigned long index) {return 0;}
    
    string getValueAsString(unsigned long index, string mask){ return dysplay(index, index, "\n");  };
    
    unsigned long addFromString (const string& str, string delimetr ) {return 0;}
    unsigned long addFromString (const string& str, string delimetr, string mask ){return 0;}

    
    string dysplay(unsigned long start, unsigned long end, string delimetr);
    string dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn);
	string saveTableToFile(string filePath, unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn );
    string dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level = 0);
    
    long find(string value);
    int deleteByIndex( unsigned long index, int releasePtrSign=0 );
    int deleteByName( string name, int releasePtrSign=0 );
    
    long  getIdxByValue( double value,  long start ) {return 0;}
    long  getIdxByValue( string value,  long start ) ;
    long  getIdxByValue( dbDataTime value,  long start ) {return 0;}
    
    long uniq(dataSetBase *ds ) {return 0;}
    long sort(int direction) ;
    long sort(string bycolumns, string direction) ;
    long group(string column, dataSetBase *newDS  ) ;

        
    double sum(unsigned long start, unsigned long end)  {return 0;}
    double max(unsigned long start, unsigned long end) {return 0;}
    double min(unsigned long start, unsigned long end) {return 0;}
    
    dbDataTime* maxDataTime(unsigned long start, unsigned long end) {return nullptr;}
    dbDataTime* minDataTime(unsigned long start, unsigned long end) {return nullptr;}
    
    string expressionPrepare (const string & expression, unsigned long index);
    
    string getValueAsCell(unsigned long index, map<string, string>& parameters, string &prefix, string &prefixRow, string jsID, int colNum, dataSetBase *parentPtr );
    string getAsHTMLTable(unsigned long start, unsigned long end, map<string, string>& parametersMap, string &prefix, string &prefixRow, string &css, string &js);


};



// ********************************************************************************************
// dataSetNumber Class for store vector of double
// ********************************************************************************************
class dataSetNumber : public dataSetBase {
private:
    
   vector<double> data;

public:
    
    
    dataSetNumber(string elementName){
        name = elementName;
        type = dataTypeNumber;
        size = 0;
    }
    
    ~dataSetNumber(){}
    
    int pushback(double element);
    int pushback(string element){return 0;}
    int pushback(unique_ptr<dataSetBase> element) {return 0;}
    int pushback(dbDataTime element){return 0;}
    
    dataSetBase* addElement(string elementName, int elementType) {return nullptr;}
    
    int setValue(double element, unsigned long index );
    int setValue(string element, unsigned long index ){return 0;}
    int setValue(dbDataTime element, unsigned long index ){return 0;}

    unsigned long addFromString (const string& str, string delimetr );
    unsigned long addFromString (const string& str, string delimetr, string mask ){return 0;}

//    dataSetBase* getELementPtrByIdx (unsigned long idx) {return this; }
    
    int getValue(double &el, unsigned long index);
    int getValue(string &el, unsigned long index) { return 0 ; }
    int getValue(dbDataTime &el, unsigned long index) {return 0;}

    string getValueAsString(unsigned long index, string mask);
    
    string dysplay(unsigned long start, unsigned long end, string delimetr);
    string dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn );
    string dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level = 0);
    
    long find(string value) {return -1;}
    int deleteByIndex( unsigned long index, int releasePtrSign=0 );
    int deleteByName( string name, int releasePtrSign=0 ){return 0;}
    
    long  getIdxByValue( double value,  long start );
    long  getIdxByValue( string value,  long start ) {return 0;}
    long  getIdxByValue( dbDataTime value,  long start ) {return 0;}
    
    long uniq(dataSetBase *ds ) ;
    long sort(int direction) ;
    long group(string column, dataSetBase *newDS  ) {return 0;}

    double sum(unsigned long start, unsigned long end);
    double max(unsigned long start, unsigned long end);
    double min(unsigned long start, unsigned long end);

    dbDataTime* maxDataTime(unsigned long start, unsigned long end) {return nullptr;}
    dbDataTime* minDataTime(unsigned long start, unsigned long end) {return nullptr;}
    
    string expressionPrepare (const string & expression, unsigned long index);

    
};



// ********************************************************************************************
// dataSetText Class for store vector of double
// ********************************************************************************************
class dataSetText : public dataSetBase {
private:
    vector<string> data;

public:
    
    dataSetText(string elementName){
        name = elementName;
        type = dataTypeText;
        size = 0;
    }
    
    ~dataSetText(){}
    
    int pushback(double element){return 0;}
    int pushback(string element);
    int pushback(unique_ptr<dataSetBase> element) {return 0;}
    int pushback(dbDataTime element){return 0;}
    
    dataSetBase* addElement(string elementName, int elementType) {return nullptr;}
    
    int setValue(double element, unsigned long index ){return 0;};
    int setValue(string element, unsigned long index );
    int setValue(dbDataTime element, unsigned long index ){return 0;}

    
    unsigned long addFromString (const string& str, string delimetr );
    unsigned long addFromString (const string& str, string delimetr, string mask ){return 0;}
    
//    dataSetBase* getELementPtrByIdx (unsigned long idx) {return this; }
    
    int getValue(double &el, unsigned long index){return 0;}
    int getValue(string &el, unsigned long index);
    int getValue(dbDataTime &el, unsigned long index) {return 0;}
    
    string getValueAsString(unsigned long index, string mask);
    
    string dysplay(unsigned long start, unsigned long end, string delimetr);
    string dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn );
    string dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level = 0);
    
    long find(string value) {return -1;}
    int deleteByIndex( unsigned long index, int releasePtrSign=0 );
    int deleteByName( string name, int releasePtrSign=0 ){return 0;}
    
    long  getIdxByValue( double value,  long start ) {return 0;}
    long  getIdxByValue( string value,  long start ) ;
    long  getIdxByValue( dbDataTime value,  long start ) {return 0;}
    
    long uniq(dataSetBase *ds );
    long sort(int direction) ;
    long group(string column , dataSetBase *newDS ) {return 0;}
    
    double sum(unsigned long start, unsigned long end) {return 0;}
    double max(unsigned long start, unsigned long end) {return 0;}
    double min(unsigned long start, unsigned long end) {return 0;}
    
    dbDataTime* maxDataTime(unsigned long start, unsigned long end) {return nullptr;}
    dbDataTime* minDataTime(unsigned long start, unsigned long end) {return nullptr;}

    string expressionPrepare (const string & expression, unsigned long index);
    
 
};


// ********************************************************************************************
// dataSetDateTime Class for store vector of dbDataTime
// ********************************************************************************************
class dataSetDateTime : public dataSetBase {
private:
    vector<dbDataTime> data;
    
public:
    
    dataSetDateTime(string elementName){
        name = elementName;
        type = dataTypeTimestamp;
        size = 0;
    }

    dataSetDateTime(string elementName, int elementType ){
        name = elementName;
        type = elementType;
        size = 0;
    }

    ~dataSetDateTime(){}
    
    int pushback(double element){return 0;}
    int pushback(string element){return 0;}
    int pushback(unique_ptr<dataSetBase> element) {return 0;}
    int pushback(dbDataTime element);
    
    dataSetBase* addElement(string elementName, int elementType) {return nullptr;}
    
    int setValue(double element, unsigned long index ){return 0;}
    int setValue(string element, unsigned long index ){return 0;}
    int setValue(dbDataTime element, unsigned long index );

    
    unsigned long addFromString (const string& str, string delimetr ){return 0; };
    unsigned long addFromString (const string& str, string delimetr, string mask );
    
//    dataSetBase* getELementPtrByIdx (unsigned long idx) {return this; }
    
    int getValue(double &el, unsigned long index){return 0;}
    int getValue(string &el, unsigned long index);
    int getValue(dbDataTime &el, unsigned long index) ;
    
    string getValueAsString(unsigned long index, string mask);
    
    string dysplay(unsigned long start, unsigned long end, string delimetr);
    string dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn);
    string dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level = 0);
    
    long find(string value) {return -1;}
    int deleteByIndex( unsigned long index, int releasePtrSign=0 );
    int deleteByName( string name, int releasePtrSign=0 ){return 0;}
    
    long  getIdxByValue( double value,  long start ) {return 0;}
    long  getIdxByValue( string value,  long start ) {return 0;}
    long  getIdxByValue( dbDataTime value,  long start );
    
    long uniq(dataSetBase *ds );
    long sort(int direction)  ;
    long group(string column, dataSetBase *newDS  ) {return 0;}
    
    double sum(unsigned long start, unsigned long end) {return 0;}
    double max(unsigned long start, unsigned long end) {return 0;}
    double min(unsigned long start, unsigned long end) {return 0;}
    
    dbDataTime* maxDataTime(unsigned long start, unsigned long end) ;
    dbDataTime* minDataTime(unsigned long start, unsigned long end) ;

    string expressionPrepare (const string & expression, unsigned long index);
    
    
};


#endif



