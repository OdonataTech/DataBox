/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#include "DataSetClases.h"
#include "Utilites.h"



// ********************************************************************************************
// dataSetText functions
// ********************************************************************************************

// pushback()
int dataSetText::pushback(string element){
    data.push_back(element);
    size = data.size();
    order.push_back(size-1);
    return 1;
};

// addFromString()
unsigned long dataSetText::addFromString (const string& str, string delimetr ){
    if (str.empty()) return 0;
    vector<string> vct;
    if (stringToVector(str, vct, delimetr) > 0){
        for (unsigned long i=0; i<vct.size(); ++i) {
            data.push_back(vct[i]);
            order.push_back(data.size()-1);
        }
        size = data.size();
    }
    return size;
};


// getValue()
int dataSetText::getValue(string &el, unsigned long index) {
    if (index >=0 && index < size ) {
        el = data[getRec(index)];
        return 1;
    }else {
        el = "";
        return 0;
    }
};

// setValue()
int dataSetText::setValue(string element, unsigned long index ){
    if (index >=0 && index < size ) {
        data[getRec(index)] = element;
        return 1;
    }else {
        return 0;
    }
}


// dysplay()
string dataSetText::dysplay(unsigned long start, unsigned long end, string delimetr){
    if ( size==0 || start <0 || start > size-1 || end < 0 || end > size-1 || end < start) return "";
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << data[getRec(i)];
        if (i < end) ost << delimetr;
    }
    return ost.str();
};

// getValueAsString()
string dataSetText::getValueAsString(unsigned long index, string mask){
    
    ostringstream ost;
    if (index >=0 && index < size )
        ost << data[getRec(index)];
    
    return ost.str();
}


// deleteByIndex()
int dataSetText::deleteByIndex(  unsigned long index, int releasePtrSign  ){
    if (index <0 || index > size-1 ) return 0;
    unsigned long recNumToDel = getRec(index);
    data.erase(data.begin()+(long)recNumToDel);
    size = data.size();
    
    for (unsigned long i=0; i<size; ++i ) {
        if (order[i] > recNumToDel ) {
            order[i] = order[i]-1 ;
        }
    }
    order.pop_back();
    return 1;
    
}

//expressionPrepare
string dataSetText::expressionPrepare (const string & expression, unsigned long index){
    if (index <0 || index > size-1 ) return "";
    
    string expressionNew = expression;
    ostringstream ostIdx ;
    ostIdx << index+1;
    
    substitute(expressionNew, "_elemvalue_",  "'" + data[getRec(index)] + "'" );
    substitute(expressionNew, "_elemidx_", ostIdx.str());
    substitute(expressionNew, "'", "\"");
    
    return expressionNew;
}

// dysplayTable
string dataSetText::dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn){
    if (end>size-1) end = size-1;
    if ( size==0 || start <0 || start > size-1 || end < 0 || end < start) return "";
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << data[getRec(i)];
        if (i < end) ost << rowDelimeter;
    }
    
    if(headerOn)
        return  name + rowDelimeter + ost.str();
    else
        return ost.str();
}

// dysplayJSON()
string dataSetText::dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level ){
    if ( size==0 || start <0 || start > size-1 || end < 0 || end > size-1 || end < start) return "\"\"";
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << "\"" << data[getRec(i)] << "\"" ;
        if (i < end) ost << ", ";
    }
    
    if ( end - start >0 || aloneElementAsArray != 0 )
        return "[ " + ost.str() + " ]";
    else
        return ost.str();
};


// getIdxByValue
long  dataSetText::getIdxByValue( string value,  long start ){
    if (value.empty()) return 0;
    
    auto itr = find_if( data.begin()+ start, data.end(),
                       [&value](const string & ptr){ return ptr == value; }
                       );
    if (itr == data.end()) {
        return -1;
    } else {
        return getIdx( (unsigned long)distance( data.begin(), itr) );
    }
    
}

// uniq()
long dataSetText::uniq(dataSetBase *ds ) {
    
    if (size == 0) return 0;
    
    for (unsigned long i=0; i<size; ++i) {
        if ( ds->getIdxByValue(data[getRec(i)], 0 ) == -1 ){
            ds->pushback( data[getRec(i)] );
        }
    }
    
    return (long) ds->lenght() ;
    
}


// sort()
long dataSetText::sort(int direction) {
    
    if (size == 0) return 0;
    
    if (direction == 1) {
        std::sort( order.begin(), order.end(), [this]( unsigned long a, unsigned long b){ return data[a] < data[b] ; }   );
    }else{
        std::sort( order.begin(), order.end(), [this]( unsigned long a, unsigned long b){ return  data[a] > data[b] ; }   );
    }
    
    
    return (long) size ;
    

}

