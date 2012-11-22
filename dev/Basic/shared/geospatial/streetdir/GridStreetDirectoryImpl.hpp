/* Copyright Singapore-MIT Alliance for Research and Technology */

#pragma once

#include "GenConfig.h"
#include "util/LangHelpers.hpp"
#include "metrics/Length.hpp"
#include "geospatial/Point2D.hpp"

#include <map>
#include <vector>

#include <boost/unordered_map.hpp>
#include <boost/utility.hpp>

#include "StreetDirectory.hpp"


namespace sim_mob {

class GridStreetDirectoryImpl : public StreetDirectory::Impl {
public:
	GridStreetDirectoryImpl(const RoadNetwork& network, centimeter_t gridWidth, centimeter_t gridHeight);
	virtual ~GridStreetDirectoryImpl() {}

protected:
	virtual StreetDirectory::LaneAndIndexPair getLane(const Point2D& position) const;

    virtual const MultiNode* GetCrossingNode(const Crossing* cross) const;

    virtual std::vector<StreetDirectory::RoadSegmentAndIndexPair> closestRoadSegments(const Point2D& point, centimeter_t halfWidth, centimeter_t halfHeight) const;

private:
    // Partition the road network into a rectangular grid.
    void partition(const RoadNetwork& network);

    // Partition the list of road segments into a rectangular grid.
    // If <isForward> is true, then vehicle traffic on the road segments flows from their start
    // to end points.
    void partition(const std::vector<RoadSegment*>& segments, bool isForward);

    // Partition the road segment into a rectangular grid.
    // If <isForward> is true, then vehicle traffic on the road segment flows from its start
    // to end points.
    void partition(const RoadSegment& segment, bool isForward);

    // Return true if the stretch of the road segment is inside the specified grid cell.
    // The stretch is specified by <p1>, <p2>, and <halfWidth>; the line from <p1> to <p2>
    // traces the middle of the stretch.  <m> and <n> specify the (m, n) grid cell of a
    // rectangular grid of gridWidth_ and gridHeight_.
    bool checkGrid(int m, int n, const Point2D& p1, const Point2D& p2, centimeter_t halfWidth) const;

    // Called once for each unique RoadSegment
    void buildLookups(const std::vector<RoadSegment*>& roadway, std::set<const Crossing*>& completed);

private:
    centimeter_t gridWidth_;
    centimeter_t gridHeight_;

    // The following custom hash and equality functions were taken
    // from the boost::unordered documentation.
    struct Hash2D : private std::unary_function<Point2D, std::size_t> {
        size_t operator()(const Point2D& key) const {
            std::size_t seed = 0;
            boost::hash_combine(seed, key.getX());
            boost::hash_combine(seed, key.getY());
            return seed;
        }
    };
    struct Equal2D : private std::binary_function<Point2D, Point2D, bool> {
        bool operator()(const Point2D& p1, const Point2D& p2) const {
            return p1.getX() == p2.getX() && p1.getY() == p2.getY();
        }
    };

    //Map of Crossings->MultiNode. May not contain all crossings.
    std::map<const Crossing*, const MultiNode*> crossings_to_multinodes;

    // map< key, vector<value> > is used for GridType instead of multimap<key, value>.
    typedef std::vector<StreetDirectory::RoadSegmentAndIndexPair> RoadSegmentSet;
    typedef boost::unordered_map<Point2D, RoadSegmentSet, Hash2D, Equal2D> GridType;

    GridType grid_;
    std::map<std::string, const RoadSegment*> roadSegments_;

};


}