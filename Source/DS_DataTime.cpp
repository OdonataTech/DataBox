/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#include "DataSetClases.h"
#include "Utilites.h"



// ********************************************************************************************
// dataSetDateTime functions
// ********************************************************************************************

// pushback()
int dataSetDateTime::pushback(dbDataTime element){
    data.push_back(element);
    size = data.size();
    order.push_back(size-1);
    return 1;
};

// addFromString()
unsigned long dataSetDateTime::addFromString (const string& str, string delimetr, string mask ){
    if (str.empty()) return 0;
    if (mask.empty()) return 0;
    vector<string> vct;
    if (stringToVector(str, vct, delimetr) > 0){
        dbDataTime dt;
        for (unsigned long i=0; i<vct.size(); ++i) {
            dt.setDateByMask(vct[i], mask);
            data.push_back(dt);
            order.push_back(data.size()-1);
        }
        size = data.size();
    }
    return size;
};


// getValue()  as string
int dataSetDateTime::getValue(string &el, unsigned long index)  {
    if (index >=0 && index < size ) {
        el = data[getRec(index)].getAsString(type);
        return 1;
    }else {
        el = "";
        return 0;
    }
};

// getValue()  as dbDataTime
int dataSetDateTime::getValue(dbDataTime &el, unsigned long index) {
    if (index >=0 && index < size ) {
        el = data[getRec(index)];
        return 1;
    }else {
        return 0;
    }
};

// setValue()
int dataSetDateTime::setValue(dbDataTime element, unsigned long index ){
    if (index >=0 && index < size ) {
        data[getRec(index)] = element;
        return 1;
    }else {
        return 0;
    }
}

// dysplay()
string dataSetDateTime::dysplay(unsigned long start, unsigned long end, string delimetr){
    if ( size==0 || start <0 || start > size-1 || end < 0 || end > size-1 || end < start) return "";
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << data[getRec(i)].getAsString(type);
        if (i < end) ost << delimetr;
    }
    return ost.str();
};

// getValueAsString()
string dataSetDateTime::getValueAsString(unsigned long index, string mask){
    
    ostringstream ost;
    if (index >=0 && index < size )
        ost << data[getRec(index)].getAsStringByMask(mask);
    
    return ost.str();
}


// deleteByIndex()
int dataSetDateTime::deleteByIndex(  unsigned long index, int releasePtrSign  ){
    if (index <0 || index > size-1 ) return 0;
    unsigned long recNumToDel = getRec(index);
    data.erase(data.begin()+(long)recNumToDel );
    size = data.size();
    
    for (unsigned long i=0; i<size; ++i ) {
        if (order[i] > recNumToDel ) {
            order[i] = order[i]-1 ;
        }
    }
    order.pop_back();
    
    return 1;
}



// max()
dbDataTime* dataSetDateTime::maxDataTime(unsigned long start, unsigned long end){
    if (start < 0 || start >= size || end < start || end < 0 ) return nullptr;
    if ( end >=size )
        end = size-1;

    auto result = std::minmax_element(data.begin()+(long)start, data.begin()+(long)end);

    return &data[(unsigned long)distance( data.begin(), result.second)] ;

}

// min()
dbDataTime* dataSetDateTime::minDataTime(unsigned long start, unsigned long end){
    if (start < 0 || start >= size || end < start || end < 0 ) return nullptr;
    if ( end >=size )
        end = size-1;
    
    auto result = std::minmax_element(data.begin()+(long)start, data.begin()+(long)end);
    
    return &data[(unsigned long)distance( data.begin(), result.first)] ;
    
}

//expressionPrepare
string dataSetDateTime::expressionPrepare (const string & expression, unsigned long index){
    if (index <0 || index > size-1 ) return "";
    
    string expressionNew = expression;
    ostringstream ostIdx, ostValue;
    if (type == dataTypeDate)
        ostValue << "Date(" << data[getRec(index)].getMonth() << ";" << data[getRec(index)].getDay() << ";" << data[getRec(index)].getYear() << ")" ;
    else if (type == dataTypeTime)
        ostValue << "Time(" << data[getRec(index)].getHour() << ";" << data[getRec(index)].getMinute() << ";" << data[getRec(index)].getSeconds() << ")" ;
    else if (type == dataTypeTimestamp) {
        ostValue << "Timestamp (";
        ostValue << "Date(" << data[getRec(index)].getMonth() << ";" << data[getRec(index)].getDay() << ";" << data[getRec(index)].getYear() << ");" ;
        ostValue << "Time(" << data[getRec(index)].getHour() << ";" << data[getRec(index)].getMinute() << ";" << data[getRec(index)].getSeconds() << ")" ;
        ostValue << ")";
    }
    
    ostIdx << index+1;
    
    substitute(expressionNew, "_elemvalue_",  ostValue.str());
    substitute(expressionNew, "_elemidx_", ostIdx.str());
    substitute(expressionNew, "'", "\"");
    
    return expressionNew;
}

// dysplayTable
string dataSetDateTime::dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn){
    if (end>size-1) end = size-1;
    if ( size==0 || start <0 || start > size-1 || end < 0 || end < start) return "";
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << data[getRec(i)].getAsString(type);
        if (i < end) ost << rowDelimeter;
    }
    if(headerOn)
        return  name + rowDelimeter + ost.str();
    else
        return ost.str();
}


// dysplayJSON()
string dataSetDateTime::dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level ){
    if ( size==0 || start <0 || start > size-1 || end < 0 || end > size-1 || end < start) return "\"\"";
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << "\"" << data[getRec(i)].getAsString(type) << "\"" ;
        if (i < end) ost << ", ";
    }
    
    if ( end - start >0 || aloneElementAsArray != 0 )
        return "[ " + ost.str() + " ]";
    else
        return ost.str();
};

// getIdxByValue
long  dataSetDateTime::getIdxByValue( dbDataTime value,  long start ){
    //    if (value.empty()) return 0;
    
    auto itr = find_if( data.begin()+ start, data.end(),
                       [&value](const dbDataTime & ptr){ return ptr == value; }
                       );
    if (itr == data.end()) {
        return -1;
    } else {
        return getIdx( (unsigned long)distance( data.begin(), itr) );
    }
    
}


// uniq()
long dataSetDateTime::uniq(dataSetBase *ds ) {
    
    if (size == 0) return 0;
    
    for (unsigned long i=0; i<size; ++i) {
        if ( ds->getIdxByValue(data[getRec(i)], 0 ) == -1 ){
            ds->pushback( data[getRec(i)] );
        }
    }
    
    return (long) ds->lenght() ;
    
}

// sort()
long dataSetDateTime::sort(int direction) {

    if (size == 0) return 0;
    
    if (direction == 1) {
        std::sort( order.begin(), order.end(), [this]( unsigned long a, unsigned long b){ return data[a] < data[b] ; }   );
    }else{
        std::sort( order.begin(), order.end(), [this]( unsigned long a, unsigned long b){ return  data[a] > data[b] ; }   );
    }
    
    return (long) size ;
    
}
