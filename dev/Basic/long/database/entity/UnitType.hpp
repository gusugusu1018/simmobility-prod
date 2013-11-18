//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

/* 
 * File:   UnitType.hpp
 * Author: Pedro Gandola <pedrogandola@smart.mit.edu>
 *
 * Created on 1 July, 2013, 3:04 PM
 */
#pragma once

#include "Common.hpp"
#include "Types.hpp"

namespace sim_mob {

    namespace long_term {

        class UnitType {
        public:
            UnitType(BigSerial id = INVALID_ID, const std::string& name = "", 
                     int type = 0);
            virtual ~UnitType();

            /**
             * gets unique identifier of the Building Type.
             * @return id.
             */
            BigSerial getId() const;

            /**
             * gets name of the type of the building.
             * @return id.
             */
            const std::string& getName() const;

            /**
             * gets the type that differentiates unit types.
             * @return type;
             */
            int getType() const;
            
            /**
             * Assign operator.
             * @param source to assign.
             * @return UnitType instance reference.
             */
            UnitType& operator=(const UnitType& source);

            /**
             * Operator to print the Building data.  
             */
            friend std::ostream& operator<<(std::ostream& strm, const UnitType& data) {
                return strm << "{"
                        << "\"id\":\"" << data.id << "\","
                        << "\"name\":\"" << data.name << "\""
                        << "\"type\":\"" << data.name << "\","
                        << "}";
            }

        private:
            friend class UnitTypeDao;
        private:
            BigSerial id;
            std::string name;
            int type;
        };
    }
}
