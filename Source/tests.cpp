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
// Test( testdata )  FOR TESTING
// ********************************************************************************************************************/
fmx::errcode Test ( short /* function_id */, const fmx::ExprEnv& /* environment */, const fmx::DataVect& parameters, fmx::Data& reply){
    
    std::string str = TextAsUTF8String( parameters.AtAsText(0) );
    std::string mask = TextAsUTF8String( parameters.AtAsText(1) );
    
    fmx::TextUniquePtr    resultString;
    fmx::LocaleUniquePtr default_locale;
    
    dataSetBase *element = enviroment->getElemnet(str);
    if (!element) {
        resultString ->Assign ( "DataSet not found",  fmx::Text::kEncoding_UTF8 );
        reply.SetAsText ( *resultString, *default_locale );
        return 0;
    }
    
    string res = element->dysplayOrder();
    

    resultString ->Assign ( res.c_str(),  fmx::Text::kEncoding_UTF8 );
    reply.SetAsText ( *resultString, *default_locale );

    
    return 0;
    
    
    
//
//    ostringstream ost;
//
//
//
//    dbDataTime dt;
//    dt.setDateByMask(str, mask);
//
//    ost << dt.getType()<< "\n";
//    ost << dt.getDay() << "\n";
//    ost << dt.getMonth() << "\n";
//    ost << dt.getYear() << "\n";
//
//    fmx::LocaleUniquePtr locale = fmx::Locale::kType_System;
//    fmx::DateTimeUniquePtr dtfmx;
//
//    if (dt.getType() == "date"){
//        dtfmx->SetNormalizedDate( (short)dt.getMonth(),  (short)dt.getDay(),  (short)dt.getYear());
//        reply.SetAsDate(*dtfmx);
//    }
//    if (dt.getType() == "time"){
//        dtfmx->SetNormalizedTime( (short)dt.getHour(), (short) dt.getMinute(),  (short)dt.getSeconds() );
//        reply.SetAsTime(*dtfmx);
//    }
//    if (dt.getType() == "timestamp"){
//        dtfmx->SetNormalizedDate( (short)dt.getMonth(),  (short)dt.getDay(),  (short)dt.getYear());
//        dtfmx->SetNormalizedTime( (short)dt.getHour(), (short) dt.getMinute(),  (short)dt.getSeconds() );
//        reply.SetAsTimeStamp(*dtfmx);
//    }
//

    
    
    
    
    
    
    //    fmx::LocaleUniquePtr locale1 = fmx::Locale::kType_RUS;
    //    fmx::DateTimeUniquePtr dt1( parameters.AtAsText(0), *locale1 );
    //
    //    ost << "kType_RUS\n" ;
    //    ost << dt1->GetDay() << "\n";
    //    ost << dt1->GetMonth() << "\n";
    //    ost << dt1->GetYear() << "\n";
    //
    //    fmx::LocaleUniquePtr locale2 = fmx::Locale::kType_System;
    //    fmx::DateTimeUniquePtr dt2( parameters.AtAsText(0), *locale2 );
    //
    //    ost << "kType_System\n" ;
    //    ost << dt2->GetDay() << "\n";
    //    ost << dt2->GetMonth() << "\n";
    //    ost << dt2->GetYear() << "\n";
    //
    //    fmx::LocaleUniquePtr locale3 = fmx::Locale::kType_UnicodeStandard;
    //    fmx::DateTimeUniquePtr dt3( parameters.AtAsText(0), *locale3 );
    //
    //    ost << "kType_UnicodeStandard\n" ;
    //    ost << dt3->GetDay() << "\n";
    //    ost << dt3->GetMonth() << "\n";
    //    ost << dt3->GetYear() << "\n";
    //
    //    fmx::LocaleUniquePtr locale4 = fmx::Locale::kType_UnicodeStandard;
    //    fmx::DateTimeUniquePtr dt4( parameters.AtAsText(0), *locale4 );
    //
    //    ost << "kType_ENG\n" ;
    //    ost << dt4->GetDay() << "\n";
    //    ost << dt4->GetMonth() << "\n";
    //    ost << dt4->GetYear() << "\n";
    //
    //
    //
    //    fmx::TextUniquePtr    resultString;
    //    fmx::LocaleUniquePtr default_locale;
    //    resultString ->Assign ( ost.str().c_str(),  fmx::Text::kEncoding_UTF8 );
    ////    reply.SetAsText ( *resultString, *default_locale );
    //
    //    reply.SetAsDate(*dt2);
    //
    return 0;
}

