/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#ifndef dbDataTime_h
#define dbDataTime_h

#include "Utilites.h"


enum dbDataTimeType {
    dbDate = 3,
    dbTime = 4,
    dbTimestamp = 5
};


// ********************************************************************************************
// dbDataTime CLASS
// store and manipulate date, time ot timestamp
// ********************************************************************************************
class dbDataTime {
protected:
    
    int type; // dbDate=3 / dbTime=4 / dbTimestamp = 5
    
    int day;
    int month;
    int year;
    int hour;
    int minute;
    int seconds;
    
    bool empty;
    
    long long timestampNum;
    
    void setTimestampNum() { timestampNum = year *10000000000 +month*100000000 + day*1000000+ hour*10000 +minute*100 + seconds; }
    
public:
    
    dbDataTime(){
        day = 0;
        month = 0;
        year = 0;
        hour = 0;
        minute = 0;
        seconds = 0;
        type = dbTimestamp;
        empty = TRUE;
        setTimestampNum();
    }
    
    dbDataTime (int dd, int mn, int yy ){
        day = dd;
        month = mn;
        year = yy;
        hour = 0;
        minute = 0;
        seconds = 0;
        type = dbDate;
        setTimestampNum();
        if (timestampNum > 0 )  empty = FALSE;
        else  empty = TRUE;
    
    }
    
    dbDataTime (int dd, int mn, int yy, int hh, int mm, int ss ){
        day = dd;
        month = mn;
        year = yy;
        hour = hh;
        minute = mm;
        seconds = ss;
        type = dbTimestamp;
        setTimestampNum();
        if (timestampNum > 0 )  empty = FALSE;
        else  empty = TRUE;
    }
    
    dbDataTime (int time, int hh, int mm, int ss ){
        day = 0;
        month = 0;
        year = 0;
        hour = hh;
        minute = mm;
        seconds = ss;
        type = dbTime;
        setTimestampNum();
        if (timestampNum > 0 )  empty = FALSE;
        else  empty = TRUE;
    }
    
    
    dbDataTime(const dbDataTime &dt ){
        type = dt.type;
        empty = dt.empty;
        setDataTime(dt.day, dt.month, dt.year, dt.hour, dt.minute, dt.seconds);
    }
    
    
    
    void setDataTime (int dd, int mn, int yy, int hh, int mm, int ss ){
        day = dd;
        month = mn;
        year = yy;
        hour = hh;
        minute = mm;
        seconds = ss;
        setTimestampNum();
        if (timestampNum > 0 )  empty = FALSE;
        else  empty = TRUE;
    }
    
    
    int setDateByMask(string str, string mask){
        day = 0;
        month = 0;
        year = 0;
        hour = 0;
        minute = 0;
        seconds = 0;
        size_t pos = 0;
        string value = "";
        int flag_y = 0, flag_h = 0;
        
        pos = mask.find("Y", pos);
        if (pos != string::npos) {
            flag_y = 1;
            while (pos != string::npos) {
                if (pos < str.length() ) {
                    value = value + str.substr(pos, 1);
                }
                ++pos;
                pos = mask.find("Y", pos);
            }
            if (value.length()==2)
                year = 2000 + atoi(value.c_str());
            else
                year = atoi(value.c_str());
            
            pos = 0;
            value = "";
            pos = mask.find("M", pos);
            while (pos != string::npos) {
                if (pos < str.length() ) {
                    value = value + str.substr(pos, 1);
                }
                ++pos;
                pos = mask.find("M", pos);
            }
            month = atoi(value.c_str());
            
            pos = 0;
            value = "";
            pos = mask.find("D", pos);
            while (pos != string::npos) {
                if (pos < str.length() ) {
                    value = value + str.substr(pos, 1);
                }
                ++pos;
                pos = mask.find("D", pos);
            }
            day = atoi(value.c_str());
        }
        
        pos = 0;
        value = "";
        pos = mask.find("h", pos);
        if (pos != string::npos) {
            flag_h = 1;
            while (pos != string::npos) {
                if (pos < str.length() ) {
                    value = value + str.substr(pos, 1);
                }
                ++pos;
                pos = mask.find("h", pos);
            }
            hour = atoi(value.c_str());
            
            pos = 0;
            value = "";
            pos = mask.find("m", pos);
            while (pos != string::npos) {
                if (pos < str.length() ) {
                    value = value + str.substr(pos, 1);
                }
                ++pos;
                pos = mask.find("m", pos);
            }
            minute = atoi(value.c_str());
            
            pos = 0;
            value = "";
            pos = mask.find("s", pos);
            while (pos != string::npos) {
                if (pos < str.length() ) {
                    value = value + str.substr(pos, 1);
                }
                ++pos;
                pos = mask.find("s", pos);
            }
            seconds = atoi(value.c_str());
        }
        
        if (flag_y == 1 && flag_h == 0) {
            type = dbDate;
        }else if (flag_y == 0 && flag_h == 1)
            type = dbTime;
        else
            type = dbTimestamp;
        
        setTimestampNum();
        if (timestampNum > 0 )  empty = FALSE;
        else  empty = TRUE;
        
        return 0;
        
    }
    
    
    int getDay() {return day; }
    int getMonth() {return month; }
    int getYear() { return year; }
    int getHour() { return hour; }
    int getMinute() {return minute; }
    int getSeconds() {return seconds;}
    long long getTimestampNum() {return timestampNum;}
    
    
    string getType(){
        if (type == dbDate ) return "date";
        if (type == dbTime ) return "time";
        if (type == dbTimestamp ) return "timestamp";
        return "uknown";
    }
    
    
    string getAsString(int outType ){
        if (empty)
            return "";
        
        ostringstream ost;
        
        if (outType == dbDate || outType == dbTimestamp ) {
            ost << year << "-";
            if (month<10) ost << 0 ;
            ost << month << "-";
            if (day<10) ost << 0 ;
            ost << day ;
        }
        
        if ( outType == dbTimestamp  )
            ost << " ";
        
        if (outType == dbTime || outType == dbTimestamp  ) {
            if (hour<10) ost << 0 ;
            ost << hour << ":";
            if (minute<10) ost << 0 ;
            ost << minute << ":";
            if (seconds<10) ost << 0 ;
            ost << seconds ;
        }
        
        return ost.str();
    }

    
    string getAsStringByMask(string mask){
        if (empty)
            return "";
        
        if (mask.empty())
            return getAsString(type);
        
        size_t pos = 0;
        ostringstream ost;

        pos = mask.find("YYYY", 0);
        if (pos != string::npos) {
            ost << year;
            substitute(mask, "YYYY",  ost.str());
        }else {
            pos = mask.find("YY", 0);
            if (pos != string::npos) {
                ost << year ;
                string yr = ost.str();
                yr = yr.substr( yr.size()-2 ) ;
                substitute(mask, "YY",  yr );
            }
        }

        ost.str("");
        pos = mask.find("MM", 0);
        if (pos != string::npos) {
            if (month<10) ost << 0 ;
            ost << month;
            substitute(mask, "MM",  ost.str());
        }else {
            pos = mask.find("M", 0);
            if (pos != string::npos) {
                ost << month;
                substitute(mask, "M",  ost.str());
            }
        }

        ost.str("");
        pos = mask.find("DD", 0);
        if (pos != string::npos) {
            if (day<10) ost << 0 ;
            ost << day;
            substitute(mask, "DD",  ost.str());
        }else {
            pos = mask.find("D", 0);
            if (pos != string::npos) {
                ost << day;
                substitute(mask, "D",  ost.str());
            }
        }
        
        ost.str("");
        pos = mask.find("hh", 0);
        if (pos != string::npos) {
            if (hour<10) ost << 0 ;
            ost << hour;
            substitute(mask, "hh",  ost.str());
        }else {
            pos = mask.find("h", 0);
            if (pos != string::npos) {
                ost << hour;
                substitute(mask, "h",  ost.str());
            }
        }

        ost.str("");
        pos = mask.find("mm", 0);
        if (pos != string::npos) {
            if (minute<10) ost << 0 ;
            ost << minute;
            substitute(mask, "mm",  ost.str());
        }else {
            pos = mask.find("m", 0);
            if (pos != string::npos) {
                ost << minute;
                substitute(mask, "m",  ost.str());
            }
        }
        
        ost.str("");
        pos = mask.find("ss", 0);
        if (pos != string::npos) {
            if (seconds<10) ost << 0 ;
            ost << seconds;
            substitute(mask, "ss",  ost.str());
        }else {
            pos = mask.find("s", 0);
            if (pos != string::npos) {
                ost << seconds;
                substitute(mask, "s",  ost.str());
            }
        }
        
        return mask;
    }
    



    bool operator< ( const dbDataTime &dt) const {
        return timestampNum < dt.timestampNum;
    }

    bool operator> ( const dbDataTime &dt) const {
        return timestampNum > dt.timestampNum;
    }
  
    bool operator<= ( const dbDataTime &dt) const {
        return timestampNum <= dt.timestampNum;
    }
    
    bool operator>= ( const dbDataTime &dt) const {
        return timestampNum >= dt.timestampNum;
    }
    
    bool operator== ( const dbDataTime &dt) const {
        return timestampNum == dt.timestampNum;
    }
    
    bool operator!= ( const dbDataTime &dt) const {
        return timestampNum != dt.timestampNum;
    }

    
};



#endif


