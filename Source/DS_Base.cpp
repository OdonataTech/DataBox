/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */


#include "DataSetClases.h"
#include "Utilites.h"

//getELementPtrByPath
dataSetBase* dataSetBase::getELementPtrByPath (deque<dataSetPath> path ){
    if (path[0].name != name || path.size() > 1) {
        return nullptr;
    }else {
        return this;
    }
};

// saveTableToFile
string dataSetBase::saveTableToFile(string filePath, unsigned long start, unsigned long end, string rowDelimeter, string colDelimeter, string columnsOrder, int headerOn ){
	
	if (end >size-1) end = size-1;
	if ( size==0 || start <0 || start > size-1 || end < 0 || end < start) return "";
	
	std::ofstream file( filePath, std::ios::out | std::ios::trunc  );
	if ( !file.is_open()) return "Incorrect file path";
	
	if(headerOn) file << name << rowDelimeter;

	unsigned long rows = 0;
	for (unsigned long i=start; i<=end; ++i) {
		file << getValueAsString(i, "");
		if (i < end) file << rowDelimeter;
		++rows;
	}
	file.close();
	
	ostringstream ost;
	ost << rows << " row saved";
	return ost.str();
}

// getValueAsCell
string dataSetBase::getValueAsCell(unsigned long index, map<string, string>& parameters, string &prefix, string &prefixRow, string jsID, int colNum, dataSetBase *parentPtr){
    
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
    
    if (index >=0 && index < size ) {
        string res = getValueAsString(index, format);
        if (res.empty() || res == " " )
            res = "&nbsp";
        ost << res;
    }
    
    
    ost << "</div>";
    
    return ost.str();
}




//getAsHTMLTable
string dataSetBase::getAsHTMLTable(unsigned long start, unsigned long end, map<string, string>& parametersMap, string &prefix, string &prefixRow, string &css, string &js){
    
    ostringstream ost, cnStr;
    ostringstream cssSelector;
    string mainjsID;
    
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
 
    if (end>size-1)
        end = size-1;
    

    // create columns class
    cnStr.str("");
    cnStr << prefix << "columns.1" << "." ;
    parametersMapItr = parametersMap.find(cnStr.str() + "class" );
    if (parametersMapItr != parametersMap.cend() ){
        cnStr.str("");
        cnStr << ".dbtable_" + mainjsID + "  .col_add_1_" << jsID  << " {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }
    cnStr.str("");
    cnStr << prefix << "columns.1" << "." ;
    parametersMapItr = parametersMap.find(cnStr.str() + "class_hover" );
    if (parametersMapItr != parametersMap.cend() ){
        cnStr.str("");
        cnStr << ".dbtable_" + mainjsID + "  .col_add_1_" << jsID  << ":hover {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }
    cnStr.str("");
    cnStr << prefix << "columns.1" << "." ;
    parametersMapItr = parametersMap.find(cnStr.str() + "class_header" );
    if (parametersMapItr != parametersMap.cend() ){
        cnStr.str("");
        cnStr << ".dbtable_" + mainjsID + "  .col_header_1_" << jsID  << " {" << parametersMapItr->second  << "}";
        css = css + cnStr.str() + "\n";
    }

    
    //create header
    cnStr.str("");
    cnStr << prefix << "columns.1" << "." ;
    parametersMapItr = parametersMap.find(cnStr.str() + "header");
    if (parametersMapItr != parametersMap.cend() && parametersMapItr->second == "on" ){
        ost << "<div class=\"dbtable_" + mainjsID + "\" >\n";
        ost << "<div id=\"dbtable_header_" << name << "_" << jsID << "\" class=\"header_" + mainjsID + "  header_add_" << jsID << "\" >\n";
        parametersMapItr = parametersMap.find(cnStr.str() + "header_class" );
        if (parametersMapItr != parametersMap.cend() ){
            cnStr.str("");
            cnStr << ".dbtable_" + mainjsID + "  .header_add_" << jsID  << " {" << parametersMapItr->second  << "}";
            css = css + cnStr.str() + "\n";
        }
        string colname;
        parametersMapItr = parametersMap.find(cnStr.str() + "prettyname" );
        if (parametersMapItr != parametersMap.cend() )
            colname = parametersMapItr->second;
        else
            colname = name;
        
        ost << "<div class=\"col_" + mainjsID + "  col_header_1_" << jsID << "\" style=\"" ;

        parametersMapItr = parametersMap.find(cnStr.str() + "width");
        if (parametersMapItr == parametersMap.cend() )
            ost << "width: auto;\" >";
        else
            ost << "width: " << parametersMapItr->second <<"\" >";
        ost << colname << "</div>\n";
        ost <<"\n</div>\n</div>\n";
    }
    
    ost << "<div id=\"dbtable_main_" << name << "_" << jsID << "\" style=\"overflow: auto;\" >\n";
    ost << "<div class=\"dbtable_" + mainjsID + "\" >\n";

    // define FM script for row click
    string fmScript = "";
    string columnPrefix = prefix + "columns.1.";
    parametersMapItr = parametersMap.find(prefix + "row_onclick_script");
    if (parametersMapItr != parametersMap.cend() )
        fmScript = parametersMapItr->second ;
    
    if ( !(size==0 || start <0 || start > size-1 || end < 0 || end < start ) ){
                
        for (unsigned long i=start; i<=end; ++i) {
            ost << "<div class=\"row_" + mainjsID + " ";
            if (i % 2 == 0)
                ost << " row_odd_add_" << jsID  << "\"";
            else
                ost << " row_even row_even_add_" << jsID  << "\"";
            
            if (!fmScript.empty()) {
                ost << " onclick=\"clickbtn_" << mainjsID << "('" << fmScript << "@@row@@" << prefixRow << i+1 ;
                ost << "@@column@@" << name ;
                ost << "@@value@@" << getValueAsString(i, "");
                ost << "')\"";
            }
            ost << ">\n";
            
            ost << getValueAsCell(i,parametersMap, columnPrefix, prefixRow, jsID, 1, this);
            
            cssSelector.str("");
            cssSelector << ".col_add_" << 1 << "_" << jsID ;
            parametersMapItr = parametersMap.find(columnPrefix + "cond_function");
            if (parametersMapItr != parametersMap.cend() ){
                ost << "<script>callConditional('" << cssSelector.str() << "', " << parametersMapItr->second << ")</script>\n";
            }
            
            ost << "\n</div>\n";
        }
    }
    
    ost << "</div>\n</div>\n</div>\n";
    

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
    
    return ost.str();
};


