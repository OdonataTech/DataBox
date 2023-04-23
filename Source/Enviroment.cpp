/*
 Dragonfly DataBox Plugin
 Copyright 2020 Dragonfly LLS. All rights reserved.
 */


#include "DataSetBase.h"
#include "DataSetClases.h"
#include "Enviroment.h"
#include "Utilites.h"
#include "dbDataTime.h"


// ********************************************************************************************
// Enviroments functions
// ********************************************************************************************

// addElemnet()
int enviroments::addElemnet(unique_ptr<dataSetBase> element, int replaceSign){
    auto itr = mapDataSets.find(element->getName());
    if (itr != mapDataSets.cend() && !replaceSign )
        return -1;
    else if (itr != mapDataSets.cend())
        mapDataSets.erase(itr);
    mapDataSets.insert( pair<string, unique_ptr<dataSetBase>>(element->getName() , move(element) ) );
    return 1;
}

// createElemnet()
int enviroments::createElemnet(string dataSetName, int dataType,  const string& strInit, string delimeter, int replaceSign, string mask){
    
    if (dataSetName.empty() ) return 0;
    
    int result;
    if (dataType == dataTypeElement){
        unique_ptr<dataSetBase> dsEl(new dataSetElement(dataSetName));
        result = addElemnet(move(dsEl), replaceSign);
    }else if (dataType == dataTypeNumber){
        unique_ptr<dataSetBase> dsNum(new dataSetNumber(dataSetName));
        dsNum->addFromString(strInit, delimeter);
        result = addElemnet(move(dsNum), replaceSign);
    }else if (dataType == dataTypeText){
        unique_ptr<dataSetBase> dsTxt(new dataSetText(dataSetName));
        dsTxt->addFromString(strInit, delimeter);
        result = addElemnet(move(dsTxt), replaceSign);
    }else if (dataType == dataTypeDate || dataType == dataTypeTime || dataType == dataTypeTimestamp ){
        unique_ptr<dataSetBase> dsDT(new dataSetDateTime(dataSetName, dataType));
        dsDT->addFromString(strInit, delimeter, mask);
        result = addElemnet(move(dsDT), replaceSign);

        
    }else{
        result = 0;
    }
    
    return result;
}

// getElemnet()
dataSetBase* enviroments::getElemnet(string name){
    if (name.empty())
        return nullptr;
    
    deque<dataSetPath> path;
    int pathResult = stringToElementsPath(name, path);
    if (pathResult<0){
        return nullptr;
    }
    
    auto itr = mapDataSets.find(path[0].name);
    if (itr == mapDataSets.cend() )
        return nullptr;
    return itr->second->getELementPtrByPath(path);
}

// getElemnetWIdx()s
dataSetBase* enviroments::getElemnetWIdx(string name, long &idx){
    if (name.empty())
        return nullptr;
    
    deque<dataSetPath> path;
    int pathResult = stringToElementsPath(name, path);
    if (pathResult<0){
        idx = -1;
        return nullptr;
    }
    
    if (path[path.size()-1].signIndex == 0)
        idx = -1;
    else
        idx = (long)path[path.size()-1].index;
    
    auto itr = mapDataSets.find(path[0].name);
    if (itr == mapDataSets.cend() ){
        idx = -1;
        return nullptr;
    }
    return itr->second->getELementPtrByPath(path);
    
}


// deleteElemnet()
int enviroments::deleteElement(string name, int releasePtrSign ){
    if (name.empty())
        return 0;
    
    deque<dataSetPath> path;
    int pathResult = stringToElementsPath(name, path);
    if (pathResult<0){
        return 0;
    }
    
    auto itr = mapDataSets.find(path[0].name);
    if (itr == mapDataSets.cend() )
        return 0;
    
    int signIndex = path[path.size()-1].signIndex;
    unsigned long index = path[path.size()-1].index;
    int res = 0;
    
    if (path.size() == 1 && signIndex == 0) {
        if (releasePtrSign)
            itr->second.release();
        mapDataSets.erase(itr);
        res = 1;
    }else if( signIndex ){
        dataSetBase* Ptr = itr->second->getELementPtrByPath(path);
        if (Ptr)
            res = Ptr->deleteByIndex(index, releasePtrSign);
    }else{
        string lastname = path[path.size()-1].name;
        path.pop_back();
        dataSetBase* parentPtr = itr->second->getELementPtrByPath(path);
        if (parentPtr)
            res = parentPtr->deleteByName(lastname, releasePtrSign);
    }
    
    return res;
}

// info()
int enviroments::info( string &elementsInfo ){
    
    unsigned long size = mapDataSets.size();
    if (size == 0 )
        return 0;
    
    ostringstream ost;
    for (auto itr = mapDataSets.begin(); itr != mapDataSets.end(); ++itr ) {
        ost << itr->second->getName() << "; ";
        ost << "Type = " <<  dataSetTypeFromNum( itr->second->getType() ) << "; ";
        ost << "Length = " <<  itr->second->lenght() << "\n";
    }
    elementsInfo = ost.str();
    
    return 1;
}
