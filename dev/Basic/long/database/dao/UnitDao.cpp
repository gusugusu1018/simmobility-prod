//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

/* 
 * File:   BuildingDao.cpp
 * Author: Pedro Gandola <pedrogandola@smart.mit.edu>
 * 
 * Created on May 7, 2013, 3:59 PM
 */

#include "UnitDao.hpp"
#include "DatabaseHelper.hpp"

using namespace sim_mob::db;
using namespace sim_mob::long_term;

UnitDao::UnitDao(DB_Connection& connection)
: SqlAbstractDao<Unit>(connection, DB_TABLE_UNIT,
DB_INSERT_UNIT, DB_UPDATE_UNIT, DB_DELETE_UNIT,
DB_GETALL_UNIT, DB_GETBYID_UNIT) {}

UnitDao::~UnitDao() {
}

void UnitDao::fromRow(Row& result, Unit& outObj) {
    outObj.id = result.get<BigSerial>(DB_FIELD_ID, INVALID_ID);
    outObj.buildingId = result.get<BigSerial>(DB_FIELD_BUILDING_ID, INVALID_ID);
    outObj.typeId = result.get<BigSerial>(DB_FIELD_TYPE_ID, INVALID_ID);
    outObj.postcodeId = result.get<BigSerial>(DB_FIELD_POSTCODE_ID, INVALID_ID);
    outObj.tazId = result.get<BigSerial>(DB_FIELD_TAZ_ID, INVALID_ID);
    outObj.storey = result.get<int>(DB_FIELD_STOREY, 0);
    outObj.floorArea = result.get<double>(DB_FIELD_FLOOR_AREA, 0);
    outObj.rent = result.get<double>(DB_FIELD_RENT, 0);
    outObj.location.latitude = result.get<double>(DB_FIELD_LATITUDE, 0);
    outObj.location.longitude = result.get<double>(DB_FIELD_LONGITUDE, 0);
}

void UnitDao::toRow(Unit& data, Parameters& outParams, bool update) {
}
