/*
 * PedestrianPathMover.h
 *
 *  Created on: Aug 20, 2012
 *      Author: redheli
 */

#ifndef PEDESTRIANPATHMOVER_H_
#define PEDESTRIANPATHMOVER_H_

#include "util/DynamicVector.hpp"
#include "geospatial/Lane.hpp"
#include "geospatial/StreetDirectory.hpp"
#include "geospatial/Crossing.hpp"
#include "geospatial/RoadSegment.hpp"
#include <vector>
#include<boost/unordered_map.hpp>


using std::vector;
using std::map;

namespace sim_mob
{
inline std::size_t hash_value(const sim_mob::Point2D& p)
{
    size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(p.getX()));
    boost::hash_combine(seed, boost::hash_value(p.getY()));
    return seed;
}
typedef vector<WayPoint> PEDESTRIAN_PATH;
typedef vector<WayPoint>::iterator PEDESTRIAN_PATH_ITERATOR;

typedef vector<sim_mob::Point2D> POLYLINEPOINTS;
typedef vector<sim_mob::Point2D>::const_iterator POLYLINEPOINTS_ITERATOR;
typedef vector<sim_mob::Point2D>::const_reverse_iterator POLYLINEPOINTS_REVERSE_ITERATOR;
typedef boost::unordered_map<sim_mob::Point2D,WayPoint* > POLYLINEPOINTS_WAYPOINT_MAP;

class PedestrianPathMover {
public:
	PedestrianPathMover();
	~PedestrianPathMover();
public:
	void setPath(const PEDESTRIAN_PATH path);
	///General forward movement function: move X cm forward. Automatically switches to a new polypoint or
	///  road segment as applicable.
	//Returns any "overflow" distance if we are in an intersection, 0 otherwise.
	double advance(double fwdDistance);
	//Retrieve our X/Y position based ONLY on forward movement (e.g., nothing with Lanes)
	sim_mob::DPoint getPosition() const;
	WayPoint* getCurrentWaypoint(){ return currentWaypoint; };
public:
	PEDESTRIAN_PATH pedestrian_path;
	PEDESTRIAN_PATH_ITERATOR pedestrian_path_iter;
	double currPolylineLength();
	bool isDoneWithEntireRoute() { return isDoneWithEntirePath; };
	bool isAtCrossing();
private:
	WayPoint *currentWaypoint;
	WayPoint *nextWaypoint;
	// advance to next waypoint, if has next waypoint ,return true
	bool advanceToNextPolylinePoint();
	POLYLINEPOINTS getCrossingPolylinePoints(WayPoint *wp);
	void relToAbs(double xRel, double yRel, double & xAbs, double & yAbs,
			double cStartX,double cStartY,double cEndX,double cEndY);
	void absToRel(double xAbs, double yAbs, double & xRel, double & yRel,double cStartX,
			double cStartY,double cEndX,double cEndY);
private:
	double distAlongPolyline;
	bool isDoneWithEntirePath;
	//ploy line start ,end point , maybe cross two segments or segment->crossring
	POLYLINEPOINTS polylinePoints;
	POLYLINEPOINTS_ITERATOR currPolylineStartpoint;
	POLYLINEPOINTS_ITERATOR currPolylineEndpoint;
	POLYLINEPOINTS_WAYPOINT_MAP polylinePoint_wayPoint_map;
};
}
#endif /* PEDESTRIANPATHMOVER_H_ */
