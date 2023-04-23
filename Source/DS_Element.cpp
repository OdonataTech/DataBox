/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */


#include "DataSetClases.h"
#include "Utilites.h"
#include "Enviroment.h"

extern enviroments *enviroment;


// ********************************************************************************************
// dataSetElement functions
// ********************************************************************************************

// pushback()
int dataSetElement::pushback(unique_ptr<dataSetBase> element) {
    data.push_back(move(element));
    size = data.size();
    order.push_back(size-1);
    return 1;
    
};

// getELementPtrByPath()
dataSetBase* dataSetElement::getELementPtrByPath (deque<dataSetPath> path ) {
    if (path[0].name != name) {
        return nullptr;
    }else if (path.size() == 1) {
        return this;
    } else {
        path.pop_front();
        vector<unique_ptr<dataSetBase>>::iterator itr;
        string nm = path[0].name;
        itr = find_if( data.begin(), data.end(),
                      [&nm](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == nm; }
                      );
        if (itr == data.end()) {
            return nullptr;
        } else {
            return itr->get()->getELementPtrByPath( path ) ;
        }
    }
};

// getELementPtrByIdx
dataSetBase* dataSetElement::getELementPtrByIdx (unsigned long idx) {
    if (idx >=0 && idx < size )
        return data[getRec(idx)]->getELementPtr();
    else
        return nullptr;
}

// getValue()
int dataSetElement::getValue(string &el, unsigned long index) {
    if (index >=0 && index < size ) {
        el = data[getRec(index)]->getName();
        return 1;
    }else {
        el = "";
        return 0;
    }
};

// dysplay()
string dataSetElement::dysplay(unsigned long start, unsigned long end, string delimetr){
    if ( size==0 || start <0 || start > size-1 || end < 0 || end > size-1 || end < start) return "";
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << "\n[" << i+1 << "]";
        ost << data[getRec(i)]->getName() << "\n" ;
        ost << data[getRec(i)]->dysplay(0, data[getRec(i)]->lenght()-1, " " );
        if (i < end) ost << delimetr;
    }
    return ost.str();
};

// find()
long dataSetElement::find(string value){
    if (value.empty()) return 0;
    
    auto itr = find_if( data.begin(), data.end(),
                  [&value](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == value; }
                  );
    if (itr == data.end()) {
        return -1;
    } else {
        return getIdx( (unsigned long)distance( data.begin(), itr) );
    }
    
}

// deleteByIndex()
int dataSetElement::deleteByIndex( unsigned long index, int releasePtrSign ){
    if (index <0 || index > size-1 ) return 0;
    unsigned long recNumToDel = getRec(index);
    if (releasePtrSign)
        data[recNumToDel].release();
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

// deleteByName()
int dataSetElement::deleteByName( string name, int releasePtrSign ){
    if (name.empty()) return 0;
    
    auto itr = find_if( data.begin(), data.end(),
                       [&name](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == name; }
                       );
    if (itr == data.end())
        return 0;
    
    unsigned long recNumToDel  = (unsigned long ) getIdx( (unsigned long)distance( data.begin(), itr) );
    
    if (releasePtrSign)
        itr->release();
    data.erase(itr);
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
string dataSetElement::expressionPrepare (const string & expression, unsigned long index){
    if (index <0 || index > size-1 ) return "";
    
    string expressionNew = expression;
    string elName = name + "$" +data[getRec(index)]->getName();
    ostringstream ostIdx ;
    ostIdx << index+1;
    
    substitute(expressionNew, "_elemvalue_",  elName  );
    substitute(expressionNew, "_elemidx_", ostIdx.str());
    substitute(expressionNew, "'", "\"");
    
    return expressionNew;
}

// dysplayTable
string dataSetElement::dysplayTable(unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn){
 
    string header = "", childColumnsOrder = "", nm;
    ostringstream ost;
    vector<string> columns;
    vector<unique_ptr<dataSetBase>>::iterator itr;
    unsigned long nrow = 0;
    dataSetBase *elementChild;
    
    // find number of table rows and create columns vector
    if (columnsOrder.empty()) {
        for (unsigned long i=0; i<size; ++i) {
            if (nrow < data[getRec(i)]->lenght()) nrow =data[getRec(i)]->lenght();
            columns.push_back(data[getRec(i)]->getName());
        }
    }else{
        substitute(columnsOrder, " ", "");
        stringToVector(columnsOrder, columns, ";");
        for (unsigned long i=0; i<size; ++i) {
            if (nrow < data[getRec(i)]->lenght()) nrow =data[getRec(i)]->lenght();
        }
    }
    if (end >nrow-1) end = nrow;
    
    if ( size==0 || start <0 || end < 0 || end < start) return "";
    
    //  loop by row
    for (unsigned long i=start; i<end; ++i) {
         //  loop by column.
        for ( unsigned long n=0; n<columns.size(); ++n) {
            nm = columns[n];
            itr = find_if( data.begin(), data.end(),
                          [&nm](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == nm; }
                          );
            if (itr != data.end()){
                if ( i == start) header = header + nm + colDelimeter;
                // if it's child dataset
                if (itr->get()->getType() == dataTypeElement) {
                    childColumnsOrder = "";
                    elementChild = itr->get()->getELementPtrByIdx(i);
                    if (elementChild) {
                        // if it's dataset find actual column names
                        if (elementChild->getType() == dataTypeElement) {
                            for( unsigned long s=0; s<columns.size(); ++s){
                                string col = columns[s];
                                string templ = nm + "$" + elementChild->getName() + "$";
                                if( col.substr(0, templ.length() ) == templ ){
                                    col.replace(0, templ.length(), "");
                                    childColumnsOrder = childColumnsOrder + col + ";" ;
                                }
                            }
                        }
                        ost << elementChild->dysplayTable(0, ULONG_MAX, colDelimeter , colDelimeter, childColumnsOrder , 0 );
                        if (n < columns.size()-1) ost << colDelimeter;
                    }
                }else{
                    // if it's just vector
                    ost << itr->get()->dysplayTable(i, i, colDelimeter , colDelimeter, "" , 0 );
                    if (n < columns.size()-1) ost << colDelimeter;
                }
            }
        }
        if (i < end) ost << rowDelimeter;
    }
    
    if (headerOn)
        return header + rowDelimeter + ost.str();
    else
        return ost.str();
    
}


// saveTableToFile
string dataSetElement::saveTableToFile(string filePath, unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn ){

	string header = "", childColumnsOrder = "", nm;
	vector<string> columns;
	vector<unique_ptr<dataSetBase>>::iterator itr;
	unsigned long nrow = 0;
	dataSetBase *elementChild;
	
	// find number of table rows and create columns vector
	if (columnsOrder.empty()) {
		for (unsigned long i=0; i<size; ++i) {
			if (nrow < data[getRec(i)]->lenght()) nrow =data[getRec(i)]->lenght();
			columns.push_back(data[getRec(i)]->getName());
			header = header + data[getRec(i)]->getName();
			if (i < size-1) header = header + colDelimeter;
		}
	}else{
		substitute(columnsOrder, " ", "");
		stringToVector(columnsOrder, columns, ";");
		for (unsigned long i=0; i<size; ++i) {
			if (nrow < data[getRec(i)]->lenght()) nrow =data[getRec(i)]->lenght();
			header = header + data[getRec(i)]->getName();
			if (i < size-1) header = header + colDelimeter;
		}
	}
	if (end > nrow-1) end = nrow-1 ;
	
	if ( size==0 || start <0 || end < 0 || end < start) return "";
	
	std::ofstream file( filePath, std::ios::out | std::ios::trunc  );
	if ( !file.is_open()) return "Incorrect file path";
	
	if(headerOn) {
		file << header << rowDelimeter;
	}

	//  loop by row
	unsigned long rows = 0;
	for (unsigned long i=start; i<=end; ++i) {
		 //  loop by column.
		for ( unsigned long n=0; n<columns.size(); ++n) {
			nm = columns[n];
			itr = find_if( data.begin(), data.end(),
						  [&nm](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == nm; }
						  );
			if (itr != data.end()){
				// if it's child dataset
				if (itr->get()->getType() == dataTypeElement) {
					childColumnsOrder = "";
					elementChild = itr->get()->getELementPtrByIdx(i);
					if (elementChild) {
						// if it's dataset find actual column names
						if (elementChild->getType() == dataTypeElement) {
							for( unsigned long s=0; s<columns.size(); ++s){
								string col = columns[s];
								string templ = nm + "$" + elementChild->getName() + "$";
								if( col.substr(0, templ.length() ) == templ ){
									col.replace(0, templ.length(), "");
									childColumnsOrder = childColumnsOrder + col + ";" ;
								}
							}
						}
						file << elementChild->dysplayTable(0, ULONG_MAX, colDelimeter , colDelimeter, childColumnsOrder , 0 );
						if (n < columns.size()-1) file << colDelimeter;
					}
				}else{
					// if it's just vector
					file << itr->get()->dysplayTable(i, i, colDelimeter , colDelimeter, "" , 0 );
					if (n < columns.size()-1) file << colDelimeter;
				}
			}
		}
		if (i < end) file << rowDelimeter;
		++rows;

	}
	
	ostringstream ost;
	ost << rows << " row saved";
	return ost.str();
	
}




// getValueAsCell
string dataSetElement::getValueAsCell(unsigned long index, map<string, string>& parameters, string &prefix, string &prefixRow, string jsID, int colNum, dataSetBase *parentPtr){
    
    ostringstream ost;
    
    auto parametersMapItr = parameters.find("mainjsID");
    string mainjsID = parametersMapItr->second;
    
    ost << "<div class=\"col_" + mainjsID + "  col_add_" << colNum << "_" << jsID  << "\"";
    
    ost << "style=\"";
    auto itr = parameters.find(prefix + "width");
    if (itr == parameters.cend() )
        ost << "width: auto;\"";
    else
        ost << "width: " << itr->second <<"\"";
    
    itr = parameters.find(prefix + "onclick_script");
    if (itr != parameters.cend() ){
        ost << " onclick=\"clickbtn_" << mainjsID << "('" << itr->second  << "@@row@@" << prefixRow << index+1 ;
        string ColumnAsParameter = "";
        long ColumnAsParameterIdx = -1;
        itr = parameters.find(prefix + "onclick_script_column_as_parameter");
        if (itr != parameters.cend() ){
            ColumnAsParameter = itr->second ;
            ColumnAsParameterIdx = parentPtr->find(ColumnAsParameter);
        }
        ost << "@@column@@" << ColumnAsParameter ;
        dataSetBase *ColumnAsParameterElement = parentPtr->getELementPtrByIdx((unsigned long)ColumnAsParameterIdx) ;
        if (ColumnAsParameterElement) {
            ost << "@@value@@" << ColumnAsParameterElement->getValueAsString(index, "");
        }
        ost << "')\"";
    }
    
    string format = "";
    itr = parameters.find(prefix + "format");
    if (itr != parameters.cend() )
        format = itr->second;
    
    ost << ">";

//  don't close div becouse it's empty cell for created table inside

    
    return ost.str();
}



// getAsHTMLTable
string dataSetElement::getAsHTMLTable(unsigned long start, unsigned long end, map<string, string>& parametersMap, string &prefix, string &prefixRow, string &css, string &js){
    
    string nm, columnPrefix, columnPrefixCurCell, nextPrefixRow, mainjsID;
    ostringstream ost, cnStr;
    ostringstream cssSelector;
    unsigned long nrow = 0;
    vector<unique_ptr<dataSetBase>>::iterator elementItr;
    dataSetBase *elementChild;
    dataSetBase *ColumnAsParameterElement;

    
    int jsIDrand = mersenne_rand_int() ;
    cnStr << prefix << jsIDrand;
    string jsID =  cnStr.str();
    substitute(jsID, ".", "_");
    cnStr.str("");
    //  int jsID = rand();
    
    // if it's main (first call) table create js and store first jsID
    if (prefix.empty() ) {
        cnStr << prefix << "_" << jsID;
        mainjsID = cnStr.str();
        cnStr.str("");
        parametersMap.insert(pair<string, string>("mainjsID", mainjsID ) );
        
        
        css  = css + "\n"
            ".db_container_" + mainjsID + " { padding: 0px; width: 100%; height: inherit; color: white; font-family: Arial; font-size: 10pt; color: black; }\n"
            ".db_container_" + mainjsID + "  .dbtable_" + mainjsID + "  { display: table; width: 100%; padding: 0px; }\n"
            ".dbtable_" + mainjsID + "  .header_" + mainjsID + "  { display: table-row; background-color: black; color: white; text-align: left; }\n"
            ".dbtable_" + mainjsID + "  .row_" + mainjsID + "  { display: table-row; background-color: white; text-align: left; }\n"
            ".dbtable_" + mainjsID + "  .row_even_" + mainjsID + "  { background-color: lightgrey; }\n"
            ".dbtable_" + mainjsID + "  .col_" + mainjsID + "  { display: table-cell; background-color: none; text-align: left; padding: 5px; vertical-align: middle; border: 0px; }\n";
        
        
        js = js + ""
        "function clickbtn_" + mainjsID + "( btnid ){\n"
        "var param = btnid.split ('@@');\n"
        "var fmScript = param[0];\n"
        "var fmParam ;\n"
        "if (param[1]){\n"
        "    fmParam = { row: param[2] };\n"
        "    if (param[3]) fmParam[ 'column' ] = param[4];\n"
        "    if (param[5]) fmParam[ 'value' ] = param[6];\n"
        "}\n"
        "var  fmParamJSON = JSON.stringify(fmParam) ;\n"
        "var fmUrlRun = fmPath +'?script=' +  encodeURIComponent ( fmScript ) + '&param=' + encodeURIComponent( fmParamJSON );\n"
        "//alert (fmUrlRun);\n"
        "window.location = fmUrlRun;\n"
        "(event.stopPropagation) ? event.stopPropagation() : event.cancelBubble = true;\n"
        "}\n";
        
        string container_height;
        auto parametersMapItr = parametersMap.find("container_height");
        if (parametersMapItr == parametersMap.cend() ){
            container_height = " document.documentElement.clientHeight  - 10 ";
        }else
            container_height = parametersMapItr->second  ;
        
        parametersMapItr = parametersMap.find("header_lock");
        if (parametersMapItr != parametersMap.cend() && parametersMapItr->second == "on" ){
            js = js + "\n"
            "window.onload = function(){\n"
            "   document.body.style.overflow = 'hidden';\n"
            "   var dbcontainer_height = " + container_height + ";\n"
            "   var dbcontainer = document.getElementById('db_container_" + name + "_" + mainjsID + "');\n"
            "   dbcontainer.style.height = dbcontainer_height + 'px';\n"
            "   var header_height = 0;\n"
            "    if (document.getElementById('dbtable_header_" + name + "_" + mainjsID + "') )\n"
            "       header_height = document.getElementById('dbtable_header_" + name + "_" + mainjsID + "').offsetHeight;\n"
            "    var dbtable_main = document.getElementById('dbtable_main_" + name + "_" + mainjsID + "');\n"
            "    var main_height = Number(dbcontainer_height) - Number(header_height);\n"
            "    dbtable_main.style.height = main_height + 'px';\n"
            "}\n";
        }
        
        
    }else{
        auto parametersMapItr = parametersMap.find("mainjsID");
        mainjsID = parametersMapItr->second;
    }
    
    ost << "<div id=\"db_container_" << name << "_" << jsID << "\" class=\"db_container_" + mainjsID + "  container_add_" << jsID << "\" >\n";
    cnStr.str("");
    auto parametersMapItr = parametersMap.find(prefix + "container_class");
    if (parametersMapItr != parametersMap.cend() ){
        cnStr << ".db_container_" + mainjsID + "   .container_add_" << jsID  << " {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }
    
    // create columns vector
    vector<string> columns;
    unsigned long cn = 1;
    cnStr.str("");
    cnStr << prefix << "columns." << cn ;
    parametersMapItr = parametersMap.find(cnStr.str());
    if (parametersMapItr == parametersMap.cend() ){
        // if params have't columns get all
        for (unsigned long i=0; i<size; ++i) {
            if (nrow < data[getRec(i)]->lenght()) nrow =data[getRec(i)]->lenght();
            columns.push_back(data[getRec(i)]->getName());
        }
    }else{
        // get only columns from parameters
        while (parametersMapItr != parametersMap.cend()) {
            nm = parametersMapItr->second;
            elementItr = find_if( data.begin(), data.end(),
                                 [&nm](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == nm; }
                                 );
            if (elementItr != data.end()){
                columns.push_back(nm);
                if (nrow < elementItr->get()->lenght()) nrow = elementItr->get()->lenght();
            }
            cnStr.str("");
            ++cn;
            cnStr << prefix << "columns." << cn ;
            parametersMapItr = parametersMap.find(cnStr.str());
        }
    }
    nrow = nrow - 1;
    if (end >nrow) end = nrow;


    //create header
    parametersMapItr = parametersMap.find(prefix + "header");
    if (parametersMapItr != parametersMap.cend() && parametersMapItr->second == "on" ){
        ost << "<div class=\"dbtable_" + mainjsID + "\" >\n";
        ost << "<div id=\"dbtable_header_" << name << "_" << jsID << "\" class=\"header_" + mainjsID + "  header_add_" << jsID << "\" >\n";
        parametersMapItr = parametersMap.find(prefix + "header_class" );
        if (parametersMapItr != parametersMap.cend() ){
            cnStr.str("");
            cnStr << ".dbtable_" + mainjsID + "  .header_add_" << jsID  << " {" << parametersMapItr->second  << "}";
            css = css + cnStr.str() + "\n";
        }
        string colname;
        for (unsigned long colNum = 0; colNum < columns.size(); ++colNum) {
            cnStr.str("");
            cnStr << prefix << "columns." << colNum+1 << "." ;
            parametersMapItr = parametersMap.find(cnStr.str() + "prettyname" );
            if (parametersMapItr != parametersMap.cend() )
                colname = parametersMapItr->second;
            else
                colname = columns[colNum];
            ost << "<div class=\"col_" + mainjsID + "  col_header_" << colNum+1 << "_" << jsID << "\" style=\"" ;
            parametersMapItr = parametersMap.find(cnStr.str() + "width");
            if (parametersMapItr == parametersMap.cend() )
                ost << "width: auto;\" >";
            else
                ost << "width: " << parametersMapItr->second <<"\" >";
            ost << colname << "</div>\n";
        }
        ost <<"\n</div>\n</div>\n";
    }
    
    //create columns classes
    for (unsigned long colNum = 0; colNum < columns.size(); ++colNum) {
        cnStr.str("");
        cnStr << prefix << "columns." << colNum+1 << "." ;
        parametersMapItr = parametersMap.find(cnStr.str() + "class" );
        if (parametersMapItr != parametersMap.cend() ){
            cnStr.str("");
            cnStr << ".dbtable_" + mainjsID + "  .col_add_"<< colNum + 1 << "_" << jsID  << " {" << parametersMapItr->second  << "}";
            css = css + cnStr.str() + "\n";
        }
        cnStr.str("");
        cnStr << prefix << "columns." << colNum+1 << "." ;
        parametersMapItr = parametersMap.find(cnStr.str() + "class_hover" );
        if (parametersMapItr != parametersMap.cend() ){
            cnStr.str("");
            cnStr << ".dbtable_" + mainjsID + "  .col_add_" << colNum + 1   << "_" << jsID  << ":hover {" << parametersMapItr->second  << "}";
            css = css + cnStr.str() + "\n";
        }
        cnStr.str("");
        cnStr << prefix << "columns." << colNum+1 << "." ;
        parametersMapItr = parametersMap.find(cnStr.str() + "class_header" );
        if (parametersMapItr != parametersMap.cend() ){
            cnStr.str("");
            cnStr << ".dbtable_" + mainjsID + "  .col_header_"<< colNum + 1 << "_" << jsID  << " {" << parametersMapItr->second  << "}";
            css = css + cnStr.str() + "\n";
        }
    }
    
    ost << "<div id=\"dbtable_main_" << name << "_" << jsID << "\" style=\"overflow: auto;\" >\n";
    ost << "<div class=\"dbtable_" + mainjsID + "\" >\n";

    
    if ( !(size==0 || start <0 || end < 0 || end < start) ){
        
        // define FM script for row click
        string fmScript = "";
        parametersMapItr = parametersMap.find(prefix + "row_onclick_script");
        if (parametersMapItr != parametersMap.cend() )
            fmScript = parametersMapItr->second ;
        
        string ColumnAsParameter = "";
        long ColumnAsParameterIdx = -1;
        parametersMapItr = parametersMap.find(prefix + "row_onclick_script_column_as_parameter");
        if (parametersMapItr != parametersMap.cend() ){
            ColumnAsParameter = parametersMapItr->second ;
            ColumnAsParameterIdx = find(ColumnAsParameter);
        }
        ColumnAsParameterElement = getELementPtrByIdx((unsigned long)ColumnAsParameterIdx) ;

        
        int inlineChildTable = 0;
        parametersMapItr = parametersMap.find(prefix + "inlineChildTable");
        if (parametersMapItr != parametersMap.cend() && parametersMapItr->second == "on" ){
            inlineChildTable = 1;
        }
        
        //  loop by row
        for (unsigned long i=start; i<=end; ++i) {
            
            // create row div with class and click
            ost << "<div class=\"row_" + mainjsID + " ";
            if (i % 2 == 0){
                ost << " row_odd_add_" << jsID  << "\"" ;
            }else{
                ost << " row_even row_even_add_" << jsID  << "\"";
            }
            if (!fmScript.empty()) {
                ost << " onclick=\"clickbtn_" << mainjsID << "('" << fmScript << "@@row@@" << prefixRow << i+1 ;
                ost << "@@column@@" << ColumnAsParameter ;
                if (ColumnAsParameterElement) {
                    ost << "@@value@@" << ColumnAsParameterElement->getValueAsString(i, "");
                }
                ost << "')\"";                    
            }
            ost << ">\n";
            
            // row by column
            for ( int colNum = 0; colNum < (int)columns.size(); ++colNum) {
                nm = columns[(unsigned long)colNum];
                elementItr = find_if( data.begin(), data.end(),
                              [&nm](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == nm; }
                              );
                if (elementItr != data.end()){
                    // if it's child dataset
                    cnStr.str("");
                    cnStr << prefix << "columns." << colNum+1 << "." ;
                    columnPrefix = cnStr.str() ;
                    columnPrefixCurCell = columnPrefix;
                    
                    if (elementItr->get()->getType() == dataTypeElement) {
                        
                        if (inlineChildTable){
                            ost << "</div>\n";
                            ost << "<div class=\"row_" + mainjsID + " " << " row_inline_add_" << jsID  << "\"" << ">\n";
                        }else{
                            ost << elementItr->get()->getValueAsCell(i, parametersMap, columnPrefix, prefixRow, jsID, colNum+1 , this) << "\n";
                        }
                        
                        elementChild = elementItr->get()->getELementPtrByIdx(i);
                        if (elementChild) {
                            cnStr << "table." ;
                            columnPrefix = cnStr.str() ;
                            cnStr.str("");
                            cnStr << prefixRow << i+1 << ".";
                            nextPrefixRow = cnStr.str();
                            ost << elementChild->getAsHTMLTable(0, ULONG_MAX, parametersMap, columnPrefix, nextPrefixRow, css, js);
                        }
                        
                        if ( !inlineChildTable ){
                            ost << "</div>\n";
                        }
                        
                    }else{
                        // if it's just vector
                        ost << elementItr->get()->getValueAsCell(i, parametersMap, columnPrefix, prefixRow, jsID, colNum+1 , this ) << "\n";
                    }
                    
                    cssSelector.str("");
                    cssSelector << ".col_add_" << colNum+1 << "_" << jsID ;
                    parametersMapItr = parametersMap.find(columnPrefixCurCell + "cond_function");
                    if (parametersMapItr != parametersMap.cend() ){
                        ost << "<script>callConditional('" << cssSelector.str() << "', " << parametersMapItr->second << ")</script>\n";
                    }
                    
                    
                }
            }
            ost << "</div>\n";
        }
    }
    
    ost << "\n</div>\n</div>\n</div>\n";
    

    // create additional classes;
    


    cnStr.str("");
    parametersMapItr = parametersMap.find(prefix + "row_class_odd");
    if (parametersMapItr != parametersMap.cend() ){
        cnStr << ".dbtable_" + mainjsID + "  .row_odd_add_" << jsID  << " {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }
    cnStr.str("");
    parametersMapItr = parametersMap.find(prefix + "row_class_even");
    if (parametersMapItr != parametersMap.cend() ){
        cnStr << ".dbtable_" + mainjsID + "  .row_even_add_" << jsID  << " {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }
    cnStr.str("");
    parametersMapItr = parametersMap.find(prefix + "row_class_hover");
    if (parametersMapItr != parametersMap.cend() ){
        cnStr << ".dbtable_" + mainjsID + "  .row_even_add_" << jsID  << ":hover {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
        cnStr.str("");
        cnStr << ".dbtable_" + mainjsID + "  .row_odd_add_" << jsID  << ":hover {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }
    cnStr.str("");
    parametersMapItr = parametersMap.find(prefix + "row_class_inline");
    if (parametersMapItr != parametersMap.cend() ){
        cnStr << ".dbtable_" + mainjsID + "  .row_inline_add_" << jsID  << " {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }

    
    
    return ost.str();
    
}


// dysplayJSON()
string dataSetElement::dysplayJSON(unsigned long start, unsigned long end, int aloneElementAsArray, int level ){
    if ( size==0 || start <0 || start > size-1 || end < 0 || end > size-1 || end < start) return "\"\"";
    
    string indent = "";
    int nextlevel = 0;
    if (level >0 ){
        indent = "\n";
        nextlevel = level + 1;
        for (int n=1; n<=level; ++n)
            indent = indent + "  ";
    }
    
    ostringstream ost;
    for (unsigned long i=start; i<=end; ++i) {
        ost << indent << "\"" << data[getRec(i)]->getName() << "\": " ;
        ost << data[getRec(i)]->dysplayJSON(0, data[getRec(i)]->lenght()-1, aloneElementAsArray, nextlevel);
        if (i < end) ost << ",";
    }
    return "{ " + ost.str() + indent + "}";
}

// getIdxByValue
long  dataSetElement::getIdxByValue( string value,  long start ){
    
    if (value.empty()) return 0;
    
    auto itr = find_if( data.begin()+start, data.end(),
                       [&value](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == value; }
                       );
    if (itr == data.end()) {
        return -1;
    } else {
        return getIdx( (unsigned long)distance( data.begin(), itr) );
        
    }
    
    
}

// sort()
long dataSetElement::sort(int direction) {
    
    if (size == 0) return 0;
    
    if (direction == 1) {
        std::sort( order.begin(), order.end(), [this]( unsigned long a, unsigned long b){ return data[a]->getName() < data[b]->getName() ; }   );
    }else{
        std::sort( order.begin(), order.end(), [this]( unsigned long a, unsigned long b){ return  data[a]->getName() > data[b]->getName() ; }   );
    }
    
    return (long) size ;
    
}

// sort()
long dataSetElement::sort(string bycolumns, string direction ) {
    
    if (size == 0 || bycolumns.empty() ) return 0;
    

    // define vector for sort via max length)
    unsigned long nrow = 0;
    dataSetBase * sortVectorPtr = data[0]->getELementPtr();
    vector<unsigned long> sortVectorOrder;
    for (unsigned long i=0; i<size; ++i) {
        if (nrow < data[i]->lenght()) {
            nrow = data[i]->lenght();
            sortVectorPtr  = data[i]->getELementPtr() ;
        }
    }
    sortVectorOrder = sortVectorPtr->getOrder();
    
    // create ptr vector of columns by sort
    string nm;
    vector<string> bycolumnsVct;
    vector<unique_ptr<dataSetBase>>::iterator itr;
    vector<dataSetBase *> bycolumnsVctPtr;
    substitute(bycolumns, " ", "");
    stringToVector(bycolumns, bycolumnsVct, ";");
    for ( unsigned long n=0; n<bycolumnsVct.size(); ++n) {
        nm = bycolumnsVct[n];
        itr = find_if( data.begin(), data.end(),
                      [&nm](const unique_ptr<dataSetBase> & ptr){ return ptr->getName() == nm; }
                      );
        if (itr != data.end()){
            bycolumnsVctPtr.push_back( itr->get() );
        }
    }
    if ( bycolumnsVctPtr.size() == 0 ) return 0;
    
    //create direction vector
    vector<string> directionVctStr;
    vector<int> directionVct;
    substitute(direction, " ", "");
    stringToVector(direction, directionVctStr, ";");
    for ( unsigned long n=0; n<bycolumnsVctPtr.size(); ++n) {
        if ( n >= directionVctStr.size() ){
            directionVct.push_back( 1 );
        }else{
            int d = atoi(directionVctStr[n].c_str() );
            if ( d == 1) directionVct.push_back( 1 );
            else directionVct.push_back( 0 );
        }
    }

    // sort sortVectorOrder by columnns
    std::sort( sortVectorOrder.begin(), sortVectorOrder.end(), [ &bycolumnsVctPtr, &directionVct ]( unsigned long a, unsigned long b){
        int elementType;
        double valueDouble1, valueDouble2 ;
        string valueString1, valueString2;
        dbDataTime valueDataTime1, valueDataTime2;
        int compare = -1;
        unsigned long n=0;
        do {
            elementType = bycolumnsVctPtr[n]->getType();
            if ( elementType == dataTypeNumber ){
                bycolumnsVctPtr[n]->getValue(valueDouble1, a);
                bycolumnsVctPtr[n]->getValue(valueDouble2, b);
                if ( valueDouble1 < valueDouble2) compare = 1;
                else if ( valueDouble1 > valueDouble2) compare = 0;
            }else if (elementType == dataTypeText ){
                bycolumnsVctPtr[n]->getValue(valueString1, a);
                bycolumnsVctPtr[n]->getValue(valueString2, b);
                if ( valueString1 < valueString2) compare = 1;
                else if ( valueString1 > valueString2) compare = 0;
            }else if (elementType == dataTypeDate ){
                bycolumnsVctPtr[n]->getValue(valueDataTime1, a);
                bycolumnsVctPtr[n]->getValue(valueDataTime2, b);
                if ( valueDataTime1 < valueDataTime2) compare = 1;
                else if ( valueDataTime1 > valueDataTime2) compare = 0;
            }
    
            if (directionVct[n] != 1 && compare == 1)
                compare = 0;
            else if (directionVct[n] != 1 && compare == 0)
                compare = 1;
            
            ++n;
        } while( n< bycolumnsVctPtr.size() && compare < 0 );
                
        if (compare == -1 || compare == 1  )
            return true;
        else
            return false;
        

    } );

    for ( unsigned long n=0; n<size; ++n) {
        data[n]->setOrder(sortVectorOrder);
    }
    
    return (long) sortVectorOrder.size() ;
    
}


// addElement()
dataSetBase* dataSetElement::addElement(string elementName, int elementType) {
    
    string tmpName = "TMPDASASETNAME_1010";
    
    enviroment->createElemnet(tmpName, elementType , "", "", 1, "" );
    dataSetBase *newEl = enviroment->getElemnet(tmpName);
    unique_ptr<dataSetBase> newPtr(newEl);
    pushback(move(newPtr));
    enviroment->deleteElement(tmpName, 1);
    newEl->rename(elementName);
    
    return newEl;
    
}

// group()
long dataSetElement::group(string column, dataSetBase *newDS  ) {
    
    // find vector for grouping by name (column name)
    long columnIdx = find(column);
    dataSetBase* columnEl = getELementPtrByIdx((unsigned long)columnIdx) ;
    unsigned long columnSize = columnEl->lenght();
    int columnType = columnEl->getType();
    if ( columnType == dataTypeElement )
        return -2;
    
    //create datasets for column and details and get pointer
    dataSetBase *groupEl =  newDS->addElement(column, columnType );
    dataSetBase *detailsEl = newDS->addElement("Details", dataTypeElement);
    
    long groupIdx;
    double valueDouble;
    string valueString, cName;
    dbDataTime valueDateTime;
    string rowName;
    dataSetBase *rowEl;
    dataSetBase *colEl;
    dataSetBase *colNew;
    ostringstream ost;
    int signNewGroup;

    for (unsigned long i=0; i<columnSize; ++i) {
        
        //find value in groupEl and if not exist create new record
        signNewGroup = 0;
        if(columnType == dataTypeNumber){
            columnEl->getValue(valueDouble, i);
            groupIdx = groupEl->getIdxByValue(valueDouble, 0 );
            if ( groupIdx == -1 ){
                signNewGroup = 1;
                groupEl->pushback( valueDouble );
            }
            
        }else if(columnType == dataTypeText){
            columnEl->getValue(valueString, i);
            groupIdx = groupEl->getIdxByValue(valueString, 0 );
            if ( groupIdx == -1 ){
                signNewGroup = 1;
                groupEl->pushback( valueString );
            }
            
        }else if(columnType == dataTypeDate || columnType == dataTypeTime || columnType == dataTypeTimestamp ){
            columnEl->getValue(valueDateTime, i);
            groupIdx = groupEl->getIdxByValue(valueDateTime, 0 );
            if ( groupIdx == -1 ){
                signNewGroup = 1;
                groupEl->pushback( valueDateTime );
            }
        }
        
        // create row in Details if it's not exist
        if(signNewGroup == 1){
            groupIdx =(long) groupEl->lenght()-1;
            ost.str("");
            ost << groupIdx+1;
            rowName = "R" + ost.str();
            rowEl = detailsEl ->addElement(rowName, dataTypeElement);
            for (unsigned long j=0; j<size; ++j) {
                rowEl->addElement(data[getRec(j)]->getName(), data[getRec(j)]->getType() );
            }
        }else {
            rowEl = detailsEl->getELementPtrByIdx((unsigned long)groupIdx);
        }
        
        // copy values of row to details
        for (unsigned long j=0; j<size; ++j) {
            colEl = getELementPtrByIdx((unsigned long)j) ;
            colNew = rowEl->getELementPtrByIdx((unsigned long)j) ;
            if (colEl->getType() == dataTypeNumber){
                colEl->getValue(valueDouble, i);
                colNew->pushback(valueDouble);
            }else if(colEl->getType() == dataTypeText){
                colEl->getValue(valueString, i);
                colNew->pushback(valueString);
            }else if(colEl->getType() == dataTypeDate || colEl->getType() == dataTypeTime || colEl->getType() == dataTypeTimestamp ){
                colEl->getValue(valueDateTime, i);
                colNew->pushback(valueDateTime);
            }
        }
        
    }
    
//    return  groupEl->getType() ;
    
    return groupEl->lenght();
    
}
