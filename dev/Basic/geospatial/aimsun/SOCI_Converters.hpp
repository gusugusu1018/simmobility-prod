/* Copyright Singapore-MIT Alliance for Research and Technology */

#pragma once

//This file contains several Type converters for SOCI object-mapping

#include "soci/soci.h"
#include "Node.hpp"
#include "Section.hpp"
#include "Turning.hpp"
#include "Polyline.hpp"


using namespace sim_mob::aimsun;
using std::string;


namespace soci
{

template<> struct type_conversion<Node>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, Node &res)
    {
    	res.id = vals.get<int>("node_id", 0);
    	res.xPos = vals.get<double>("xpos", 0.0);
    	res.yPos = vals.get<double>("ypos", 0.0);
    	res.isIntersection = vals.get<int>("isintersection", 0);
    }
    static void to_base(const Node& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("node_id", src.id);
        vals.set("xpos", src.xPos);
        vals.set("ypos", src.yPos);
        vals.set("isintersection", src.isIntersection?1:0);
        ind = i_ok;
    }
};


template<> struct type_conversion<Section>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, Section &res)
    {
    	res.id = vals.get<int>("id", 0);
    	res.roadName = vals.get<string>("road_name", "");
    	res.numLanes = vals.get<int>("nb_lanes", 1);
    	res.speed = vals.get<double>("speed", 0);
    	res.capacity = vals.get<double>("capacity", 0);
    	res.length = vals.get<double>("length", 0);
    	res.TMP_FromNodeID = vals.get<int>("fnode", 0);
    	res.TMP_ToNodeID = vals.get<int>("tnode", 0);
    }
    static void to_base(const Section& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("id", src.id);
        vals.set("road_name", src.roadName);
        vals.set("nb_lanes", src.numLanes);
        vals.set("speed", src.speed);
        vals.set("capacity", src.capacity);
        vals.set("length", src.length);
        vals.set("fnode", src.fromNode->id);
        vals.set("tnode", src.toNode->id);
        ind = i_ok;
    }
};



template<> struct type_conversion<Turning>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, Turning &res)
    {
    	res.id = vals.get<int>("turning_id", 0);
    	res.fromLane.first = vals.get<int>("from_lane_a", 0);
    	res.fromLane.second = vals.get<int>("from_lane_b", 0);
    	res.toLane.first = vals.get<int>("to_lane_a", 0);
    	res.toLane.second = vals.get<int>("to_lane_b", 0);
    	res.TMP_FromSection = vals.get<int>("from_section", 0);
    	res.TMP_ToSection = vals.get<int>("to_section", 0);
    }
    static void to_base(const Turning& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("turning_id", src.id);
    	vals.set("from_lane_a", src.fromLane.first);
    	vals.set("from_lane_b", src.fromLane.second);
    	vals.set("to_lane_a", src.toLane.first);
    	vals.set("to_lane_b", src.toLane.second);
    	vals.set("from_section", src.fromSection->id);
    	vals.set("to_section", src.toSection->id);
        ind = i_ok;
    }
};


template<> struct type_conversion<Polyline>
{
    typedef values base_type;
    static void from_base(const soci::values& vals, soci::indicator& ind, Polyline &res)
    {
    	res.xPos = vals.get<double>("xpos", 0.0);
    	res.yPos = vals.get<double>("ypos", 0.0);
    	res.TMP_SectionId = vals.get<int>("section_id", 0);
    }
    static void to_base(const Polyline& src, soci::values& vals, soci::indicator& ind)
    {
    	vals.set("section_id", src.section->id);
        vals.set("xpos", src.xPos);
        vals.set("ypos", src.yPos);
        ind = i_ok;
    }
};



}
