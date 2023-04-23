/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */


#define FMX_USE_UNIQUE_PTR 1

#include "DataBox.h"
#include "DataSetClases.h"
#include "Enviroment.h"


#pragma mark -
#pragma mark Prototypes
#pragma mark -

#define MAIN_CONTAINER_TYPE "MAIN"
#define SOUND_CONTAINER_TYPE "snd "
#define JPEG_CONTAINER_TYPE "JPEG"
#define GIF_CONTAINER_TYPE "GIFf"
#define EPS_CONTAINER_TYPE "EPS "
#define META_CONTAINER_TYPE "META"
#define PNG_CONTAINER_TYPE "PNGf"
#define BMP_CONTAINER_TYPE "BMPf"
#define PDF_CONTAINER_TYPE "PDF "

using namespace std;


int initCounter = 1;

const fmx::ptrtype Init ( FMX_ExternCallPtr /* pb */ );
void Shutdown ( void );

const fmx::QuadCharUniquePtr PluginID ( void );
const fmx::TextUniquePtr PluginPrefix ( void );
const fmx::TextUniquePtr PluginOptionsString ( void );

void GetString ( FMX_ExternCallPtr pb );
const fmx::errcode RegisterFunction ( const short functionID, const std::string prototype, const fmx::ExtPluginType function, const std::string description = "" );
void UnregisterFunctions ( void );
const fmx::TextUniquePtr FunctionName ( const fmx::TextUniquePtr& signature );
void NumberOfParameters ( const fmx::TextUniquePtr& signature, short& required, short& optional );


// ******************************************************************************************************************
// Define enviroment object
// ********************************************************************************************************************/
enviroments *enviroment;

// pragma mark generates a warning C4068 in Visual Studio so that warning is disabled in VS

#pragma mark -
#pragma mark Enums, Defines & Globals
#pragma mark -

enum {
	kSPOptionsString = 1
};


enum {
	kSPManyParameters = -1,
	kSPPrefixLength = 2,
    kSPPluginIDLength = 5,	//	kSPPrefixLength = 4 + 1 | not use now
	kSPFirstFunction = 1000
};


enum {
	kSPFirstParameter = 0,
	kSPSecondParameter = 1,
	kSPThirdParameter = 2,
	kSPFourthParameter = 3,
	kSPFifthParameter = 4
};


enum errors {
	kSPNoError = 0,
};





// Globals

short g_next_function;	// used to keep track of the funciton id number


#pragma mark -
#pragma mark Functions
#pragma mark -


typedef std::auto_ptr<std::string> StringAutoPtr;



/* *************************************************************************************************************
SPECIAL FUNCTION FOR INIT AND REGISTRATE PLUGIN FUNCTIONS
*****************************************************************************************************************/

#pragma mark -
#pragma mark Plugin
#pragma mark -


/*
 initalise the plug-in
 perform any setup and register functions
 */


const fmx::ptrtype Init ( FMX_ExternCallPtr /* pb */ )
{

    enviroment = new enviroments;

    fmx::errcode error = kSPNoError;
	fmx::ptrtype enable = kCurrentExtnVersion; // kDoNotEnable to prevent the plugin loading

	// perform any initialisation and set-up globals

	/*
	 register plug-in functions
	 functions must always be registered in the same order (to avoid breaking existing calculation in FM).
	 */

    error = RegisterFunction ( DB_NewDataSet,
                              "NewDataSet( dataSetName ; dataType ; initValues ; delimeter ; replaceSign {; datatymeMask })",
                              NewDataSet,
                              "Create a new object in plugin memory as a vector from the InitValues string separated by the Delimiter"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_NewDataSetAsTable,
                              "NewDataSetAsTable ( dataSetName ; columnNames ; columnTypes ; replaceSign)" ,
                              NewDataSetAsTable,
                              "Create a new object in plugin memory as a table. You must define name and type for each columns."
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_GetDataSet,
                              "GetDataSet( dataSetName )" ,
                              GetDataSet,
                              "Dysplay DataSet as list of each object"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_Lenght,
                              "Lenght( dataSetName )" ,
                              Lenght,
                              "Get DataSet vector number of objects"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_AddValues,
                              "AddValues( dataSetName ; values ; delimeter {; datetymeMask } ) " ,
                              AddValues,
                              "Add values to dataSet from separeted text by delimeter"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_MoveDataSet,
                              "MoveDataSet ( dataSetNameSource ; dataSetNameTarget ; replaceSign )" ,
                              MoveDataSet,
                              "Move dataSet to end of other dataSet vector"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_IsExist,
                              "IsExist ( dataSetName )" ,
                              IsExist,
                              "Return 1 if DataSeet exist in plugin memory, else 0"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_Delete,
                              "Delete ( dataSetName )" ,
                              Delete,
                              "Delete DataSet or value by index . Name is full path to DS. ex: DSName or DSName1$DSName1.1 or DSName1$DSName1.1[2]"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_EnviromentInfo,
                              "EnviromentInfo" ,
                              EnviromentInfo,
                              "Return list of DataSets store in plugin memory"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_Agregate,
                              "Agregate (dataSetName; action {; start; end } )" ,
                              Agregate,
                              "Return aggregated values like min/max/sum/avg for DataSet from diapason"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_GetValue,
                              "GetValue (dataSetValuePath )" ,
                              GetValue,
                              "Return value by full path with index (ex: Parent$Child[3]). For 'dataset' type return name of child element"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_SetValue,
                              "SetValue ( dataSetValuePath ; value )" ,
                              SetValue,
                              "Set value to Dataset by path"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_AddRows,
                              "AddRows ( dataSetName ; tabletext ; row_delimeter ; cell_delimeter {; skipFirstRowSign; datetymeMask ; }  )" ,
                              AddRows,
                              "Add row to table from separeted text by delimeter"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_Seq,
                              "Seq ( dataSetName ; dataType ; start ; end {; step } )" ,
                              Seq,
                              "Create DataSet as sequence from start to end with step (default step = 1)."
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_sApply,
                              "sApply ( dataSetName ; expression { ; newDataSet ; newDataSetType} ) " ,
                              sApply,
                              "Apply expression for each element of vector. Store result to new DataSet or replace source"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_GetDataSetAsTextTable,
                              "GetDataSetAsTextTable ( dataSetName { ; row_delimeter ; cell_delimeter; headerOn ; columnsOrder ; start ; end; } )" ,
                              GetDataSetAsTextTable,
                              "Return DataSet as a separated text by a delimiter"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_GetDataSetAsHTMLTable,
                              "GetDataSetAsHTMLtTable ( dataSetName ; tableParameters { ; start ; end ; customJS ; datasetWHtmlPart } ) " ,
                              GetDataSetAsHTMLtTable,
                              "Return DataSet as HTML table"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_GetDataSetAsJSON,
                              "GetDataSetAsJSON ( dataSetName { ; aloneElemenAsArray  ; pretty } ) " ,
                              GetDataSetAsJSON,
                              "Return DataSet as JSON"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_GetIndexByValue,
                              "GetIndexByValue ( dataSetName ; value { ; startIndex } ) " ,
                              GetIndexByValue,
                              "Return first founded Index of element by Value starting from startIndex"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

    error = RegisterFunction ( DB_Version,
                              "Version" ,
                              Version,
                              "Return vesion number of DataBox PlugIn"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_Uniq,
                              "Uniq ( dataSetFrom ; dataSetTo )" ,
                              Uniq,
                              "Create new DataSet with unique values from dataSet"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_Sort,
                              "Sort ( dataSetName { ; direction ; bycolumns })" ,
                              Sort,
                              "Sort DataSet ASC if direction = 1, else DSC "
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    error = RegisterFunction ( DB_Group,
                              "Group ( dataSetName ; columnName ; newDataSetName)" ,
                              Group,
                              "Group Table DataSet by one of column and save to new DataSet "
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }

	
	error = RegisterFunction ( DB_SaveTableToTextFile,
							  "SaveTableToTextFile ( dataSetName, filepath, { ; row_delimeter ; cell_delimeter; headerOn ; columnsOrder ; start ; end; } )" ,
							  SaveTableToTextFile,
							  "Save DataSet as a separated text by a delimiter to file"
							  );
	if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


	



    error = RegisterFunction ( DB_Test,
                              "Test ( datestr; mask )" ,
                              Test,
                              "for testing"
                              );
    if ( kSPNoError != error ) { enable = (fmx::ptrtype)kDoNotEnable; }


    return enable;

} // Init


/*
 clean up anything set up or allocated in Init
 plug-in functions are un-registered automatically before this function is called
 */

void Shutdown ( void )
{
    delete enviroment;

}


/*
 the main entry point for the plug-in

 calls from fmp go either here or directly to registered plugin function
 see also the options for FMX_ExternCallSwitch in FMXExtern.h

 only edit to add additonal call handlers
 */

void FMX_ENTRYPT FMExternCallProc ( FMX_ExternCallPtr pb )
{

	switch ( pb->whichCall )
	{
		case kFMXT_GetString:
			GetString ( pb );
			break;

		case kFMXT_Init:
			g_next_function = kSPFirstFunction;
			pb->result = Init ( pb );
			break;

		case kFMXT_Shutdown:
			UnregisterFunctions ( );
			Shutdown ( );
			break;

	}

}	// FMExternCallProc


/* ***************************************************************************
 You should not need to edit anything in this section.
 *************************************************************************** */

#pragma mark -
#pragma mark Private Functions
#pragma mark -


// get the plug-in name or options string and hand back to FileMaker

void GetString ( FMX_ExternCallPtr pb )
{
	fmx::TextUniquePtr string;

	switch ( pb->parm1 )
	{
		case kSPOptionsString:
		case kFMXT_OptionsStr:
			string->SetText ( *PluginOptionsString() );
			break;

		case kFMXT_NameStr:
			string->Assign ( PLUGIN_NAME );
			break;
		case kFMXT_AppConfigStr:
            string->Assign ( PLUGIN_DESCRIPTION_STRING_ID );
			break;

//		default:

	}

	string->GetUnicode ( (fmx::unichar16*)(pb->result), 0, fmx::Text::kSize_End );

} // GetString



/*
 register plug-in functions

 RegisterFunction takes three parameters:
 1. the external function signature as it should appear in the calcuation dialogs but
 without the prefix i.e. Explode ( timer ) rather than Bomb_Explode ( timer )
 2.	the plug-in function to call when the function is used in FileMaker
 3. (optional) a description of the function ... default: ""
 */

const fmx::errcode RegisterFunction ( const short functionID, const std::string prototype, const fmx::ExtPluginType function, const std::string description )
{
	fmx::TextUniquePtr underscore;
	underscore->Assign ( "_" );

	fmx::TextUniquePtr function_protoype;
	function_protoype->Assign ( prototype.c_str() );
	function_protoype->InsertText ( *PluginPrefix(), 0 );
	function_protoype->InsertText ( *underscore, kSPPrefixLength );

	fmx::TextUniquePtr function_description;
	function_description->Assign ( description.c_str() );

	fmx::TextUniquePtr name;
	name->SetText ( *FunctionName ( function_protoype ) );

	short required_parameters = 0;
	short optional_rarameters = 0;
	NumberOfParameters ( function_protoype, required_parameters, optional_rarameters );

	const fmx::uint32 function_flags = fmx::ExprEnv::kDisplayInAllDialogs;

	const fmx::errcode error = fmx::ExprEnv::RegisterExternalFunctionEx ( *PluginID(),
																		 functionID,
																		 *name,
																		 *function_protoype,
																		 *function_description,
																		 required_parameters,
																		 required_parameters + optional_rarameters,
																		 function_flags,
																		 function
																 );

	if ( error != kSPNoError ) {
		std::cerr << "Error registering: " << prototype << "! Error #: " << error << std::endl;
	}

	// ++g_next_function;

	return error;

} // RegisterFunction



// unregister all registered functions

void UnregisterFunctions ( void )
{
	for ( short i = kSPFirstFunction ; i < g_next_function ; i++ ) {
		fmx::ExprEnv::UnRegisterExternalFunction ( *PluginID(), i );
	}
}


// automaticlly generate the PluginID from the prefix

const fmx::QuadCharUniquePtr PluginID ( void )
{
    /*
	fmx::TextUniquePtr prefix;
	prefix->SetText ( *PluginPrefix() );
	char buffer[kSPPluginIDLength];
	prefix->GetBytes ( buffer, kSPPluginIDLength );
	fmx::QuadCharUniquePtr id ( buffer[0], buffer[1], buffer[2], buffer[3] );
    */

    std::string plgID = PLUGIN_ID;
    fmx::QuadCharUniquePtr id ( plgID[0], plgID[1], plgID[2], plgID[3] );
    return id;

}


// use the defined prefix if it exists otherwise use the first four characters of the name

const fmx::TextUniquePtr PluginPrefix ( void )
{
	fmx::TextUniquePtr prefix;

#ifdef PLUGIN_PREFIX
	prefix->Assign ( PLUGIN_PREFIX );
#else
	prefix->Assign ( PLUGIN_NAME );
	prefix->DeleteText ( kSPPrefixLength );
#endif

	return prefix;
}


// use the options string defined above otherwise turn everything on

const fmx::TextUniquePtr PluginOptionsString ( void )
{
	fmx::TextUniquePtr optionsString;

#ifdef PLUGIN_OPTIONS_STRING
	optionsString->Assign ( PLUGIN_OPTIONS_STRING );
#else
	optionsString->Assign ( "1YnYYnn" );
#endif

	//optionsString->InsertText ( *PluginPrefix(), 0 );

    fmx::TextUniquePtr plgID;
    plgID->Assign (  PLUGIN_ID );

	optionsString->InsertText ( *plgID, 0 );

	return optionsString;
}


// extract the function name from a function signature/prototype

const fmx::TextUniquePtr FunctionName ( const fmx::TextUniquePtr& signature )
{

	fmx::TextUniquePtr separator;
	separator->Assign ( "(" );

	fmx::uint32 parameters_start = signature->Find ( *separator, 0 );
	if ( parameters_start == fmx::Text::kSize_Invalid ) {
		parameters_start = fmx::Text::kSize_End;
	} else {

		// there may or may not be spaces between the function name and the bracket

		fmx::TextUniquePtr space;
		space->Assign ( " " );

		fmx::uint32 last = parameters_start - 1;
		while ( signature->Find ( *space, last ) == last ) {
			--last;
		}
		parameters_start = last + 1;
	}

	fmx::TextUniquePtr name;
	name->SetText ( *signature, 0, parameters_start );

	return name;

} // FunctionName


// calculate the number of required and optional parameters from a function signature/prototye

void NumberOfParameters ( const fmx::TextUniquePtr& signature, short& required, short& optional )
{
	required = 0;
	optional = 0;

	fmx::TextUniquePtr separator;
	separator->Assign ( "(" );

	const fmx::uint32 parameters_start = signature->Find ( *separator, 0 );
	if ( parameters_start == fmx::Text::kSize_Invalid ) {
		return;
	}

	// we have parameters

	fmx::TextUniquePtr semi_colon;
	semi_colon->Assign ( ";" );

	fmx::TextUniquePtr curly_bracket;
	curly_bracket->Assign ( "{" );

	bool has_optional_parameters = false;
	fmx::uint32 next = parameters_start;

	while ( next != fmx::Text::kSize_Invalid ) {

		++next;
		const fmx::uint32 next_semi_colon = signature->Find ( *semi_colon, next );
		const fmx::uint32 next_curly_bracket = signature->Find ( *curly_bracket, next );

		if ( next_curly_bracket < next_semi_colon && has_optional_parameters == false ) {

			next = signature->Find ( *semi_colon, next_curly_bracket + 1 );
			++required;
			has_optional_parameters = true;

			fmx::TextUniquePtr elipsis;
			elipsis->Assign ( "…" );

			if ( signature->Find ( *elipsis, next_curly_bracket + 1 ) != fmx::Text::kSize_Invalid ) {
				optional = -1;
				next = fmx::Text::kSize_Invalid;
			} else {

				fmx::TextUniquePtr faux_elipsis;
				faux_elipsis->Assign ( "..." );

				if ( signature->Find ( *faux_elipsis, next_curly_bracket + 1 ) != fmx::Text::kSize_Invalid ) {
					optional = kSPManyParameters;
					next = fmx::Text::kSize_Invalid;
				}
			}


		} else {
			next = next_semi_colon;

			if ( has_optional_parameters == true ) {
				++optional;
			} else {
				++required;
			}
		}

	}

} // NumberOfParameters


