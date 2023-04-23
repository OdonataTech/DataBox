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
// NewDataSetAsTable ( dataSetName ; columnNames ; columnTypes ) CREATE NEW OBJECT IN ENVIROMENT AS TABLE
// ********************************************************************************************************************/
fmx::errcode NewDataSetAsTable ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    std::string columnNames = TextAsUTF8String( parameters.AtAsText(1) );
    std::string columnTypes = TextAsUTF8String( parameters.AtAsText(2) );
    int replaceSign = parameters.AtAsNumber(3).AsLong();
    
    // return result status
    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    
    if (columnNames.empty()) {
        resultString ->Assign ( "Column names not difne",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    if (columnNames.empty()) {
        resultString ->Assign ( "Column types not difne",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    // create element and add to enviroment
    int res = enviroment->createElemnet(dataSetName, dataTypeElement, "", "", replaceSign, "");
    
    if (res == -1){
        resultString ->Assign ( "Object already exist",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    if(res == 0){
        resultString ->Assign ( "Incorrect parameters",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    
    if (res == 1){
        
        dataSetBase *table = enviroment->getElemnet(dataSetName);
        if (table) {
            vector<string> columns;
            stringToVector(columnNames, columns, ";");
            vector<string> types;
            stringToVector(columnTypes, types, ";");
            
            string c_name;
            int c_type;
            int result = 1;
            
            for (unsigned long i=0; i<columns.size(); ++i) {
                c_name = columns[i];
                if (i > types.size()-1 ) c_type = dataTypeText;
                else c_type = dataSetTypeFromStr(types[i]);
                
                if (c_type == dataTypeElement){
                    unique_ptr<dataSetBase> dsEl(new dataSetElement(c_name));
                    table->pushback(move(dsEl));
                }else if (c_type == dataTypeNumber){
                    unique_ptr<dataSetBase> dsNum(new dataSetNumber(c_name));
                    table->pushback(move(dsNum));
                }else if (c_type == dataTypeText){
                    unique_ptr<dataSetBase> dsTxt(new dataSetText(c_name));
                    table->pushback(move(dsTxt));
                }else if (c_type == dataTypeDate || c_type == dataTypeTime || c_type == dataTypeTimestamp ){
                    unique_ptr<dataSetBase> dsDT(new dataSetDateTime(c_name, c_type));
                    table->pushback(move(dsDT));
                    
                }else{
                    result = 0;
                }
                
            }
            if (result == 1) {
                resultString ->Assign ( "OK",  fmx::Text::kEncoding_UTF8 );
            }else{
                resultString ->Assign ( "Incorrect parameters",  fmx::Text::kEncoding_UTF8 );
                enviroment->deleteElement(dataSetName, 0);
            }
            
        }else{
            resultString ->Assign ( "Table not created",  fmx::Text::kEncoding_UTF8 );
        }
        
    }else{
        resultString ->Assign ( "Incorrect parameters",  fmx::Text::kEncoding_UTF8 );
    }
    
    
    reply.SetAsText ( *resultString, *default_locale );
    return 0;
    
}


// ******************************************************************************************************************
// AddRows ( dataSetName ; tabletext ; row_delimeter ; cell_delimeter {; skipFirstRowSign; datetymeMask }  ) Add row to table from separeted text by delimeter
// ********************************************************************************************************************/
fmx::errcode AddRows ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    std::string tableText = TextAsUTF8String( parameters.AtAsText(1) );
    std::string rowDelimeter = TextAsUTF8String( parameters.AtAsText(2) );
    if (rowDelimeter.empty() ) rowDelimeter = "\n";
    std::string cellDelimeter = TextAsUTF8String( parameters.AtAsText(3) );
    if (cellDelimeter.empty() ) cellDelimeter = "\t";
    
    int skipFirstRowSign = 0;
    if (parameters.Size()>4)
        skipFirstRowSign = parameters.AtAsNumber(4).AsLong();
    
    std::string datetimeMask = "YYYY-MM-DD";
    if (parameters.Size()>5)
        datetimeMask = TextAsUTF8String( parameters.AtAsText(5) );
    
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    
    if (tableText.empty()) {
        resultString ->Assign ( "tabletext is empty",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    // get dataset pointer
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    if (!element) {
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    if (element->getType() != dataTypeElement) {
        std::string restx = "The type of " + dataSetName + " is not dataset";
        resultString ->Assign ( restx.c_str(),  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    if(element->lenght() == 0){
        std::string restx = "The " + dataSetName + " has not columns";
        resultString ->Assign ( restx.c_str(),  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
        
    }
    
    vector<string> rowVect;
    vector<string> cellVect;
    unsigned long nrow = stringToVector(tableText, rowVect, rowDelimeter);
    
    // check number of column
    unsigned long ncell = stringToVector(rowVect[0], cellVect, cellDelimeter);
    if (ncell > element->lenght()) {
        std::string restx = "The number of columns in text more then column in " + dataSetName ;
        resultString ->Assign ( restx.c_str(),  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    
    unsigned long start = 0;
    if (skipFirstRowSign > 0) start = 1;
    dataSetBase *columnElement;
    string value;
    
//    ostringstream ost;
//    ost << nrow << "\n" << start << "\n";
    
    // main loop by row
    for (unsigned long i=start; i<nrow; ++i) {
        cellVect.clear();
        ncell = stringToVector(rowVect[i], cellVect, cellDelimeter);
        
//        ost << rowVect[i] << "\n";
        
        //  loop by column. Add values to cell
        for ( unsigned long n=0; n < element->lenght(); ++n) {
            columnElement = element->getELementPtrByIdx(n);
            if (columnElement) {
                if (n>=ncell)
                    value = " ";
                else
                    value = cellVect[n];

                if ( value.empty()) value = " ";
                    
                if ( columnElement->getType() == dataTypeDate || columnElement->getType() == dataTypeTime || columnElement->getType() == dataTypeTimestamp  )
                    columnElement->addFromString(value, "", datetimeMask);
                else
                    columnElement->addFromString(value, "");
//                ost << value << "\n";
            }
        }
    }
    
    //resultString ->Assign ( ost.str().c_str() ,  fmx::Text::kEncoding_UTF8 );
    resultString ->Assign ( "OK",  fmx::Text::kEncoding_UTF8 );
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
}



// ******************************************************************************************************************
// sApply ( dataSetName ; expression { ; newDataSet ; newDataSetType} ) Apply expression for each element of vector. Store result to new DataSet or replace source
// ********************************************************************************************************************/
fmx::errcode sApply ( short /* function_id */, const fmx::ExprEnv&  environment , const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    std::string expression = TextAsUTF8String( parameters.AtAsText(1) );
    
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    
    // get dataset pointer
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    if (!element) {
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    int elementType = element->getType();
    int elementTypeNew = elementType;
    
    // create new dataset if need and get pointer
    std::string newDataSetName;
    dataSetBase *elementNew = nullptr;
    int newDSSign = 0;
    if (parameters.Size()>2){
        newDataSetName = TextAsUTF8String( parameters.AtAsText(2) );
        if (parameters.Size()>3) {
            elementTypeNew = dataSetTypeFromStr( TextAsUTF8String(parameters.AtAsText(3)) );
        }
        newDSSign = enviroment->createElemnet(newDataSetName, elementTypeNew, "", "", 1, "");
        elementNew = enviroment->getElemnet(newDataSetName);
    }
    
    if ( element->lenght() == 0 ){
            resultString ->Assign ( "",  fmx::Text::kEncoding_UTF8 );
            reply.SetAsText ( *resultString, *default_locale );
            return 0;
    }
    
    fmx::errcode error = 0;
    fmx::DataUniquePtr resultPtr;
    fmx::TextUniquePtr expressionPtr;
    
    double valueDouble;
    string valueString;
    dbDataTime valueDate(0,0,0);
    dbDataTime valueTime(1, 0,0,0);
    dbDataTime valueTimestamp(0,0,0,0,0,0);

    string expressionNew, elementValue;    
    
    for (unsigned long i=0; i<element->lenght(); ++i) {
        
        expressionNew = element->expressionPrepare(expression, i);
        expressionPtr->Assign(expressionNew.c_str(), fmx::Text::kEncoding_UTF8 );
        error = environment.Evaluate ( *expressionPtr, *resultPtr ) ;
        
        if (newDSSign == 1){
        // set value to new dataseet
            switch (elementTypeNew) {
                case dataTypeNumber:
                    valueDouble = resultPtr->GetAsNumber().AsFloat() ;
                    elementNew->pushback(valueDouble);
                    break;
                case dataTypeText:
                    valueString = TextAsUTF8String( resultPtr->GetAsText() );
                    elementNew->pushback(valueString);
                    break;
                case dataTypeDate:
                     valueDate.setDataTime( resultPtr->GetAsDate().GetDay() , resultPtr->GetAsDate().GetMonth(), resultPtr->GetAsDate().GetYear(), 0,0,0 );
                     elementNew->pushback(valueDate);
                     break;
                case dataTypeTime:
                    valueTime.setDataTime( 0,0,0,  resultPtr->GetAsTime().GetHour() , resultPtr->GetAsTime().GetMinute(), resultPtr->GetAsTime().GetSec() );
                    elementNew->pushback(valueTime);
                    break;
                case dataTypeTimestamp:
                    valueTimestamp.setDataTime( resultPtr->GetAsTimeStamp().GetDay() , resultPtr->GetAsTimeStamp().GetMonth(), resultPtr->GetAsTimeStamp().GetYear(), resultPtr->GetAsTimeStamp().GetHour() , resultPtr->GetAsTimeStamp().GetMinute(), resultPtr->GetAsTimeStamp().GetSec() );
                    elementNew->pushback(valueTimestamp);
                    break;

                default:
                    break;
            }
            
        }else{
        // replace value in source dataseet
            switch (elementType) {
                case dataTypeNumber:
                    valueDouble = resultPtr->GetAsNumber().AsFloat() ;
                    element->setValue(valueDouble, (unsigned long)i);
                    break;
                case dataTypeText:
                    valueString = TextAsUTF8String( resultPtr->GetAsText() );
                    element->setValue(valueString, (unsigned long)i);
                    break;
                case dataTypeDate:
                    valueDate.setDataTime( resultPtr->GetAsDate().GetDay() , resultPtr->GetAsDate().GetMonth(), resultPtr->GetAsDate().GetYear(), 0,0,0 );
                    element->setValue(valueDate, (unsigned long)i);
                    break;
                case dataTypeTime:
                    valueTime.setDataTime( 0,0,0,  resultPtr->GetAsTime().GetHour() , resultPtr->GetAsTime().GetMinute(), resultPtr->GetAsTime().GetSec() );
                    element->setValue(valueTime, (unsigned long)i);
                    break;
                case dataTypeTimestamp:
                    valueTimestamp.setDataTime( resultPtr->GetAsTimeStamp().GetDay() , resultPtr->GetAsTimeStamp().GetMonth(), resultPtr->GetAsTimeStamp().GetYear(), resultPtr->GetAsTimeStamp().GetHour() , resultPtr->GetAsTimeStamp().GetMinute(), resultPtr->GetAsTimeStamp().GetSec() );
                    element->setValue(valueTimestamp, (unsigned long)i);
                    break;

                default:
                    break;
            }
            
        }
//        ost.str("");
    }

    // return result as text
    string res ;
    if (newDSSign == 1)
        res = elementNew->dysplay(0, elementNew->lenght()-1, "\n");
    else
        res = element->dysplay(0, element->lenght()-1, "\n");
    
    resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
    reply.SetAsText ( *resultString, *default_locale );
    
    
    return 0;

}


// ******************************************************************************************************************
// GetDataSetAsTextTable ( dataSetName { ; row_delimeter ; cell_delimeter; headerOn ; columnsOrder ; start ; end; } ) Return DataSet as a separated text by a delimiter
// ********************************************************************************************************************/
fmx::errcode GetDataSetAsTextTable ( short /* function_id */, const fmx::ExprEnv&  environment , const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    string rowDelimeter = "\n";
    if (parameters.Size()>1) rowDelimeter = TextAsUTF8String( parameters.AtAsText(1) );
    
    string cellDelimeter = "\t";
    if (parameters.Size()>2) cellDelimeter = TextAsUTF8String( parameters.AtAsText(2) );
    
    int headerOn = 0;
    if (parameters.Size()>3) headerOn = parameters.AtAsNumber(3).AsLong();
    
    string columnsOrder = "";
    if (parameters.Size()>4) columnsOrder = TextAsUTF8String( parameters.AtAsText(4) );

    unsigned long start = 0;
    if (parameters.Size()>5) start = (unsigned long)parameters.AtAsNumber(5).AsLong() - 1;
    
    unsigned long end = ULONG_MAX;
    if (parameters.Size()>6) {
        end = (unsigned long)parameters.AtAsNumber(6).AsLong() - 1;
        if(end == 0) end = ULONG_MAX;
    }
    
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    
    // get dataset pointer
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    
    if (element) {
        string res = element->dysplayTable(start, end, rowDelimeter, cellDelimeter, columnsOrder, headerOn );
        resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
    }else{
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
    }
    
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
}


// ******************************************************************************************************************
// SaveTableToTextFile ( dataSetName, filepath, { ; row_delimeter ; cell_delimeter; headerOn ; columnsOrder ; start ; end; } ) Save DataSet as a separated text by a delimiter to file
// ********************************************************************************************************************/
fmx::errcode SaveTableToTextFile ( short /* function_id */, const fmx::ExprEnv&  environment , const fmx::DataVect& parameters, fmx::Data& reply)
{
	//prepare parameters
	std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
	
	std::string filepath = TextAsUTF8String( parameters.AtAsText(1) );
	
	string rowDelimeter = "\n";
	if (parameters.Size()>2) rowDelimeter = TextAsUTF8String( parameters.AtAsText(2) );
	
	string cellDelimeter = "\t";
	if (parameters.Size()>3) cellDelimeter = TextAsUTF8String( parameters.AtAsText(3) );
	
	int headerOn = 0;
	if (parameters.Size()>4) headerOn = parameters.AtAsNumber(4).AsLong();
	
	string columnsOrder = "";
	if (parameters.Size()>5) columnsOrder = TextAsUTF8String( parameters.AtAsText(5) );

	unsigned long start = 0;
	if (parameters.Size()>6) start = (unsigned long)parameters.AtAsNumber(6).AsLong() - 1;
	
	unsigned long end = ULONG_MAX;
	if (parameters.Size()>7) {
		end = (unsigned long)parameters.AtAsNumber(7).AsLong() - 1;
		if(end == 0) end = ULONG_MAX;
	}
	
	fmx::LocaleUniquePtr default_locale;
	fmx::TextUniquePtr    resultString;
	
	// get dataset pointer
	dataSetBase *element = enviroment->getElemnet(dataSetName);
	
	if (element) {
		string res = element->saveTableToFile(filepath, start, end, rowDelimeter, cellDelimeter, columnsOrder, headerOn );
		resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
		
	}else{
		resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
	}
	
	reply.SetAsText ( *resultString, *default_locale );
	
	return 0;
}



// ******************************************************************************************************************
// GetDataSetAsHTMLtTable ( dataSetName ; tableParameters { ; start ; end ; customJS ; datasetWHtmlPart } ) Return DataSet as HTML table
// ********************************************************************************************************************/
fmx::errcode GetDataSetAsHTMLtTable ( short /* function_id */, const fmx::ExprEnv&  environment , const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    
    std::string paramString = TextAsUTF8String( parameters.AtAsText(1) );
    
    unsigned long start = 0;
    if (parameters.Size()>2) start = (unsigned long)parameters.AtAsNumber(2).AsLong() - 1;
    
    unsigned long end = ULONG_MAX;
    if (parameters.Size()>3) {
        end = (unsigned long)parameters.AtAsNumber(3).AsLong() - 1;
        if(end == 0) end = ULONG_MAX;
    }
        
    std::string customJS = "";
    if (parameters.Size()>4) {
        customJS = TextAsUTF8String( parameters.AtAsText(4) );
    }
    
    
    fmx::LocaleUniquePtr default_locale;
    fmx::TextUniquePtr    resultString;
    
    // get dataset pointer
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    

    
    if (element) {
        
        srand(static_cast<unsigned int>(time(0)));
        string css  = "";
//        string css = "\n"
//            ".db_container { padding: 0px; width: 100%; height: inherit; color: white; font-family: Arial; font-size: 10pt; color: black; }\n"
//            ".db_container .dbtable { display: table; width: 100%; padding: 0px; }\n"
//            ".dbtable .header { display: table-row; background-color: black; color: white; text-align: left; }\n"
//            ".dbtable .row { display: table-row; background-color: white; text-align: left; }\n"
//            ".dbtable .row_even { background-color: lightgrey; }\n"
//            ".dbtable .col { display: table-cell; background-color: none; text-align: left; padding: 5px; vertical-align: middle; border: 0px; }\n";
        
        string js;
        string body;

        fsmParams fsmParam;
        long paramCount = fsmParam.setParametersMap(paramString);
        string prefix = "", prefixRow = "";
        body = element->getAsHTMLTable(start, end, fsmParam.parametersMap, prefix , prefixRow , css , js);
        
        string callConditionsJS = "\n"
                "function callConditional(selector, func ) {\n"
                "   var elements = document.querySelectorAll(selector) ;\n"
                "   func(elements[ elements.length-1 ] );\n"
                "}\n";
                
        
//        res = res + ""
        string JSBeforeBody = ""
"<script>\n"
"        function numberFormat( num, sep, curr, round, decimal) {\n"
"            if( num < 0 ){\n"
"            num = num*(-1);\n"
"            minus = '-';\n"
"            }else{\n"
"           minus = '';\n"
"            }\n"
"            var prec = '10000000000';\n"
"            prec = +prec.substr(0,round+1);\n"
"            num = +num;\n"
"            num = Math.round(num * prec) / prec;\n"
"            var dec = num.toFixed(round);\n"
"            var start = dec.indexOf('.');\n"
"            dec = dec.substr(start+1, round);\n"
"            var number = Math.floor(num);\n"
"            number = number.toFixed(0);\n"
"            number = typeof number != 'undefined' && number > 0 ? number : '';\n"
"            number = number.replace("
"        new RegExp('^(\\\\d{' + (number.length%3? number.length%3:0) + '})(\\\\d{3})', 'g'), '$1 $2').replace(/(\\d{3})+?/gi, '$1 ').trim();\n"
"            if(typeof sep != 'undefined' && sep != ' ') { number = number.replace(/\\s/g, sep); }\n"
"            number = number == '' ? '0' : number;\n"
"            number = minus + number + decimal + dec + curr;\n"
"            return number;\n"
"        }"
 "</script>\n";
                
        JSBeforeBody = JSBeforeBody + "<script>\n //cusom JS\n" + customJS + "\n</script>\n" ;
        JSBeforeBody= JSBeforeBody + "<script>\n" + callConditionsJS + "\n</script>\n";
        
        
        string bodyWTag = "\n<body>\n"  + body + "\n</body>" ;
        
//        auto parametersMapItr = fsmParam.parametersMap.find( "container_height" );
//        if (parametersMapItr == fsmParam.parametersMap.cend() ){
//            
//        }
//        
//        parametersMapItr = fsmParam.parametersMap.find( "header_lock" );
//        if (parametersMapItr == fsmParam.parametersMap.cend() && parametersMapItr->second == "on" ){
//            
//        }
//

        // define name filemaker file and add js
        string exprFName = "Get(FileName)";
        fmx::DataUniquePtr resultPtr;
        fmx::errcode error = 0;
        fmx::TextUniquePtr exprFnamePtr;
        exprFnamePtr->Assign(exprFName.c_str(), fmx::Text::kEncoding_UTF8 );
        error = environment.Evaluate ( *exprFnamePtr, *resultPtr ) ;
        string fmPath = "var fmPath = 'fmp://$/" + TextAsUTF8String( resultPtr->GetAsText() ) + "'; \n" ;
        
        string JSAfterBody = "\n<script>\n" + fmPath + js + "\n</script>\n";
        
        
//        res = res + "\n<script>\n" + fmPath + js + "\n</script>\n</body>\n</html>";
        
        string head = ""
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"utf-8\">\n";
        
        css = "<style>\n" + css + "\n</style>\n" ;
        
        string res = head + css + "</head>\n" + JSBeforeBody + bodyWTag + JSAfterBody +  "\n</html>";
        
        resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
        
        
        // create new dataset with part of html if need and save it
        std::string newDataSetName;
        dataSetBase *elementNew = nullptr;
        dataSetBase *elementPart = nullptr;
        int newDSSign = 0;
        if (parameters.Size()>5){
            newDataSetName = TextAsUTF8String( parameters.AtAsText(5) );
            newDSSign = enviroment->createElemnet(newDataSetName, dataTypeElement, "", "", 1, "");
            elementNew = enviroment->getElemnet(newDataSetName);
            
            if (elementNew) {
                
                unique_ptr<dataSetBase> dsCSS(new dataSetText("CSS"));
                elementNew->pushback(move(dsCSS));
                elementPart = elementNew->getELementPtrByIdx(0);
                elementPart->pushback(css);
                
                unique_ptr<dataSetBase> dsJS1(new dataSetText("JSBeforeBody"));
                elementNew->pushback(move(dsJS1));
                elementPart = elementNew->getELementPtrByIdx(1);
                elementPart->pushback(JSBeforeBody);
                
                unique_ptr<dataSetBase> dsJS2(new dataSetText("JSAfterBody"));
                elementNew->pushback(move(dsJS2));
                elementPart = elementNew->getELementPtrByIdx(2);
                elementPart->pushback(JSAfterBody);
                
                unique_ptr<dataSetBase> dsBody(new dataSetText("Body"));
                elementNew->pushback(move(dsBody));
                elementPart = elementNew->getELementPtrByIdx(3);
                elementPart->pushback(body);
                
            }
        
        }
            

        
    }else{
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
    }
    
    reply.SetAsText ( *resultString, *default_locale );
    
    return 0;
}



// ******************************************************************************************************************
// Group ( dataSetName ; columnName ; newDataSetName) Group Table DataSet by one of column and save to new DataSet
// ********************************************************************************************************************/
fmx::errcode Group ( short /* function_id */, const fmx::ExprEnv&  environment , const fmx::DataVect& parameters, fmx::Data& reply)
{
    //prepare parameters
    std::string dataSetName = TextAsUTF8String( parameters.AtAsText(0) );
    std::string columnName = TextAsUTF8String( parameters.AtAsText(1) );
    std::string newDataSetName = TextAsUTF8String( parameters.AtAsText(2) );

    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    
    dataSetBase *element = enviroment->getElemnet(dataSetName);
    if (!element) {
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    if (element->getType() != dataTypeElement) {
        resultString ->Assign ( "The type of this DataSet incorrect for this function",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    if ( element->find(columnName) <0 ){
        resultString ->Assign ( "The column not found in DataSet",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    //create dataset and get pointer
    enviroment->createElemnet(newDataSetName, dataTypeElement, "", "", 1, "" );
    dataSetBase *elementNew = enviroment->getElemnet(newDataSetName);

    long res = element->group(columnName, elementNew );
    if (res < 0){
        resultString ->Assign ( "The column is incorret",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    fmx::FixPtUniquePtr    resultDouble;
    resultDouble ->AssignDouble( res );
    reply.SetAsNumber( *resultDouble );
        
    return 0;
}
