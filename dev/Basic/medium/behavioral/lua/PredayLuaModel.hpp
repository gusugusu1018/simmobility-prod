//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#pragma once
#include <boost/unordered_map.hpp>
#include "behavioral/params/PersonParams.hpp"
#include "behavioral/params/StopGenerationParams.hpp"
#include "behavioral/params/TimeOfDayParams.hpp"
#include "behavioral/params/TourModeParams.hpp"
#include "behavioral/params/TourModeDestinationParams.hpp"
#include "behavioral/StopType.hpp"
#include "lua/LuaModel.hpp"

namespace sim_mob
{
namespace medium
{

/**
 * Interface between C++ layer and preday discrete choice models specified in Lua scripts.
 * This class contains a separate function for each model.
 *
 * \author Harish Loganathan
 */
class PredayLuaModel: public lua::LuaModel
{
public:
    PredayLuaModel();
    virtual ~PredayLuaModel();

    /**
     * Predicts the types of tours and intermediate stops the person is going to make.
     *
     * @param personParams object containing person and household related variables
     * @param dayPattern map to fill up with the prediction result
     */
    void predictDayPattern(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig>& activityTypes,
                           std::unordered_map<int, bool>& dayPatternTours, std::unordered_map<int, bool>& dayPatternStops) const;

    /**
     * Predicts the number of tours for each type of tour predicted by the day pattern model.
     *
     * @param personParams object containing person and household related variables
     * @param dayPattern map containing the day pattern of this person
     * @param numStops output container for setting tour counts predicted by this function
     */
    void predictNumTours(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig> &activityTypes,
                         std::unordered_map<int, bool>& dayPatternTours, std::unordered_map<int, int>& numTours) const;

    /**
     * For each work tour, if the person has a usual work location, this function predicts whether the person goes to his usual location or some other location.
     *
     * @param personParams object containing person and household related variables
     * @param usualWorkParams parameters specific to attends usual work model
     * @return true if the tour is to a usual work location. false otherwise.
     */
    bool predictUsualWorkLocation(PersonParams& personParams, UsualWorkParams& usualWorkParams) const;

    /**
     * Predicts the mode of transportation for tours to usual locations (education or work tours)
     *
     * @param personParams object containing person and household related variables
     * @param tourModeParams parameters specific to tour mode models
     * @return mode id
     *         (1 for public bus; 2 for MRT/LRT; 3 for private bus; 4 for drive1;
     *      5 for shared2; 6 for shared3+; 7 for motor; 8 for walk; 9 for taxi)
     */
    int predictTourMode(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig> &activityTypes,
                        TourModeParams& tourModeParams) const;

    /**
     * Predicts the mode and destination for the tour together for tours to unusual locations
     *
     * @param personParams object containing person and household related variables
     * @param tourModeDestinationParams parameters specific to tour mode-destination models
     * @return an integer in the range of 1 to 9828 (9 modes * 1092 zones) which represents a combination of a zone (one of 1092) and a mode (one of 9)
     */
    int predictTourModeDestination(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig> &activityTypes,
                                   TourModeDestinationParams& tourModeDestinationParams) const;

    void initializeLogsums(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig>& activityTypes) const;

    /**
     * Computes log sums for all tour types by invoking corresponding functions in tour mode-destination model
     *
     * @param personParams object containing person and household related variables. logsums will be updated in this object
     * @param tourModeDestinationParams parameters specific to tour mode-destination models
     */
    void computeTourModeDestinationLogsum(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig> &activityTypes,
                                          TourModeDestinationParams& tourModeDestinationParams, int zoneSize) const;

    /**
     * Computes log sums for work tour type for persons with fixed work location
     *
     * @param personParams object containing person and household related variables. logsums will be updated in this object
     * @param tourModeParams parameters specific to tour mode model
     */
    void computeTourModeLogsumWork(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig> &activityTypes, TourModeParams& tourModeParams) const;

    void computeTourModeLogsumEducation(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig> &activityTypes, TourModeParams& tourModeParams) const;
    /**
     * Computes logsums from day-pattern tours and day-pattern stops models
     *
     * @param personParams object containing person and household related variables. logsums will be updated in this object
     */
    void computeDayPatternLogsums(PersonParams& personParams) const;

    /**
     * Computes logsums from day pattern binary model. These logsums will be passed to long-term
     *
     * @param personParams object containing person and household related variables. logsums will be updated in this object
     */
    void computeDayPatternBinaryLogsums(PersonParams& personParams) const;

    /**
     * Predicts the time window for a tour
     *
     * @param personParams object containing person and household related variables
     * @param tourTimeOfDayParams parameters for the tour time of day model
     * @param tourType type of the tour for which time window is being predicted
     * @return an integer in the range of 1 to 1176 representing a particular time window in the day
     *          (1 = (3.25,3.25), 2 = (3.25,3.75), 3 = (3.25, 4.25), ... , 48 = (3.25, 26.75), 49 = (3.75, 3.75), ... , 1176 = (26.75,26.75))
     */
    int predictTourTimeOfDay(PersonParams& personParams, const std::unordered_map<int, ActivityTypeConfig> &activityTypes,
                             TourTimeOfDayParams& tourTimeOfDayParams, StopType tourType) const;

    /**
     * Predicts the next stop for a tour
     *
     * @param personParam object containing person and household related variables
     * @param isgParams object containing parameters specific to intermediate stop generation model
     * @return an integer from 1 to 4 representing the type of stop that is generated
     *          (1 = WORK, 2 = EDUCATION, 3 = SHOP, 4 = OTHER)
     */
    int generateIntermediateStop(PersonParams& personParams, StopGenerationParams& isgParams) const;

    /**
     * Predicts the mode and destination for an intermediate stop
     *
     * @param personParams object containing person and household related variables
     * @param stopModeDestinationParams parameters specific to stop mode-destination models
     * @return an integer in the range of 1 to 9828 (9 modes * 1092 zones) which represents a combination of a zone (one of 1092) and a mode (one of 9)
     */
    int predictStopModeDestination(PersonParams& personParams, StopModeDestinationParams& imdParams) const;

    /**
     * Predicts the departure/arrival time for a stop
     *
     * @param personParams object containing person and household related variables
     * @param stopTimeOfDayParams parameters for the stop time of day model
     * @return an integer in the range of 1 to 48 representing a particular half-hour time window in the day
     */
    int predictStopTimeOfDay(PersonParams& personParams, StopTimeOfDayParams& stopTimeOfDayParams) const;

    /**
     * Predicts sub tour for a work tour
     *
     * @param personParams object containing person and household related variables
     * @param subTourParams parameters for sub tours model
     * @return an integer in the range 1-5 (1 for work; 2 for education; 3 for shopping; 4 for other; 5 for quit)
     */
    int predictWorkBasedSubTour(PersonParams& personParams, SubTourParams& subTourParams) const;

    /**
     * Predicts the mode and destination for sub-tour
     *
     * @param personParams object containing person and household related variables
     * @param tourModeDestinationParams parameters specific to tour mode-destination models
     * @return an integer in the range of 1 to 9828 (9 modes * 1092 zones) which represents a combination of a zone (one of 1092) and a mode (one of 9)
     */
    int predictSubTourModeDestination(PersonParams& personParams, TourModeDestinationParams& tourModeDestinationParams) const;

    /**
     * Predicts the time window for a sub-tour
     *
     * @param personParams object containing person and household related variables
     * @param subTourParams parameters for the sub-tour time of day model
     * @return an integer in the range of 1 to 1176 representing a particular time window in the day
     *          (1 = (3.25,3.25), 2 = (3.25,3.75), 3 = (3.25, 4.25), ... , 48 = (3.25, 26.75), 49 = (3.75, 3.75), ... , 1176 = (26.75,26.75))
     */
    int predictSubTourTimeOfDay(PersonParams& personParams, SubTourParams& subTourParams) const;

    /**
     * predicts an address within the zone
     *
     * @param znAddressParams parameters pertinent to the zone under consideration
     */
    int predictAddress(ZoneAddressParams& znAddressParams) const;

private:
    /**
     * Inherited from LuaModel
     */
    void mapClasses();
};
} // end namespace medium
} //end namespace sim_mob
