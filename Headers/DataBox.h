/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#ifndef DataBox_h
#define DataBox_h


#include <FMWrapper/FMXBinaryData.h>
#include <FMWrapper/FMXCalcEngine.h>
#include <FMWrapper/FMXClient.h>
#include <FMWrapper/FMXData.h>
#include <FMWrapper/FMXDateTime.h>
#include <FMWrapper/FMXExtern.h>
#include <FMWrapper/FMXFixPt.h>
#include <FMWrapper/FMXText.h>
#include <FMWrapper/FMXTextStyle.h>
#include <FMWrapper/FMXTypes.h>

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iterator>
#include <memory>
#include <map>
#include <cmath>
#include <limits>
#include <random>

using namespace std;



#define PLUGIN_NAME "DataBox"        //    default is the project name (Xcode only)
//        in Visual Studio set $(TargetName)

#define PLUGIN_PREFIX "DB"        //    default is the first characters of the function name
#define PLUGIN_ID "DTBX"        //   always must be four characters Plugin ID

#define VERSION_STRING "1.0.1020" // also in xcode project build settings

#define PLUGIN_DESCRIPTION_STRING_ID PLUGIN_NAME " Version: " VERSION_STRING "\n\nA lot of functions for data manipulate in memory"

//#define PLUGIN_DESCRIPTION_STRING_ID "DataBox\nVersion: " VERSION_STRING
//
//#define BUNDLE_VERSION_ID "DataBox"
//#define    VERSION_NUMBER_STRING L"1.0.1"

//#define    AUTO_UPDATE_VERSION L"03030600"

//#define PLUGIN_OPTIONS_STRING "1YnYYnn"    //    ask for everything ( "1nnYnnn" )


/* *************************************************************************************************************
 PLUGIN FUNCTIONS DEFENITIONS
 *****************************************************************************************************************/



#define DB_NewDataSet 101
fmx::errcode NewDataSet ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply );


//void DB_functionSetDescriptions ();
//void DB_functionSetDescriptions (){
//    
//    vector<int> DB_functionID;
//    vector<fmx::ExtPluginType> DB_functions;
//    
//    
//    DB_functionID.push_back(101);
//    DB_functions.push_back(NewDataSet);
//    
//}

#define DB_GetDataSet 102
fmx::errcode GetDataSet ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_Lenght 103
fmx::errcode Lenght ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_AddValues 104
fmx::errcode AddValues ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_MoveDataSet 105
fmx::errcode MoveDataSet ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_IsExist 106
fmx::errcode IsExist ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_Delete 107
fmx::errcode Delete ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_EnviromentInfo 108
fmx::errcode EnviromentInfo ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_Agregate 109
fmx::errcode Agregate ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_GetValue 110
fmx::errcode GetValue ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_SetValue 111
fmx::errcode SetValue ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);

#define DB_NewDataSetAsTable 112
fmx::errcode NewDataSetAsTable ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_AddRows 113
fmx::errcode AddRows ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_Seq 114
fmx::errcode Seq ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_sApply 115
fmx::errcode sApply ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_GetDataSetAsTextTable 116
fmx::errcode GetDataSetAsTextTable ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_GetDataSetAsHTMLTable 117
fmx::errcode GetDataSetAsHTMLtTable ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_GetDataSetAsJSON 118
fmx::errcode GetDataSetAsJSON ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_GetIndexByValue 119
fmx::errcode GetIndexByValue ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_Version 120
fmx::errcode Version ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_Uniq 121
fmx::errcode Uniq ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_Sort 122
fmx::errcode Sort ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_Group 123
fmx::errcode Group ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );

#define DB_SaveTableToTextFile 124
fmx::errcode SaveTableToTextFile ( short /* function_id */, const fmx::ExprEnv& environment , const fmx::DataVect& parameters, fmx::Data& reply );





#define DB_Test 500
fmx::errcode Test ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply);



#endif /* DataBox_hpp */
