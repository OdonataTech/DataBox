/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */

#ifndef Enviroment_h
#define Enviroment_h

using namespace std;


// *************************************************************************************************************************************************
// CLASS ENVIROMENTS
// main class for store user's data
// *************************************************************************************************************************************************
class enviroments {
    
private:
    map<string, unique_ptr<dataSetBase>> mapDataSets;
    
public:
    
    
    int addElemnet(unique_ptr<dataSetBase> element, int replaceSign);
    
    int createElemnet(string elementName, int elementType,  const string& strInit, string delimetr, int replaceSign, string mask);
    
    dataSetBase* getElemnet(string name);
    dataSetBase* getElemnetWIdx(string name, long &idx);
    
    int deleteElement(string name, int clearptrsign );
    
    int info( string &elementsInfo );
    
    
};





#endif /* DataSet_hpp */



