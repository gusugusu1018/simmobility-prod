//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#include "conf/settings/DisableMPI.h"
#include "util/LangHelpers.hpp"

#ifndef SIMMOB_DISABLE_MPI

#include "entities/roles/driver/Driver.hpp"
#include "entities/BusController.hpp"
#include "entities/roles/pedestrian/Pedestrian2.hpp"
#include "entities/roles/pedestrian/Pedestrian.hpp"
#include "entities/roles/activityRole/ActivityPerformer.hpp"
#include "partitions/PackageUtils.hpp"
#include "partitions/UnPackageUtils.hpp"

#include "entities/roles/driver/DriverUpdateParams.hpp"

#include "geospatial/network/Lane.hpp"
#include "geospatial/network/Node.hpp"
#include "geospatial/network/RoadSegment.hpp"

#include "util/GeomHelpers.hpp"
#include "partitions/ParitionDebugOutput.hpp"

/*
 * \author Xu Yan
 */

namespace sim_mob {

/**
 * Serialization for Class ActivityPerformer
 */
void sim_mob::ActivityPerformer::pack(PackageUtils& packageUtil) {
}

void sim_mob::ActivityPerformer::unpack(UnPackageUtils& unpackageUtil) {
}

void sim_mob::ActivityPerformer::packProxy(PackageUtils& packageUtil) {
}

void sim_mob::ActivityPerformer::unpackProxy(UnPackageUtils& unpackageUtil) {
}

/**
 * Serialization for Class Pedestrain2
 */
void sim_mob::Pedestrian2::pack(PackageUtils& packageUtil) {
}

void sim_mob::Pedestrian2::unpack(UnPackageUtils& unpackageUtil) {
}

void sim_mob::Pedestrian2::packProxy(PackageUtils& packageUtil) {
}

void sim_mob::Pedestrian2::unpackProxy(UnPackageUtils& unpackageUtil) {
}

/**
 * Serialization for Class BusController
 */

void sim_mob::BusController::pack(PackageUtils& packageUtil) {
}

void sim_mob::BusController::unpack(UnPackageUtils& unpackageUtil) {
}

void sim_mob::BusController::packProxy(PackageUtils& packageUtil) {
}

void sim_mob::BusController::unpackProxy(UnPackageUtils& unpackageUtil) {
}

/**
 * Serialization for Class Pedestrain
 */
void sim_mob::Pedestrian::pack(PackageUtils& packageUtil) {
    //Part 1

    packageUtil<<(speed);
    packageUtil<<(xVel);
    packageUtil<<(yVel);
    packageUtil<<(goalInLane);

    //Part 2
    packageUtil<<(sigColor);
//  packageUtil<<(curCrossingID);
    packageUtil<<(startToCross);
    packageUtil<<(cStartX);
    packageUtil<<(cStartY);
    packageUtil<<(cEndX);
    packageUtil<<(cEndY);
    //packageUtil<<(firstTimeUpdate);

    std::cout << " CCC " << std::endl;

//  packageUtil<<(interPoint);
    packageUtil<<(xCollisionVector);
    packageUtil<<(yCollisionVector);

    GeneralPathMover::pack(packageUtil, &fwdMovement);
//  packageUtil.packGeneralPathMover(&fwdMovement);

    if (prevSeg) {
        bool hasSegment = true;
        packageUtil<<(hasSegment);
        RoadSegment::pack(packageUtil, prevSeg);
//      packageUtil.packRoadSegment(prevSeg);
    } else {
        bool hasSegment = false;
        packageUtil<<(hasSegment);
    }

    packageUtil<<(isUsingGenPathMover);

    //no need to package params, params will be rebuild in the next time step
    //packageUtil.packPedestrianUpdateParams(params);
}

void sim_mob::Pedestrian::unpack(UnPackageUtils& unpackageUtil) {
    //Part 1
    unpackageUtil >> speed;
    unpackageUtil >> xVel;
    unpackageUtil >> yVel;
//  unpackageUtil >> goal;
    unpackageUtil >> goalInLane;

    int value = 0;
    unpackageUtil >> value;
//  currentStage = PedestrianStage(value);

    //Part 2
    unpackageUtil >> sigColor;
//  unpackageUtil >> curCrossingID;
    unpackageUtil >> startToCross;
    unpackageUtil >> cStartX;
    unpackageUtil >> cStartY;
    unpackageUtil >> cEndX;
    unpackageUtil >> cEndY;
//  unpackageUtil >> interPoint;
//  interPoint = *(unpackageUtil.unpackPoint2D());

    unpackageUtil >> xCollisionVector;
    unpackageUtil >> yCollisionVector;

//  xCollisionVector = unpackageUtil.unpackBasicData<double> ();
//  yCollisionVector = unpackageUtil.unpackBasicData<double> ();

    GeneralPathMover::unpack(unpackageUtil, &fwdMovement);
//  unpackageUtil.unpackGeneralPathMover(&fwdMovement);
    //fwdMovement = *(unpackageUtil.unpackGeneralPathMover());
    bool hasSegment = false;
    unpackageUtil >> hasSegment;
    if (hasSegment) {
        prevSeg = RoadSegment::unpack(unpackageUtil);
//      prevSeg = unpackageUtil.unpackRoadSegment();
    }

    unpackageUtil >> isUsingGenPathMover;
//  isUsingGenPathMover = unpackageUtil.unpackBasicData<bool> ();

    //no need to unpackage params, params will be rebuild in the next time step
    //unpackageUtil.unpackPedestrianUpdateParams(params);
}

void sim_mob::Pedestrian::packProxy(PackageUtils& packageUtil) {
    //Part 1
    //std::cout << "1-1-6-1" << std::endl;
    //Part 1
    packageUtil<<(speed);
    packageUtil<<(xVel);
    packageUtil<<(yVel);
//  packageUtil<<(goal);
    packageUtil<<(goalInLane);
//  packageUtil<<(currentStage);

    //Part 2
    packageUtil<<(sigColor);
//  packageUtil<<(curCrossingID);
    packageUtil<<(startToCross);
    packageUtil<<(cStartX);

    std::cout << " DDD " << std::endl;

    packageUtil<<(cStartY);
    packageUtil<<(cEndX);
    packageUtil<<(cEndY);
    //packageUtil<<(firstTimeUpdate);
//  packageUtil<<(interPoint);

    packageUtil<<(xCollisionVector);
    packageUtil<<(yCollisionVector);

    GeneralPathMover::pack(packageUtil, &fwdMovement);
//  packageUtil.packGeneralPathMover(&fwdMovement);
//
//  if(prevSeg)
//  {
//      bool hasSegment = true;
//      packageUtil<<(hasSegment);
//      packageUtil.packRoadSegment(prevSeg);
//  }
//  else
//  {
//      bool hasSegment = false;
//      packageUtil<<(hasSegment);
//  }

    packageUtil<<(isUsingGenPathMover);

}

void sim_mob::Pedestrian::unpackProxy(UnPackageUtils& unpackageUtil) {
//Part 1

//  if(this->getParent()->getId() > 1000)
//  std::cout << "1-1-6-2" << std::endl;
//
    unpackageUtil >> speed;
    unpackageUtil >> xVel;
    unpackageUtil >> yVel;

//  speed = unpackageUtil.unpackBasicData<double> ();
//  xVel = unpackageUtil.unpackBasicData<double> ();
//  yVel = unpackageUtil.unpackBasicData<double> ();

//  if(this->getParent()->getId() > 1000)
//  std::cout << "1-1-6-23" << std::endl;

//  unpackageUtil >> goal;
    unpackageUtil >> goalInLane;

    int value = 0;
    unpackageUtil >> value;
//  currentStage = PedestrianStage(value);

//  unpackageUtil >> speed;
//
//  goal = *(unpackageUtil.unpackPoint2D());
//  goalInLane = *(unpackageUtil.unpackPoint2D());
//  int value = unpackageUtil.unpackBasicData<int> ();
//  currentStage = PedestrianStage(value);
//  if(this->getParent()->getId() > 1000)
//  std::cout << "1-1-6-4" << std::endl;
//
//  bool hasSignal = unpackageUtil.unpackBasicData<bool> ();
//  if (hasSignal) {
//      Point* signal_location = unpackageUtil.unpackPoint2D();
//      trafficSignal = sim_mob::getSignalBasedOnNode(signal_location);
//  }
//
//  bool hasCrossing = unpackageUtil.unpackBasicData<bool> ();
//  if (hasCrossing) {
//      currCrossing = unpackageUtil.unpackCrossing();
//  }
//  if(this->getParent()->getId() > 1000)
//  std::cout << "1-1-6-5" << std::endl;
    //Part 2

    unpackageUtil >> sigColor;
//  unpackageUtil >> curCrossingID;
    unpackageUtil >> startToCross;
    unpackageUtil >> cStartX;
    unpackageUtil >> cStartY;
    unpackageUtil >> cEndX;
    unpackageUtil >> cEndY;

//  sigColor = unpackageUtil.unpackBasicData<int> ();
//  curCrossingID = unpackageUtil.unpackBasicData<int> ();
//  startToCross = unpackageUtil.unpackBasicData<bool> ();
//  if(this->getParent()->getId() > 1000)
//      std::cout << "1-1-6-5-1" << std::endl;

//  cEndX = unpackageUtil.unpackBasicData<double> ();
//  cEndY = unpackageUtil.unpackBasicData<double> ();
    //firstTimeUpdate = unpackageUtil.unpackBasicData<bool> ();
//  if(this->getParent()->getId() > 1000)
//  std::cout << "1-1-6-6" << std::endl;

//  unpackageUtil >> interPoint;
//  interPoint = *(unpackageUtil.unpackPoint2D());

    unpackageUtil >> xCollisionVector;
    unpackageUtil >> yCollisionVector;

//  xCollisionVector = unpackageUtil.unpackBasicData<double> ();
//  yCollisionVector = unpackageUtil.unpackBasicData<double> ();

    GeneralPathMover::unpack(unpackageUtil, &fwdMovement);
//  unpackageUtil.unpackGeneralPathMover(&fwdMovement);
//  if(this->getParent()->getId() > 1000)
//  std::cout << "1-1-6-7" << std::endl;
//  //fwdMovement = *(unpackageUtil.unpackGeneralPathMover());
//  bool hasSegment = unpackageUtil.unpackBasicData<bool> ();
//  if (hasSegment) {
//      prevSeg = unpackageUtil.unpackRoadSegment();
//  }

    unpackageUtil >> isUsingGenPathMover;
//  isUsingGenPathMover = unpackageUtil.unpackBasicData<bool> ();
//  if(this->getParent()->getId() > 1000)
//  std::cout << "1-1-6-8" << std::endl;

}

/**
 * Serialize Class Driver
 */
void sim_mob::Driver::pack(PackageUtils& packageUtil) {
    //Part 1
    Lane::pack(packageUtil, currLane_.get());
//  packageUtil.packLane(currLane_.get());
    packageUtil<<(currLaneOffset_.get());
    packageUtil<<(currLaneLength_.get());
    packageUtil<<(isInIntersection.get());

    packageUtil<<(latMovement.get());
    packageUtil<<(fwdVelocity.get());
    packageUtil<<(latVelocity.get());
    packageUtil<<(fwdAccel.get());

    //part 2
    //no need to package params, params will be rebuild in the next time step
    //packageUtil.packDriverUpdateParams(params);
    Vehicle::pack(packageUtil, vehicle);
    //packageUtil.packVehicle(vehicle);
    bool hasSomething = false;
    if (intModel) {
        SimpleIntDrivingModel* simple_model = dynamic_cast<SimpleIntDrivingModel*> (intModel);
        if (simple_model) {
            hasSomething = true;
            packageUtil<<(hasSomething);
            SimpleIntDrivingModel::pack(packageUtil, simple_model);
//          packageUtil.packIntersectionDrivingModel(simple_model);
        } else {
            packageUtil<<(hasSomething);
        }
    } else {
        packageUtil<<(hasSomething);
    }

    //Part 3
//  packageUtil<<(reacTime_LeadingVehicle);
//  packageUtil<<(reacTime_SubjectVehicle);
//  packageUtil<<(reacTime_Gap);

//  std::cout << "B0027," << reacTime_Gap << std::endl;

//  packageUtil << perceivedVelocity;
//  packageUtil << perceivedVelocityOfFwdCar;
//  packageUtil << perceivedAccelerationOfFwdCar;
    packageUtil << perceivedDistToFwdCar;
//  packageUtil << perceivedTrafficSignalStop;

    packageUtil << (origin.point);
    Node::pack(packageUtil, origin.node);
//  packageUtil.packNode(origin.node);
    packageUtil << (goal.point);
    Node::pack(packageUtil, goal.node);
//  packageUtil.packNode(goal.node);

    //packageUtil<<(firstFrameTick);
    packageUtil<<(maxLaneSpeed);

    //Part 4
    //packageUtil.packLink(desLink);
    packageUtil<<(currLinkOffset);
    packageUtil<<(targetLaneIndex);
    //packageUtil.packLink(nextLink);
    Lane::pack(packageUtil, nextLaneInNextLink);
    //packageUtil.packLane(nextLaneInNextLink);

    //Part 5
    packageUtil<<(targetSpeed);

    if(trafficSignal)
    {
        bool hasSignal = true;
        packageUtil<<(hasSignal);
        packageUtil << (trafficSignal->getNode().location);
    }
    else
    {
        bool hasSignal = false;
        packageUtil<<(hasSignal);
    }

    std::string value_ = DebugStream.str();
    packageUtil<<(value_);
}

void sim_mob::Driver::unpack(UnPackageUtils& unpackageUtil) {
    //Part 1
    const Lane* one_lane = Lane::unpack(unpackageUtil);
    currLane_.force(one_lane);

    double value_lane_offset = 0;
    unpackageUtil >> value_lane_offset;
    currLaneOffset_.force(value_lane_offset);

//  double value_lane_offset = unpackageUtil.unpackBasicData<double> ();
//  currLaneOffset_.force(value_lane_offset);

    double value_lane_length = 0;
    unpackageUtil >> value_lane_length;
    currLaneLength_.force(value_lane_length);
//
//  double value_lane_length = unpackageUtil.unpackBasicData<double> ();
//  currLaneLength_.force(value_lane_length);

    bool value_inIntersection = false;
    unpackageUtil >> value_inIntersection;
    isInIntersection.force(value_inIntersection);
    std::cout << "value_inIntersection:" << value_inIntersection << std::endl;

//  bool value_inIntersection = unpackageUtil.unpackBasicData<bool> ();
//  isInIntersection.force(value_inIntersection);

//  std::cout << "A001" << this->getParent()->getId() << std::endl;

    double latMovement_buffer = 0;
    unpackageUtil >> latMovement_buffer;
    latMovement.force(latMovement_buffer);


    double fwdVelocity_buffer = 0;
    unpackageUtil >> fwdVelocity_buffer;
    fwdVelocity.force(fwdVelocity_buffer);


    double latVelocity_buffer = 0;
    unpackageUtil >> latVelocity_buffer;
    latVelocity.force(latVelocity_buffer);


    double fwdAccel_buffer = 0;
    unpackageUtil >> fwdAccel_buffer;
    fwdAccel.force(fwdAccel_buffer);

//  std::cout << "A002" << this->getParent()->getId() << std::endl;

    //part 2
    //currTimeMS = unpackageUtil.unpackBasicData<int> ();
    vehicle = Vehicle::unpack(unpackageUtil) ;
//  std::cout << "A0025" << this->getParent()->getId() << std::endl;

    bool hasSomething = false;
    unpackageUtil >> hasSomething;
    if (hasSomething) {
        intModel = new SimpleIntDrivingModel();
        SimpleIntDrivingModel::unpack(unpackageUtil, dynamic_cast<SimpleIntDrivingModel *>(intModel)) ;
    }

//  std::cout << "A0026" << this->getParent()->getId() << std::endl;

    //Part 3
//  unpackageUtil >> reacTime_LeadingVehicle;
//  unpackageUtil >> reacTime_SubjectVehicle;
//  unpackageUtil >> reacTime_Gap;

//  std::cout << "A0027," << reacTime_Gap << std::endl;

//  unpackageUtil >> perceivedVelocity;
//  unpackageUtil >> perceivedVelocityOfFwdCar;
//  unpackageUtil >> perceivedAccelerationOfFwdCar;
    unpackageUtil >> perceivedDistToFwdCar;
//  unpackageUtil >> perceivedTrafficSignalStop;

    unpackageUtil >> origin.point;
    origin.node = Node::unpack(unpackageUtil);
    unpackageUtil >> goal.point;
    goal.node = Node::unpack(unpackageUtil);

    //firstFrameTick = unpackageUtil.unpackBasicData<bool> ();
    unpackageUtil >> maxLaneSpeed;
//  maxLaneSpeed = unpackageUtil.unpackBasicData<double> ();

    //Part 4
    //desLink = unpackageUtil.unpackLink();
    unpackageUtil >> currLinkOffset;
    unpackageUtil >> targetLaneIndex;

//  currLinkOffset = unpackageUtil.unpackBasicData<double> ();
//  targetLaneIndex = unpackageUtil.unpackBasicData<int> ();
    //nextLink = unpackageUtil.unpackLink();
    nextLaneInNextLink = Lane::unpack(unpackageUtil);

    std::cout << "A005" << this->getParent()->getId() << std::endl;

    //Part 5
    unpackageUtil >> targetSpeed;
//  targetSpeed = unpackageUtil.unpackBasicData<double> ();
    bool hasSignal = false;
    unpackageUtil >> hasSignal;
    if(hasSignal)
    {
        Point signal_location;
        unpackageUtil >> signal_location;
        trafficSignal = sim_mob::getSignalBasedOnNode(&signal_location);
    }

    std::string buffer;
    unpackageUtil >> buffer;
    DebugStream << buffer;

//  std::cout << "A006" << this->getParent()->getId() << std::endl;
}

void sim_mob::Driver::packProxy(PackageUtils& packageUtil) {
    //Part 1
    ParitionDebugOutput debug;
//  std::cout << "packProxy" << std::endl;
//  ParitionDebugOutput::outputToConsole("RRRRRRRRRRRRR");

//  double test_5 = 555.555;
//  packageUtil << (test_5);
//  debug.outputToConsole("debug sent out");

    Lane::pack(packageUtil, currLane_.get());
//  ParitionDebugOutput::outputToConsole("00");

    packageUtil<<(currLaneOffset_.get());
    packageUtil<<(currLaneLength_.get());
    packageUtil<<(isInIntersection.get());

//  ParitionDebugOutput::outputToConsole("latMovement.get()");
//  ParitionDebugOutput::outputToConsole(latMovement.get());

    packageUtil<<(latMovement.get());
    packageUtil<<(fwdVelocity.get());
    packageUtil<<(latVelocity.get());
    packageUtil<<(fwdAccel.get());


//  debug.outputToConsole("packProxy Driver 111");
//  std::cout << "fwdVelocity.get():" << fwdVelocity.get() << std::endl;

//  double test_6 = 666.555;
//  packageUtil << (test_6);

    Vehicle::pack(packageUtil, vehicle);

//  double test_7 = 777.555;
//  packageUtil << (test_7);
//
//  debug.outputToConsole("packProxy Driver 222");

    bool hasSomething = false;
    if(intModel)
    {
        SimpleIntDrivingModel* simple_model = dynamic_cast<SimpleIntDrivingModel*> (intModel);
        if(simple_model)
        {
            hasSomething = true;
            packageUtil<<(hasSomething);
//          debug.outputToConsole("packProxy Driver 244");

            SimpleIntDrivingModel::pack(packageUtil, simple_model);
        }
        else
        {
            packageUtil<<(hasSomething);
        }
    }
    else
    {
        packageUtil<<(hasSomething);
    }

//  debug.outputToConsole("packProxy Driver 255");

    packageUtil<<(currLinkOffset);

    int lane_size = targetLaneIndex;

//  debug.outputToConsole("currLinkOffset");
//  debug.outputToConsole(currLinkOffset);

    packageUtil<<(lane_size);
//  debug.outputToConsole("packProxy Driver 333");
}

void sim_mob::Driver::unpackProxy(UnPackageUtils& unpackageUtil) {
    //Part 1
//  std::cout << "Driver::unpackageProxy" << std::endl;
    ParitionDebugOutput debug;

//  double test_5 = 1;
//  unpackageUtil >> (test_5);
//
//  debug.outputToConsole("test_5");
//  debug.outputToConsole(test_5);

    const Lane* one_lane = Lane::unpack(unpackageUtil);
    currLane_.force(one_lane);
//      if(this->getParent()->getId() < 1000)
//      std::cout << this->getParent()->getId() << "1-1-6-2" << std::endl;
//  std::cout << "Step 4.2.7.1:" << std::endl;

    double value_lane_offset = 0;
    unpackageUtil >> value_lane_offset;
    currLaneOffset_.force(value_lane_offset);
//  std::cout << "Step 4.2.7.2:" << std::endl;

//  if(this->getParent()->getId() > 1000)
//      std::cout << this->getParent()->getId() << "1-1-6-3" << std::endl;

    double value_lane_length = 0;
    unpackageUtil >> value_lane_length;
    currLaneLength_.force(value_lane_length);
//  std::cout << "Step 4.2.7.3:" << std::endl;

    bool value_inIntersection = false;
    unpackageUtil >> value_inIntersection;
    isInIntersection.force(value_inIntersection);
//  std::cout << "Step 4.2.7.4:" << std::endl;

    double latMovement_buffer = 0;
    unpackageUtil >> latMovement_buffer;
    latMovement.force(latMovement_buffer);

    double fwdVelocity_buffer = 0;
    unpackageUtil >> fwdVelocity_buffer;
    fwdVelocity.force(fwdVelocity_buffer);

    double latVelocity_buffer = 0;
    unpackageUtil >> latVelocity_buffer;
    latVelocity.force(latVelocity_buffer);

    double fwdAccel_buffer = 0;
    unpackageUtil >> fwdAccel_buffer;
    fwdAccel.force(fwdAccel_buffer);

    vehicle = Vehicle::unpack(unpackageUtil);

    bool hasSomething = false;
    unpackageUtil >> hasSomething;
    if(hasSomething)
    {
        intModel = new SimpleIntDrivingModel();
        SimpleIntDrivingModel::unpack(unpackageUtil, dynamic_cast<SimpleIntDrivingModel *>(intModel));
    }

    unpackageUtil >> currLinkOffset;

    int buffer;
    unpackageUtil >> buffer;
    targetLaneIndex = buffer;

}

/**
 * Serialize Class DriverUpdateParams
 */
void DriverUpdateParams::pack(PackageUtils& package, const DriverUpdateParams* params) {

    if (params == NULL)
    {
        bool is_NULL = true;
        package << (is_NULL);
        return;
    }
    else
    {
        bool is_NULL = false;
        package << (is_NULL);
    }

//  package << (params->frameNumber);
//  package << (params->currTimeMS);

    sim_mob::Lane::pack(package, params->currLane);
    package << (params->currLaneIndex);
//  package << (params->fromLaneIndex);
    sim_mob::Lane::pack(package, params->leftLane);
    sim_mob::Lane::pack(package, params->rightLane);

    package << (params->currSpeed);
    package << (params->currLaneOffset);
    package << (params->currLaneLength);
//  package << (params->isTrafficLightStop);

    package << (params->trafficSignalStopDistance);
    package << (params->elapsedSeconds);
    package << (params->perceivedFwdVelocity);
    package << (params->perceivedLatVelocity);
    package << (params->perceivedFwdVelocityOfFwdCar);
    package << (params->perceivedLatVelocityOfFwdCar);
    package << (params->perceivedAccelerationOfFwdCar);
    package << (params->perceivedDistToFwdCar);

    package << (params->laneChangingVelocity);
    package << (params->isCrossingAhead);
    package << (params->isApproachingToIntersection);
    package << (params->crossingFwdDistance);

    package << (params->space);
    package << (params->a_lead);
    package << (params->v_lead);
    package << (params->space_star);
    package << (params->distanceToNormalStop);

    package << (params->dis2stop);
    package << (params->isWaiting);

    package << (params->justChangedToNewSegment);
    package << (params->TEMP_lastKnownPolypoint);
    package << (params->justMovedIntoIntersection);
    package << (params->overflowIntoIntersection);
}

void DriverUpdateParams::unpack(UnPackageUtils& unpackage, DriverUpdateParams* params) {
    bool is_NULL = false;
    unpackage >> is_NULL;
    if (is_NULL) {
        return;
    }

//  unpackage >> params->frameNumber;
//  unpackage >> params->currTimeMS;

//  params->frameNumber = unpackage.unpackBasicData<double> ();
//  params->currTimeMS = unpackage.unpackBasicData<double> ();

    params->currLane = sim_mob::Lane::unpack(unpackage);
    unpackage >> params->currLaneIndex;
//  unpackage >> params->fromLaneIndex;
    params->leftLane = sim_mob::Lane::unpack(unpackage);
    params->rightLane = sim_mob::Lane::unpack(unpackage);

    unpackage >> params->currSpeed;
    unpackage >> params->currLaneOffset;
    unpackage >> params->currLaneLength;
//  unpackage >> params->isTrafficLightStop;

//  params->currSpeed = unpackage.unpackBasicData<double> ();
//  params->currLaneOffset = unpackage.unpackBasicData<double> ();
//  params->currLaneLength = unpackage.unpackBasicData<double> ();
//  params->isTrafficLightStop = unpackage.unpackBasicData<bool> ();

    unpackage >> params->trafficSignalStopDistance;
    unpackage >> params->elapsedSeconds;
    unpackage >> params->perceivedFwdVelocity;
    unpackage >> params->perceivedLatVelocity;
    unpackage >> params->perceivedFwdVelocityOfFwdCar;
    unpackage >> params->perceivedLatVelocityOfFwdCar;
    unpackage >> params->perceivedAccelerationOfFwdCar;
    unpackage >> params->perceivedDistToFwdCar;

//  params->trafficSignalStopDistance = unpackage.unpackBasicData<double> ();
//  params->elapsedSeconds = unpackage.unpackBasicData<double> ();
//  params->perceivedFwdVelocity = unpackage.unpackBasicData<double> ();
//  params->perceivedLatVelocity = unpackage.unpackBasicData<double> ();
//  params->perceivedFwdVelocityOfFwdCar = unpackage.unpackBasicData<double> ();
//  params->perceivedLatVelocityOfFwdCar = unpackage.unpackBasicData<double> ();
//  params->perceivedAccelerationOfFwdCar = unpackage.unpackBasicData<double> ();
//  params->perceivedDistToFwdCar = unpackage.unpackBasicData<double> ();
}


}

#endif
