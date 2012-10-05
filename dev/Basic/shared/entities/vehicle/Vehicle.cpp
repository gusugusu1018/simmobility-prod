/* Copyright Singapore-MIT Alliance for Research and Technology */

/*
 * Vehicle.cpp
 *
 *  Created on: Oct 24, 2011
 *      Author: lzm
 */

#include "Vehicle.hpp"
#include "geospatial/RoadSegment.hpp"

#ifndef SIMMOB_DISABLE_MPI
#include "partitions/PackageUtils.hpp"
#include "partitions/UnPackageUtils.hpp"
#include "partitions/ParitionDebugOutput.hpp"
#endif

using namespace sim_mob;
using std::vector;

sim_mob::Vehicle::Vehicle(vector<WayPoint> wp_path, int startLaneID) :
	length(400), width(200), latMovement(0), fwdVelocity(0), latVelocity(0), fwdAccel(0), error_state(true), turningDirection(LCS_SAME) {
	initPath(wp_path, startLaneID);
}

sim_mob::Vehicle::Vehicle(vector<WayPoint> wp_path, int startLaneID, double length, double width) :
	length(length), width(width), latMovement(0), fwdVelocity(0), latVelocity(0), fwdAccel(0), error_state(true), turningDirection(LCS_SAME) {
	initPath(wp_path, startLaneID);
}

sim_mob::Vehicle::Vehicle() :
	length(400), width(200), latMovement(0), fwdVelocity(0), latVelocity(0), fwdAccel(0), error_state(true), turningDirection(LCS_SAME) {
}

sim_mob::Vehicle::Vehicle(const Vehicle& copyFrom) :
	length(copyFrom.length), width(copyFrom.width), fwdMovement(copyFrom.fwdMovement),
			latMovement(copyFrom.latMovement), fwdVelocity(copyFrom.fwdVelocity), latVelocity(copyFrom.latVelocity),
			fwdAccel(copyFrom.fwdAccel), posInIntersection(copyFrom.posInIntersection), error_state(
					copyFrom.error_state), turningDirection(LCS_SAME) {

}

void sim_mob::Vehicle::initPath(vector<WayPoint> wp_path, int startLaneID) {
	//Construct a list of RoadSegments.
	vector<const RoadSegment*> path;
	for (vector<WayPoint>::iterator it = wp_path.begin(); it != wp_path.end(); it++) {
		if (it->type_ == WayPoint::ROAD_SEGMENT) {
			path.push_back(it->roadSegment_);
		}
	}

	//Assume this is sufficient; we will specifically test for error cases later.
	error_state = false;

	//Init
	fwdMovement.setPath(path, startLaneID);
}


void sim_mob::Vehicle::resetPath(vector<WayPoint> wp_path) {
	//Construct a list of RoadSegments.
	vector<const RoadSegment*> path;
	for (vector<WayPoint>::iterator it = wp_path.begin(); it != wp_path.end(); it++) {
		if (it->type_ == WayPoint::ROAD_SEGMENT) {
			path.push_back(it->roadSegment_);
//			std::cout<<it->roadSegment_->getStart()->location.getX()<<std::endl;
		}
	}

	//Assume this is sufficient; we will specifically test for error cases later.
	error_state = false;

	//reset
	fwdMovement.resetPath(path);
}

const vector<const RoadSegment*>& sim_mob::Vehicle::getCompletePath() const {
	return fwdMovement.fullPath;
}

void sim_mob::Vehicle::setPositionInIntersection(double x, double y) {
	posInIntersection.x = x;
	posInIntersection.y = y;
}

const RoadSegment* sim_mob::Vehicle::getCurrSegment() const {
	return fwdMovement.getCurrSegment();
}

double sim_mob::Vehicle::getCurrentSegmentLength()
{
	return fwdMovement.getCurrentSegmentLength();
}

const RoadSegment* sim_mob::Vehicle::getNextSegment(bool inSameLink) const {
	return fwdMovement.getNextSegment(inSameLink);
}
const sim_mob::RoadSegment* sim_mob::Vehicle::getSecondSegmentAhead() {
	return fwdMovement.getNextToNextSegment();
}

const RoadSegment* sim_mob::Vehicle::hasNextSegment(bool inSameLink) const {
	return fwdMovement.getNextSegment(inSameLink);
}

const RoadSegment* sim_mob::Vehicle::getPrevSegment() const {
	return fwdMovement.getPrevSegment(true);
}

const Node* sim_mob::Vehicle::getNodeMovingTowards() const {
	//return fwdMovement.isMovingForwardsOnCurrSegment() ? fwdMovement.getCurrSegment()->getEnd() : fwdMovement.getCurrSegment()->getStart();
	return fwdMovement.getCurrSegment()->getEnd(); //TEMP
}

const Node* sim_mob::Vehicle::getNodeMovingFrom() const {
	//return fwdMovement.isMovingForwardsOnCurrSegment() ? fwdMovement.getCurrSegment()->getStart() : fwdMovement.getCurrSegment()->getEnd();
	return fwdMovement.getCurrSegment()->getStart(); //TEMP
}

const Link* sim_mob::Vehicle::getCurrLink() const {
	return fwdMovement.getCurrLink();
}

const Lane* sim_mob::Vehicle::getCurrLane() const {
	return fwdMovement.getCurrLane();
}


double sim_mob::Vehicle::getCurrLinkReportedLength() const {
	return fwdMovement.getCurrLinkReportedLength(); // ()->getLength(fwdMovement.isMovingForwardsOnCurrSegment());
}

sim_mob::DynamicVector sim_mob::Vehicle::getCurrPolylineVector() const {
	return DynamicVector(fwdMovement.getCurrPolypoint().getX(), fwdMovement.getCurrPolypoint().getY(),
			fwdMovement.getNextPolypoint().getX(), fwdMovement.getNextPolypoint().getY());
}

bool sim_mob::Vehicle::hasPath() const {
	return fwdMovement.isPathSet();
}

bool sim_mob::Vehicle::isMovingForwardsInLink() const {
	return fwdMovement.isMovingForwardsInLink;
}

double sim_mob::Vehicle::getX() const {
	throw_if_error();
	return getPosition().x;
}

double sim_mob::Vehicle::getY() const {
	throw_if_error();
	return getPosition().y;
}

DPoint sim_mob::Vehicle::getPosition() const {
	throw_if_error();

	//Temp
	if (isInIntersection() && (posInIntersection.x == 0 || posInIntersection.y == 0)) {
#ifndef SIMMOB_DISABLE_OUTPUT
		boost::mutex::scoped_lock local_lock(sim_mob::Logger::global_mutex); //Note: beware double-locking.
		std::cout << "WARNING: Vehicle is in intersection without a position!\n";
#endif
	}

	DPoint origPos = fwdMovement.getPosition();
	if (isInIntersection() && posInIntersection.x != 0 && posInIntersection.y != 0) {
		//Override: Intersection driving
		origPos.x = posInIntersection.x;
		origPos.y = posInIntersection.y;
	} else if (latMovement != 0 && !fwdMovement.isDoneWithEntireRoute()) {
		DynamicVector latMv(0, 0, fwdMovement.getNextPolypoint().getX() - fwdMovement.getCurrPolypoint().getX(),
				fwdMovement.getNextPolypoint().getY() - fwdMovement.getCurrPolypoint().getY());
		latMv.flipLeft();
		latMv.scaleVectTo(latMovement).translateVect();
		origPos.x += latMv.getX();
		origPos.y += latMv.getY();
	}
	return origPos;
}

void sim_mob::Vehicle::shiftToNewLanePolyline(bool moveLeft) {
	fwdMovement.shiftToNewPolyline(moveLeft);
}

void sim_mob::Vehicle::moveToNewLanePolyline(int laneID) {
	fwdMovement.moveToNewPolyline(laneID);
}

double sim_mob::Vehicle::getDistanceMovedInSegment() const {
	throw_if_error();
	return fwdMovement.getCurrDistAlongRoadSegment();
}

double sim_mob::Vehicle::getDistanceToSegmentStart() const {
	throw_if_error();
	DynamicVector dis(this->getX(), this->getY(),
			this->getCurrSegment()->getStart()->location.getX(),
			this->getCurrSegment()->getStart()->location.getY());
	return dis.getMagnitude();
}

double sim_mob::Vehicle::getCurrLinkLaneZeroLength() const {
	throw_if_error();
	return fwdMovement.getTotalRoadSegmentLength();
}

double sim_mob::Vehicle::getCurrPolylineLength() const {
	throw_if_error();
	return fwdMovement.getCurrPolylineTotalDist();
}

double sim_mob::Vehicle::getAllRestRoadSegmentsLength() const {
	throw_if_error();
	return fwdMovement.getAllRestRoadSegmentsLength();
}

double sim_mob::Vehicle::getVelocity() const {
	throw_if_error();
	return fwdVelocity;
}

double sim_mob::Vehicle::getLatVelocity() const {
	throw_if_error();
	return latVelocity;
}

double sim_mob::Vehicle::getAcceleration() const {
	throw_if_error();
	return fwdAccel;
}

/*bool sim_mob::Vehicle::reachedSegmentEnd() const
 {
 throw_if_error();
 return position.reachedEnd();
 }*/

LANE_CHANGE_SIDE sim_mob::Vehicle::getTurningDirection() const{
	return turningDirection;
}

double sim_mob::Vehicle::getAngle() const {
	throw_if_error();
	if (fwdMovement.isDoneWithEntireRoute()) {
		return 0; //Shouldn't matter.
	}

	DynamicVector temp(fwdMovement.getCurrPolypoint().getX(), fwdMovement.getCurrPolypoint().getY(),
			fwdMovement.getNextPolypoint().getX(), fwdMovement.getNextPolypoint().getY());

	return temp.getAngle();
}

void sim_mob::Vehicle::setVelocity(double value) {
	throw_if_error();
	fwdVelocity = value;
}

void sim_mob::Vehicle::setLatVelocity(double value) {
	throw_if_error();
	latVelocity = value;
}

void sim_mob::Vehicle::setAcceleration(double value) {
	throw_if_error();
	fwdAccel = value;
}

double sim_mob::Vehicle::moveFwd(double amt) {
	throw_if_error();
	return fwdMovement.advance(amt);
}

void sim_mob::Vehicle::moveLat(double amt) {
	throw_if_error();
	latMovement += amt;
}

void sim_mob::Vehicle::setTurningDirection(LANE_CHANGE_SIDE direction)
{
	throw_if_error();
	turningDirection = direction;
}

void sim_mob::Vehicle::resetLateralMovement() {
	throw_if_error();
	latMovement = 0;
}

double sim_mob::Vehicle::getLateralMovement() const {
	throw_if_error();
	return latMovement;
}

bool sim_mob::Vehicle::isInIntersection() const {
	throw_if_error();
	return fwdMovement.isInIntersection();
}

const Lane* sim_mob::Vehicle::moveToNextSegmentAfterIntersection() {
	throw_if_error();
	posInIntersection.x = posInIntersection.y = 0;
	return fwdMovement.leaveIntersection();
}

bool sim_mob::Vehicle::isDone() const {
	throw_if_error();
	return fwdMovement.isDoneWithEntireRoute();
}

#ifndef SIMMOB_DISABLE_MPI
//void sim_mob::Vehicle::pack(PackageUtils& package, Vehicle* one_vehicle) {
//	package.packBasicData<double> (one_vehicle->length);
//	package.packBasicData<double> (one_vehicle->width);
//}


#endif
