/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#include "DataBox.h"
#include "DataSetClases.h"
#include "Utilites.h"
#include "Enviroment.h"
#include "dbDataTime.h"


extern enviroments *enviroment;


// ******************************************************************************************************************
// Version Return vesion number of DataBox PlugIn
// ********************************************************************************************************************/
fmx::errcode Version ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
 
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    resultString ->Assign ( VERSION_STRING ,  fmx::Text::kEncoding_UTF8 );
    
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
}



// ******************************************************************************************************************
// NewDataSet( dataSetName ; dataType ; initValues ; delimeter ; replaceSign {; datatymeMask }) CREATE NEW OBJECT IN ENVIROMENT
// ********************************************************************************************************************/
fmx::errcode NewDataSet ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    int dataType = dataSetTypeFromStr( TextAsUTF8String(parameters.AtAsText(1)) );
    std::string initValues = TextAsUTF8String( parameters.AtAsText(2) );
    std::string delimeter = TextAsUTF8String( parameters.AtAsText(3) );
    int replaceSign = parameters.AtAsNumber(4).AsLong();
    
    std::string mask = "";
    if (parameters.Size()>5)
        mask = TextAsUTF8String( parameters.AtAsText(5) );
    
    // create element and add to enviroment
    int res = enviroment->createElemnet(dataSetName, dataType, initValues, delimeter, replaceSign, mask);
    
    // return result status
    fmx::TextUniquePtr    resultString;
    if (res == 1){
        resultString ->Assign ( "OK",  fmx::Text::kEncoding_UTF8 );
    } else if (res == -1){
        resultString ->Assign ( "Object already exist",  fmx::Text::kEncoding_UTF8 );
    }else{
        resultString ->Assign ( "Incorrect parameters",  fmx::Text::kEncoding_UTF8 );
    }
    fmx::LocaleUniquePtr default_locale;
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
    
}




// ******************************************************************************************************************
// GetDataSet( dataSetName )  Dysplay DataSet
// ********************************************************************************************************************/
fmx::errcode GetDataSet ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *element = enviroment->getElemnet(dataSetName);

    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    if (element) {
        string res = element->dysplay(0, element->lenght()-1, "\n");
        resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
    }else{
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
    }
    
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
    
}

// ******************************************************************************************************************
// Lenght( dataSetName )  Dysplay DataSet
// ********************************************************************************************************************/
fmx::errcode Lenght ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    
    fmx::LocaleUniquePtr default_locale;
    if (element) {
        fmx::FixPtUniquePtr    resultDouble;
        resultDouble ->AssignDouble( element->lenght() );
        reply.SetAsNumber( *resultDouble );
    }else{
        fmx::TextUniquePtr    resultString;
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
    }
    
    return 0;
    
}

// ******************************************************************************************************************
// AddValues( dataSetName ; values ; delimeter {; datatymeMask } )   add values to dataset from string with delimeter
// ********************************************************************************************************************/
fmx::errcode AddValues ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    std::string values = TextAsUTF8String( parameters.AtAsText(1) );
    std::string delimeter = TextAsUTF8String( parameters.AtAsText(2) );
    std::string mask = "";
    if (parameters.Size()>3)
        mask = TextAsUTF8String( parameters.AtAsText(3) );
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    
    if (element){
        unsigned long res = 0;
        if (element->getType() == dataTypeDate || element->getType() == dataTypeTime || element->getType() == dataTypeTimestamp )
            res = element->addFromString(values, delimeter, mask);
        else
            res = element->addFromString(values, delimeter);
        fmx::FixPtUniquePtr    resultDouble;
        resultDouble ->AssignDouble( res );
        reply.SetAsNumber( *resultDouble );
    }else{
        fmx::LocaleUniquePtr default_locale;
        fmx::TextUniquePtr    resultString;
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
    }

    return 0;
    
}

// ******************************************************************************************************************
// MoveDataSet ( dataSetNameSource ; dataSetNameTarget ; replaceSign )  Move dataSet to end of other dataSet vector
// ********************************************************************************************************************/
fmx::errcode MoveDataSet ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetNameSrc = TextAsUTF8String( parameters.AtAsText(0) );
    std::string dataSetNameTrg = TextAsUTF8String( parameters.AtAsText(1) );
    int replaceSign = parameters.AtAsNumber(2).AsLong();
    
    if(dataSetNameSrc == dataSetNameTrg ){
        fmx::LocaleUniquePtr default_locale;
        fmx::TextUniquePtr    resultString;
        resultString ->Assign ( "DataSet can't move to self",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *elementSrc = enviroment->getElemnet(dataSetNameSrc);
    dataSetBase *elementTrg = enviroment->getElemnet(dataSetNameTrg);

    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;

    if (!elementSrc){
        resultString ->Assign ( "DataSet-Source not found",  fmx::Text::kEncoding_UTF8 );
    }else if(!elementTrg){
        resultString ->Assign ( "DataSet-Target not found",  fmx::Text::kEncoding_UTF8 );
    }else if( elementTrg->getType() != dataTypeElement ){
        resultString ->Assign ( "Incorrect DataSet type. DataSet-Target can't has DataSet-child ",  fmx::Text::kEncoding_UTF8 );
    }else {
        long index = elementTrg->find(elementSrc->getName());
        if( index>=0 && !replaceSign ){
            resultString ->Assign ( "Object with same name already exist in DataSet-Target",  fmx::Text::kEncoding_UTF8 );
        }else{
            if( index>=0 )
                elementTrg->deleteByIndex((unsigned long)index, 0);
        unique_ptr<dataSetBase> srcPtr(elementSrc);
        elementTrg->pushback(move(srcPtr));
        enviroment->deleteElement(dataSetNameSrc, 1);
//        ostringstream ost;s
//        ost << "OK index:" << index;
//        string res = ost.str();
        string  res = "OK";
        resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
        }
    }
    reply.SetAsText ( *resultString, *default_locale );
    return 0;
    
}


// ******************************************************************************************************************
// IsExist( dataSetName )  Return 1 if DataSeet exist in plugin memory, else 0
// ********************************************************************************************************************/
fmx::errcode IsExist ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    
    fmx::FixPtUniquePtr    resultDouble;
    double res;
    if (element)
        res = 1;
    else
        res = 0;

    resultDouble ->AssignDouble( res );
    reply.SetAsNumber( *resultDouble );
    
    return 0;
    
}


// ******************************************************************************************************************
// Delete( dataSetName )  Delete DataSet or value by index . Name is full path to DS. ex: DSName1$DSName1.1 or DSName1$DSName1.1[2]
// ********************************************************************************************************************/
fmx::errcode Delete ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    int res = enviroment->deleteElement(dataSetName, 0);
    
    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    if (res)
        resultString ->Assign ( "OK",  fmx::Text::kEncoding_UTF8 );
    else
        resultString ->Assign ( "Object not found",  fmx::Text::kEncoding_UTF8 );
    
    reply.SetAsText ( *resultString, *default_locale );
    
    
    return 0;
    
}

// ******************************************************************************************************************
// EnviromentInfo  Return list of DataSets store in plugin memory
// ********************************************************************************************************************/
fmx::errcode EnviromentInfo ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    
    string envInfo;
    int res = enviroment->info(envInfo);

    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    if(res){
        resultString ->Assign ( envInfo.c_str(),  fmx::Text::kEncoding_UTF8 );
    }else{
        resultString ->Assign ( "Objects not found",  fmx::Text::kEncoding_UTF8 );
    }
    
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
    
}


// ******************************************************************************************************************
// Agregate (dataSetName; action {; start; end } )  Return aggregated values like min/max/sum/avg for DataSet from diapason
// ********************************************************************************************************************/
fmx::errcode Agregate ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply){
 
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    std::string action = TextAsUTF8String( parameters.AtAsText(1) );
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    fmx::FixPtUniquePtr    resultDouble;
    fmx::DateTimeUniquePtr resultDataTime;

    if (element){
        unsigned long start = 0;
        unsigned long end = element->lenght()-1;
        if (parameters.Size()>2) start = (unsigned long)parameters.AtAsNumber(2).AsLong()-1;
        if (parameters.Size()>3) {
            end = (unsigned long)parameters.AtAsNumber(3).AsLong()-1;
            if (end >= element->lenght() ) end = element->lenght()-1;
        }
        
        int elementType = element->getType();

        //dataTypeNumber
        if ( elementType == dataTypeNumber && action == "sum" ) {
            resultDouble ->AssignDouble( element->sum(start, end) );
            reply.SetAsNumber( *resultDouble );
            
        }else if ( elementType == dataTypeNumber && action == "avg" ) {
            resultDouble ->AssignDouble( element->sum(start, end)/(end-start+1) );
            reply.SetAsNumber( *resultDouble );
            
        }else if ( elementType == dataTypeNumber && action == "max" ) {
            resultDouble ->AssignDouble( element->max(start, end) );
            reply.SetAsNumber( *resultDouble );

        }else if ( elementType == dataTypeNumber && action == "min" ) {
            resultDouble ->AssignDouble( element->min(start, end) );
            reply.SetAsNumber( *resultDouble );
          
        // dataTypeDate
        }else if ( elementType == dataTypeDate && action == "max" ) {
            dbDataTime *maxdate = element->maxDataTime(start, end);
            resultDataTime->SetNormalizedDate( (short)maxdate->getMonth(),  (short)maxdate->getDay(),  (short)maxdate->getYear());
            reply.SetAsDate(*resultDataTime);

        }else if ( elementType == dataTypeDate && action == "min" ) {
            dbDataTime *mindate = element->minDataTime(start, end);
            resultDataTime->SetNormalizedDate( (short)mindate->getMonth(),  (short)mindate->getDay(),  (short)mindate->getYear());
            reply.SetAsDate(*resultDataTime);

        // dataTypeTime
        }else if ( elementType == dataTypeTime && action == "max" ) {
            dbDataTime *maxtime = element->maxDataTime(start, end);
            resultDataTime->SetNormalizedTime( (short)maxtime->getHour(),  (short)maxtime->getMinute(),  (short)maxtime->getSeconds());
            reply.SetAsTime(*resultDataTime);
            
        }else if ( elementType == dataTypeTime && action == "min" ) {
            dbDataTime *mintime = element->minDataTime(start, end);
            resultDataTime->SetNormalizedTime( (short)mintime->getHour(),  (short)mintime->getMinute(),  (short)mintime->getSeconds());
            reply.SetAsTime(*resultDataTime);
 
        // dataTypeTimestamp
        }else if ( elementType == dataTypeTimestamp && action == "max" ) {
            dbDataTime *maxtimestamp = element->maxDataTime(start, end);
            resultDataTime->SetNormalizedDate( (short)maxtimestamp->getMonth(),  (short)maxtimestamp->getDay(),  (short)maxtimestamp->getYear());
            resultDataTime->SetNormalizedTime( (short)maxtimestamp->getHour(),  (short)maxtimestamp->getMinute(),  (short)maxtimestamp->getSeconds());
            reply.SetAsTime(*resultDataTime);

        }else if ( elementType == dataTypeTimestamp && action == "min" ) {
            dbDataTime *mintimestamp = element->minDataTime(start, end);
            resultDataTime->SetNormalizedDate( (short)mintimestamp->getMonth(),  (short)mintimestamp->getDay(),  (short)mintimestamp->getYear());
            resultDataTime->SetNormalizedTime( (short)mintimestamp->getHour(),  (short)mintimestamp->getMinute(),  (short)mintimestamp->getSeconds());
            reply.SetAsTime(*resultDataTime);
            
        // Other
        }else {
            resultString ->Assign ( "This function can't apply to this type of dataset",  fmx::Text::kEncoding_UTF8 );
            reply.SetAsText ( *resultString, *default_locale );
        }
    }else{
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
    }
    
    return 0;
    
}


// ******************************************************************************************************************
// GetValue (dataSetValuePath ) Return value by full path with index (ex: Parent$Child[3]). For 'dataset' type return name of child element
// ********************************************************************************************************************/
fmx::errcode GetValue ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply){
    
    //prepare parameters
    std::string dataSetPath = TextAsUTF8String( parameters.AtAsText(0) );
    
    // get element by name. name could be path like elementparent$elementchild
    long index = -1;
    dataSetBase *element = enviroment->getElemnetWIdx(dataSetPath, index);
    
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    fmx::FixPtUniquePtr    resultDouble;
    fmx::DateTimeUniquePtr resultDataTime;
    
    if(element && index >=0){
        int elementType = element->getType();
        if (index > (long)element->lenght()-1) {
            resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
            reply.SetAsText ( *resultString, *default_locale );
        }else{
            
            string valueElement;
            double valueDouble;
            string valueText;
            dbDataTime valueDataTime;

            switch (elementType) {
                case dataTypeElement:
                    if ( element->getValue(valueElement, (unsigned long)index) )
                        resultString ->Assign ( valueElement.c_str(),  fmx::Text::kEncoding_UTF8 );
                    else
                        resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
                    reply.SetAsText ( *resultString, *default_locale );
                    break;
                    
                case dataTypeNumber:
                    if ( element->getValue(valueDouble, (unsigned long)index) ){
                        resultDouble ->AssignDouble( valueDouble );
                        reply.SetAsNumber( *resultDouble );
                    }else{
                        resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
                        reply.SetAsText ( *resultString, *default_locale );
                    }
                    break;
                    
                case dataTypeText:
                    if ( element->getValue(valueText, (unsigned long)index) )
                        resultString ->Assign ( valueText.c_str(),  fmx::Text::kEncoding_UTF8 );
                    else
                        resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
                    reply.SetAsText ( *resultString, *default_locale );
                    break;
                    
                case dataTypeDate:
                    if ( element->getValue(valueDataTime, (unsigned long)index) ){
                        resultDataTime->SetNormalizedDate( (short)valueDataTime.getMonth(),  (short)valueDataTime.getDay(),  (short)valueDataTime.getYear());
                        reply.SetAsDate(*resultDataTime);
                    } else {
                        resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
                        reply.SetAsText ( *resultString, *default_locale );
                    }
                    break;

                case dataTypeTime:
                    if ( element->getValue(valueDataTime, (unsigned long)index) ){
                        resultDataTime->SetNormalizedTime( (short)valueDataTime.getHour(),  (short)valueDataTime.getMinute(),  (short)valueDataTime.getSeconds() );
                        reply.SetAsTime(*resultDataTime);
                    } else {
                        resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
                        reply.SetAsText ( *resultString, *default_locale );
                    }
                    break;
                    
                case dataTypeTimestamp:
                    if ( element->getValue(valueDataTime, (unsigned long)index) ){
                        resultDataTime->SetNormalizedDate( (short)valueDataTime.getMonth(),  (short)valueDataTime.getDay(),  (short)valueDataTime.getYear());
                        resultDataTime->SetNormalizedTime( (short)valueDataTime.getHour(),  (short)valueDataTime.getMinute(),  (short)valueDataTime.getSeconds() );
                        reply.SetAsTimeStamp(*resultDataTime);
                    } else {
                        resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
                        reply.SetAsText ( *resultString, *default_locale );
                    }
                    break;

                    
                    
                default:
                    resultString ->Assign ( "This function can't apply to this type of dataset",  fmx::Text::kEncoding_UTF8 );
                    reply.SetAsText ( *resultString, *default_locale );
                    break;
            }
        }
        
    }else{
        resultString ->Assign ( "Incorrect path",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
    }
    
    return 0;
}



// ******************************************************************************************************************
// SetValue ( dataSetValuePath, value )  Set value to Dataset by path
// ********************************************************************************************************************/
fmx::errcode SetValue ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetPath = TextAsUTF8String( parameters.AtAsText(0) );
    
    // get element by name. name could be path like elementparent$elementchild
    long index = -1;
    dataSetBase *element = enviroment->getElemnetWIdx(dataSetPath, index);
    
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    fmx::FixPtUniquePtr    resultDouble;
    fmx::DateTimeUniquePtr resultDataTime;
    int res=0;
    double valueDouble;
    string valueString;
    dbDataTime valueDate(0,0,0);
    dbDataTime valueTime(1, 0,0,0);
    dbDataTime valueTimestamp(0,0,0,0,0,0);

    
    fmx::DateTimeUniquePtr DTptr;
    
    if(element && index >=0){
        int elementType = element->getType();
        if (index > (long)element->lenght()-1) {
            resultString ->Assign ( "Index is out of range",  fmx::Text::kEncoding_UTF8 );
            reply.SetAsText ( *resultString, *default_locale );
        }else{
            switch (elementType) {
                case dataTypeNumber:
                    valueDouble = parameters.AtAsNumber(1).AsFloat();
                    res = element->setValue(valueDouble, (unsigned long)index);
                    break;
                    
                case dataTypeText:
                    valueString = TextAsUTF8String( parameters.AtAsText(1) );
                    res = element->setValue(valueString, (unsigned long)index);
                    break;

                case dataTypeDate:
                    valueDate.setDataTime( parameters.AtAsDate(1).GetDay() , parameters.AtAsDate(1).GetMonth(), parameters.AtAsDate(1).GetYear(), 0,0,0 );
                    res = element->setValue(valueDate, (unsigned long)index);
                    break;

                case dataTypeTime:
                    valueTime.setDataTime( 0,0,0, parameters.AtAsTime(1).GetHour() , parameters.AtAsTime(1).GetMinute(), parameters.AtAsTime(1).GetSec() );
                    res = element->setValue(valueDate, (unsigned long)index);
                    break;

                case dataTypeTimestamp:
                    valueTimestamp.setDataTime( parameters.AtAsTimeStamp(1).GetDay() , parameters.AtAsTimeStamp(1).GetMonth(), parameters.AtAsTimeStamp(1).GetYear(), parameters.AtAsTimeStamp(1).GetHour() , parameters.AtAsTimeStamp(1).GetMinute(), parameters.AtAsTimeStamp(1).GetSec() );
                    res = element->setValue(valueDate, (unsigned long)index);
                    break;

                default:
                    res = -1;
                    resultString ->Assign ( "This function can't apply to this type of dataset",  fmx::Text::kEncoding_UTF8 );
                    reply.SetAsText ( *resultString, *default_locale );
                    break;
            }
            
            if (res == 1) {
                resultString ->Assign ( "OK",  fmx::Text::kEncoding_UTF8 );
                reply.SetAsText ( *resultString, *default_locale );
            }else if (res == 0){
                resultString ->Assign ( "Index is incorrect",  fmx::Text::kEncoding_UTF8 );
                reply.SetAsText ( *resultString, *default_locale );
            }
        
        }
        
    }else{
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
    }
    
    
    return 0;
    
}

// ******************************************************************************************************************
// Seq ( dataSetName ; dataType ; start ; end {; step } ) Create DataSet as sequence from start to end with step (default step = 1)
// ********************************************************************************************************************/
fmx::errcode Seq ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    int dataType = dataSetTypeFromStr( TextAsUTF8String(parameters.AtAsText(1)) );
    
    
    fmx::TextUniquePtr  resultString;
    fmx::LocaleUniquePtr default_locale;
    if (dataType != dataTypeNumber && dataType != dataTypeDate && dataType != dataTypeTime && dataType != dataTypeTimestamp) {
        resultString ->Assign ( "Incorrect dataType",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    //create dataset and get pointer
    int res = enviroment->createElemnet(dataSetName, dataType, "", "", 1, "" );
    dataSetBase *element = enviroment->getElemnet(dataSetName);

    string debugStr;
    
    if (dataType == dataTypeNumber) {
        double numStart=0, numEnd=0, step = 1;
        if (parameters.Size()>4)
            step  = parameters.AtAsNumber(4).AsFloat();
        
        if (parameters.AtAsNumber(2).AsFloat() <= parameters.AtAsNumber(3).AsFloat()) {
            numStart = parameters.AtAsNumber(2).AsFloat();
            numEnd = parameters.AtAsNumber(3).AsFloat();
        }else{
            numStart = parameters.AtAsNumber(3).AsFloat();
            numEnd = parameters.AtAsNumber(2).AsFloat();
        }
        
        double valueNum = parameters.AtAsNumber(2).AsFloat();
        
        while (numStart <= numEnd) {
            element->pushback(valueNum);
            numStart = numStart + abs(step);
            valueNum = valueNum+step;
        }
        
        
    }else if (dataType == dataTypeDate){
        dbDataTime valueDate(0,0,0), dateStart(0,0,0), dateEnd(0,0,0);
        dateStart.setDataTime( parameters.AtAsDate(2).GetDay() , parameters.AtAsDate(2).GetMonth(), parameters.AtAsDate(2).GetYear(), 0,0,0 );
        dateEnd.setDataTime( parameters.AtAsDate(3).GetDay() , parameters.AtAsDate(3).GetMonth(), parameters.AtAsDate(3).GetYear(), 0,0,0 );
        if (dateStart > dateEnd) {
            dbDataTime tmpDate = dateStart;
            dateStart = dateEnd;
            dateEnd = tmpDate;
        }
        
        long step = 1;
        if (parameters.Size()>4)
            step  = parameters.AtAsNumber(4).AsLong();
        
        
        valueDate.setDataTime( parameters.AtAsDate(2).GetDay() , parameters.AtAsDate(2).GetMonth(), parameters.AtAsDate(2).GetYear(), 0,0,0 );
        fmx::DateTimeUniquePtr tmpDatePtr;
        
        while (dateStart <= dateEnd) {
            element->pushback(valueDate);
            
            tmpDatePtr->SetNormalizedDate( (short)dateStart.getMonth(),  (short)(dateStart.getDay()+abs(step)),  (short)dateStart.getYear() );
            dateStart.setDataTime(tmpDatePtr->GetDay(), tmpDatePtr->GetMonth(), tmpDatePtr->GetYear(), 0, 0, 0);

            tmpDatePtr->SetNormalizedDate( (short)valueDate.getMonth(),  (short)(valueDate.getDay()+step),  (short)valueDate.getYear() );
            valueDate.setDataTime(tmpDatePtr->GetDay(), tmpDatePtr->GetMonth(), tmpDatePtr->GetYear(), 0, 0, 0);
            
        }
        
    }else if (dataType == dataTypeTime){
        dbDataTime valueTime(1,0,0,0), timeStart(1,0,0,0), timeEnd(1,0,0,0);
        timeStart.setDataTime( 0,0,0, parameters.AtAsTime(2).GetHour() , parameters.AtAsTime(2).GetMinute(), parameters.AtAsTime(2).GetSec() );
        timeEnd.setDataTime( 0,0,0, parameters.AtAsTime(3).GetHour() , parameters.AtAsTime(3).GetMinute(), parameters.AtAsTime(3).GetSec() );
        if (timeStart > timeEnd) {
            dbDataTime tmpTime = timeStart;
            timeStart = timeEnd;
            timeEnd = tmpTime;
        }
        
        long step = 1;
        if (parameters.Size()>4)
            step  = parameters.AtAsNumber(4).AsLong();
        
            valueTime.setDataTime( 0,0,0, parameters.AtAsTime(2).GetHour() , parameters.AtAsTime(2).GetMinute(), parameters.AtAsTime(2).GetSec() );
            fmx::DateTimeUniquePtr tmpTimePtr;
        
        while (timeStart <= timeEnd) {
            element->pushback(valueTime);
            
            tmpTimePtr->SetNormalizedTime( (short)timeStart.getHour(),  (short)timeStart.getMinute(),  (short)(timeStart.getSeconds()+abs(step)) );
            timeStart.setDataTime(0,0,0, tmpTimePtr->GetHour(), tmpTimePtr->GetMinute(), tmpTimePtr->GetSec());
            
            tmpTimePtr->SetNormalizedTime( (short)valueTime.getHour(),  (short)valueTime.getMinute(),  (short)(valueTime.getSeconds()+step) );
            valueTime.setDataTime(0,0,0, tmpTimePtr->GetHour(), tmpTimePtr->GetMinute(), tmpTimePtr->GetSec());
            
        }
        
    }else if (dataType == dataTypeTimestamp){
        dbDataTime valueTS(0,0,0,0,0,0), tsStart(0,0,0,0,0,0), tsEnd(0,0,0,0,0,0);
        tsStart.setDataTime( parameters.AtAsTimeStamp(2).GetDay() , parameters.AtAsTimeStamp(2).GetMonth(), parameters.AtAsTimeStamp(2).GetYear(), parameters.AtAsTimeStamp(2).GetHour() , parameters.AtAsTimeStamp(2).GetMinute(), parameters.AtAsTimeStamp(2).GetSec() );
        tsEnd.setDataTime( parameters.AtAsTimeStamp(3).GetDay() , parameters.AtAsTimeStamp(3).GetMonth(), parameters.AtAsTimeStamp(3).GetYear(), parameters.AtAsTimeStamp(3).GetHour() , parameters.AtAsTimeStamp(3).GetMinute(), parameters.AtAsTimeStamp(3).GetSec() );
        if (tsStart > tsEnd) {
            dbDataTime tmpTime = tsStart;
            tsStart = tsEnd;
            tsEnd = tmpTime;
        }
        
        long step = 1;
        if (parameters.Size()>4)
            step  = parameters.AtAsNumber(4).AsLong();
        
            valueTS.setDataTime( parameters.AtAsTimeStamp(2).GetDay() , parameters.AtAsTimeStamp(2).GetMonth(), parameters.AtAsTimeStamp(2).GetYear(), parameters.AtAsTimeStamp(2).GetHour() , parameters.AtAsTimeStamp(2).GetMinute(), parameters.AtAsTimeStamp(2).GetSec() );
            fmx::DateTimeUniquePtr tmpTSPtr;
        
        while (tsStart <= tsEnd) {
            element->pushback(valueTS);
            
            tmpTSPtr->SetNormalizedDate( (short)tsStart.getMonth(),  (short)tsStart.getDay(),  (short)tsStart.getYear() );
            tmpTSPtr->SetNormalizedTime( (short)tsStart.getHour(),  (short)tsStart.getMinute(),  (short)(tsStart.getSeconds()+abs(step)) );
            tsStart.setDataTime(tmpTSPtr->GetDay(), tmpTSPtr->GetMonth(), tmpTSPtr->GetYear(), tmpTSPtr->GetHour(), tmpTSPtr->GetMinute(), tmpTSPtr->GetSec());

            tmpTSPtr->SetNormalizedDate( (short)valueTS.getMonth(),  (short)valueTS.getDay(),  (short)valueTS.getYear() );
            tmpTSPtr->SetNormalizedTime( (short)valueTS.getHour(),  (short)valueTS.getMinute(),  (short)(valueTS.getSeconds()+step) );
            valueTS.setDataTime(tmpTSPtr->GetDay(), tmpTSPtr->GetMonth(), tmpTSPtr->GetYear(), tmpTSPtr->GetHour(), tmpTSPtr->GetMinute(), tmpTSPtr->GetSec());
            
        }
        
    }


    
    
    resultString ->Assign ( "OK",  fmx::Text::kEncoding_UTF8 );
    reply.SetAsText ( *resultString, *default_locale );
    return 0;

}



// ******************************************************************************************************************
// GetDataSetAsJSON ( dataSetName { ; aloneElemenAsArray  ; pretty } )    Dysplay DataSet as JSON
// ********************************************************************************************************************/
fmx::errcode GetDataSetAsJSON ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    int aloneElementAsArray = 0;
    if (parameters.Size()>1)
        aloneElementAsArray = parameters.AtAsNumber(1).AsLong();
    int pretty = 1;
    if (parameters.Size()>2)
        pretty = parameters.AtAsNumber(2).AsLong();
    if (pretty != 0 )
        pretty = 1;
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    
    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    if (element) {
        string res = element->dysplayJSON(0, element->lenght()-1, aloneElementAsArray, pretty );
        resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
    }else{
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
    }
    
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
    
}



// ******************************************************************************************************************
// GetIndexByValue ( dataSetName; value { ; startIndex } ) Return first founded Index of element by Value starting from startIndex
// ********************************************************************************************************************/
fmx::errcode GetIndexByValue ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    
    // get element by name. name could be path like elementparent$elementchild
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    if (!element) {
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    int elementType = element->getType();
    
    long startIndex = 0;
    if (parameters.Size()>2)
        startIndex = parameters.AtAsNumber(1).AsLong();

    
    long res=0;
    double valueDouble;
    string valueString;
    dbDataTime valueDate(0,0,0);
    dbDataTime valueTime(1, 0,0,0);
    dbDataTime valueTimestamp(0,0,0,0,0,0);

    
    switch (elementType) {
            
        case dataTypeElement:
            valueString = TextAsUTF8String( parameters.AtAsText(1) );
            res = element->getIdxByValue(valueString, startIndex );
            break;
            
        case dataTypeNumber:
            valueDouble = parameters.AtAsNumber(1).AsFloat();
            res = element->getIdxByValue(valueDouble, startIndex);
            break;
            
        case dataTypeText:
            valueString = TextAsUTF8String( parameters.AtAsText(1) );
            res = element->getIdxByValue(valueString, startIndex );
            break;
            
        case dataTypeDate:
            valueDate.setDataTime( parameters.AtAsDate(1).GetDay() , parameters.AtAsDate(1).GetMonth(), parameters.AtAsDate(1).GetYear(), 0,0,0 );
            res = element->getIdxByValue(valueDate, startIndex );
            break;
            
        case dataTypeTime:
            valueTime.setDataTime( 0,0,0, parameters.AtAsTime(1).GetHour() , parameters.AtAsTime(1).GetMinute(), parameters.AtAsTime(1).GetSec() );
            res = element->getIdxByValue(valueDate, startIndex );
            break;
            
        case dataTypeTimestamp:
            valueTimestamp.setDataTime( parameters.AtAsTimeStamp(1).GetDay() , parameters.AtAsTimeStamp(1).GetMonth(), parameters.AtAsTimeStamp(1).GetYear(), parameters.AtAsTimeStamp(1).GetHour() , parameters.AtAsTimeStamp(1).GetMinute(), parameters.AtAsTimeStamp(1).GetSec() );
            res = element->getIdxByValue(valueDate, startIndex );
            break;
            
        default:
            res = -1;
            resultString ->Assign ( "This function can't apply to this type of dataset",  fmx::Text::kEncoding_UTF8 );
            reply.SetAsText ( *resultString, *default_locale );
            break;
    }
    
    res = res + 1;
    
    fmx::FixPtUniquePtr resultDouble;
    resultDouble ->AssignDouble( res );
    reply.SetAsNumber( *resultDouble );
    
    return 0;
    
}



// ******************************************************************************************************************
// Uniq ( dataSetFrom ; dataSetTo )  Create new DataSet with unique values from dataSet
// ********************************************************************************************************************/
fmx::errcode Uniq ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetNameFrom = TextAsUTF8String( parameters.AtAsText(0) );
    std::string dataSetNameTo = TextAsUTF8String( parameters.AtAsText(1) );
    
    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    
    dataSetBase *elementFrom = enviroment->getElemnet(dataSetNameFrom);
    if (!elementFrom) {
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    if (elementFrom->getType() == dataTypeElement) {
        resultString ->Assign ( "The type of this DataSet incorrect for this function",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    //create dataset and get pointer
    enviroment->createElemnet(dataSetNameTo, elementFrom->getType(), "", "", 1, "" );
    dataSetBase *elementTo = enviroment->getElemnet(dataSetNameTo);
    
    long res = elementFrom->uniq( elementTo );
    
    fmx::FixPtUniquePtr    resultDouble;
    resultDouble ->AssignDouble( res );
    reply.SetAsNumber( *resultDouble );
    
    return 0;
    
}


// ******************************************************************************************************************
// Sort ( dataSetName { ; direction ; bycolumns }) Sort DataSet ASC if direction = 1, else DSC. bycolumns - names of columns for table sort
// ********************************************************************************************************************/
fmx::errcode Sort ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    if (!element) {
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    long res ;
    if (element->getType() == dataTypeElement ) {
        
        string directionStr = "1";
        if (parameters.Size()>1)
            directionStr = TextAsUTF8String( parameters.AtAsText(1) );
        
        string bycolumns = "";
        if (parameters.Size()>2)
            bycolumns = TextAsUTF8String( parameters.AtAsText(2) );
        
        if ( bycolumns.empty() ) {
            resultString ->Assign ( "Columns for order is undefined",  fmx::Text::kEncoding_UTF8 );
            reply.SetAsText ( *resultString, *default_locale );
            return 0;
        }
    
        res = element->sort( bycolumns, directionStr );

        
    }else {
        
        int direction = 1;
        if (parameters.Size()>1)
            direction = parameters.AtAsNumber(1).AsLong();
        
        res = element->sort( direction );
    }
    
    
    fmx::FixPtUniquePtr    resultDouble;
    resultDouble ->AssignDouble( res );
    reply.SetAsNumber( *resultDouble );
    
    return 0;
    
}
