/*
 * ParamData.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: Max
 */

#pragma once

#include <string>

namespace sim_mob
{

class ParamData
{
public:
    enum ParamType
    {
        TypeBoolean,
        TypeInt,
        TypeDouble,
        TypeString
    };

    ParamData()
    {
    }
    
    ParamData(const std::string& s);
    ParamData(double& d);
    ParamData(int& i);
    ParamData(bool& b);
    ParamData(const ParamData& source);
    virtual ~ParamData();

public:
    std::string toString()
    {
        return dataString;
    }
    
    double toDouble();
    int toInt();
    bool toBool();

    void setParaFileName(const std::string &file)
    {
        fromWhichFile = file;
    }

private:
    std::string dataString;
    double dataDouble;
    int dataInt;
    bool dataBool;
    std::string fromWhichFile;
    ParamType type;
};

} /* namespace sim_mob */

