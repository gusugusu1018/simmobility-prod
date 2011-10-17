#pragma once


/**
 * \file GeomHelpers.hpp
 * Contains functions which are useful when calculating geometric information, such as distances or line intersections.
 */


#include <vector>
#include <string>

#include "DynamicVector.hpp"


namespace sim_mob {
class Point2D;

namespace aimsun {
class Node;
class Crossing;
class Lane;
class Section;
} //End aimsun namespace



/**
 * Distance between two points, (x1,y1) and (x2,y2)
 *
 * Various alternative forms of this function exist, all of which reduce to this form.
 */
double dist(double x1, double y1, double x2, double y2);
double dist(const sim_mob::aimsun::Crossing* c1, const sim_mob::aimsun::Crossing* c2);
double dist(const sim_mob::aimsun::Lane* ln, const sim_mob::aimsun::Node* nd);
double dist(const sim_mob::aimsun::Lane* ln1, const sim_mob::aimsun::Lane* ln2);
double dist(const sim_mob::aimsun::Node* n1, const sim_mob::aimsun::Node* n2);
double dist(const sim_mob::Point2D* p1, const sim_mob::Point2D* p2);


/**
 * Check if an intersection point is actually on a line segment. The line from (ax,ay) to (bx,by) is
 *   checked to see if (cx,cy) is a valid point.
 *
 * Various alternative forms of this function exist, all of which reduce to this form.
 */
bool lineContains(double ax, double ay, double bx, double by, double cx, double cy);
bool lineContains(const sim_mob::aimsun::Crossing* p1, const sim_mob::aimsun::Crossing* p2, double xPos, double yPos);
bool lineContains(const sim_mob::aimsun::Section* sec, double xPos, double yPos);


/**
 * Check if a point is left or right of a vector. The vector is defined with a starting point of (ax,ay)
 *   and an ending point of (bx,by). The point (cx,cy) is tested to see if it is left of the vector.
 *
 * \return true if the point is left of the vector.
 *
 * Various alternative forms of this function exist, all of which reduce to this form.
 */
bool PointIsLeftOfVector(double ax, double ay, double bx, double by, double cx, double cy);
bool PointIsLeftOfVector(const sim_mob::aimsun::Node* vecStart, const sim_mob::aimsun::Node* vecEnd, const sim_mob::aimsun::Lane* point);
bool PointIsLeftOfVector(const sim_mob::DynamicVector& vec, const sim_mob::aimsun::Lane* point);
bool PointIsLeftOfVector(const sim_mob::DynamicVector& vec, double x, double y);


/**
 * Check if two lines intersect. The two lines in question are (x1,y1)->(x2, y2) and (x3,y3)->(x4,y4).
 *
 * \note
 * This function uses an arithmetic (NOT vector) computation. Hence, the size of the line segment is not taken into account; ANY two lines
 * will have an intersection unless they are parallel.
 *
 * \return the line intersection. If the lines are parallel, returns Point2D(MAX, MAX), where MAX is the maximum value of a double.
 *
 * Various alternative forms of this function exist, all of which reduce to this form.
 */
sim_mob::Point2D LineLineIntersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
sim_mob::Point2D LineLineIntersect(const sim_mob::aimsun::Crossing* const p1, const sim_mob::aimsun::Crossing* p2, const sim_mob::aimsun::Section* sec);
sim_mob::Point2D LineLineIntersect(const sim_mob::DynamicVector& v1, const sim_mob::DynamicVector& v2);
sim_mob::Point2D LineLineIntersect(const sim_mob::Point2D& p1, const sim_mob::Point2D& p2, const sim_mob::Point2D& p3, const sim_mob::Point2D& p4);


/**
 * Shift a polyline to the left or right by an amount.
 *
 * \param orig The original polyline.
 * \param shiftAmt The distance in CM to shift the polyline.
 * \param shiftLeft Whether to shift this polyline left (true) or right (false). If a vector goes from (0,0) to (1,0), then the point (0,1) is defined as "left".
 */
std::vector<sim_mob::Point2D> ShiftPolyline(const std::vector<sim_mob::Point2D>& orig, double shiftAmt, bool shiftLeft=true);



}