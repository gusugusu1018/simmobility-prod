/* Copyright Singapore-MIT Alliance for Research and Technology */

#include "BusController.hpp"
#include "entities/Person.hpp"
#include "util/LangHelpers.hpp"

using std::vector;
using namespace sim_mob;
typedef Entity::UpdateStatus UpdateStatus;
vector<BusController*> BusController::all_busctrllers_;// Temporary saved all the buscontroller, eventually it will go to all agent stream

//NOTE: Using a shared static variable is MUCH better than using a global variable. ~Seth
//sim_mob::BusController* sim_mob::BusController::busctrller = new sim_mob::BusController(0);

void sim_mob::BusController::registerBusController(unsigned int startTime, const MutexStrategy& mtxStrat)
{
	//TODO: Why not just use the Agent auto-id generator? (id==-1)  ~Seth
	BusController * busctrller = new sim_mob::BusController(999);// If needed , add more  busctrllers and design id for them
	busctrller->setStartTime(startTime);
	all_busctrllers_.push_back(busctrller);
}

sim_mob::BusController::BusController(int id, const MutexStrategy& mtxStrat) :
	Agent(mtxStrat, id),frameNumberCheck(0), nextTimeTickToStage(0), tickStep(1), firstFrameTick(true)
{
}

sim_mob::BusController::~BusController()
{
}

sim_mob::Link* sim_mob::BusController::getCurrLink(){
	return currLink;
}
void sim_mob::BusController::setCurrLink(sim_mob::Link* link){
	currLink = link;
}

void sim_mob::BusController::addBus(Bus* bus)
{
	managedBuses.push_back(bus);
}

void sim_mob::BusController::remBus(Bus* bus)
{
	std::vector<Bus*>::iterator it = std::find(managedBuses.begin(), managedBuses.end(), bus);
	if (it!=managedBuses.end()) {
		managedBuses.erase(it);
	}
}

bool sim_mob::BusController::SetRouteforBusTrip(unsigned int busRoute_id)
{
	sim_mob::BusRouteInfo* busRouteInfo = new sim_mob::BusRouteInfo(busRoute_id);

	// find (route_id) 	return busStop_vecTemp; // vector<BusStop*>;
	// find (route_id)  return roadsegment_vecTemp; // vector<RoadSegment*>;
	// find (route_id)  return busStopInfo_vecTemp; // vector<BusStopInfo*>;   (there is already BusStopID matched BusStop)

	// const vector<BusStop*>& busStop_vecTemp = route_BusStops.find(route_id)->second; // route_BusStops is a map loaded from database
//	for(vector<BusStop*>::const_iterator it = busStop_vecTemp.begin();it != busStop_vecTemp.end(); it++)
//	{
//		busRouteInfo.addRoadSegment(*it);
//	}

	// const vector<RoadSegment*>& roadsegment_vecTemp = route_RoadSegments.find(route_id)->second; // route_RoadSegments is a map loaded from database
//	for(vector<RoadSegment*>::const_iterator it1 = roadsegment_vecTemp.begin();it1 != roadsegment_vecTemp.end(); it1++)
//	{
//		busRouteInfo.addRoadSegment(*it1);
//	}

	// const vector<BusStopInfo*>& busStopInfo_vecTemp = route_BusStopInfos.find(route_id)->second; // route_BusStopInfos is a map loaded from database
//	for(vector<BusStopInfo*>::const_iterator iter = BusStopInfo_vecTemp.begin();iter != BusStopInfo_vecTemp.end(); iter++)
//	{
//		busRouteInfo.addRoadSegment(*iter);
//	}

	return true;
}

sim_mob::BusTrip* sim_mob::BusController::MakeBusTrip(const TripChainItem& tcItem)
{
	//sim_mob::BusTrip* BusTripToSave = new sim_mob::BusTrip(tcItem.entityID,tcItem.sequenceNumber,tcItem.startTime,tcItem.endTime,tcItem.tripID);   //need Database Table connections(BusTripChainItem and BusTrip)
	//SetRouteforBusTrip
}

void sim_mob::BusController::assignBusTripChainWithPerson()
{

}

void sim_mob::BusController::receiveBusInformation(DPoint pt) {
	posBus = pt;
	std::cout<<"Report Given Bus position: --->("<<posBus.x<<","<<posBus.y<<")"<<std::endl;
}

DailyTime sim_mob::BusController::sendBusInformation()
{

}

void sim_mob::BusController::addOrStashBuses(Agent* p, std::vector<Entity*>& active_agents)
{
	if (ConfigParams::GetInstance().DynamicDispatchDisabled() || p->getStartTime()==0) {
		//Only agents with a start time of zero should start immediately in the all_agents list.
		p->load(p->getConfigProperties());
		p->clearConfigProperties();
		active_agents.push_back(p);
	} else {
		//Start later.
		pending_buses.push(p);
	}
}


void sim_mob::BusController::dispatchFrameTick(frame_t frameTick)
{
	//Note: The WorkGroup (see below) will delay an entity until its time tick has arrived, so there's nothing wrong
	//      with dispatching early. To reflect this, I've added +3 to the next time tick. Ideally, the BusController
	//      would stage the Bus as soon as it was 100% sure that this bus would run. (We can add functionality later for
	//      updating a pending request). In other words, let the WorkGroup do what it does best. ~Seth
	nextTimeTickToStage += tickStep;
	unsigned int nextTickMS = (nextTimeTickToStage+3)*ConfigParams::GetInstance().baseGranMS;

	//Stage any pending entities that will start during this time tick.
	while (!pending_buses.empty() && pending_buses.top()->getStartTime() <= nextTickMS) {
		///////////////////////////////////////////////////////////////////
		//Ask the current worker's parent WorkGroup to schedule this Entity.
		///////////////////////////////////////////////////////////////////
		//
		// TODO: The use of "getParent()" in Worker is extremely dangerous.
		//       It only works because there is exactly 1 BusController, and that
		//       Controller is the only thing which accesses the "parent" class.
		//       To fix this, we should probably return a list of entities to schedule
		//       as a result of update(), and let the WorkGroup skim this list
		//       when it knows it's safe to. ~Seth
		//
		///////////////////////////////////////////////////////////////////
		currWorker->getParent()->scheduleEntity(pending_buses.top());
		pending_buses.pop();
	}
}

UpdateStatus sim_mob::BusController::update(frame_t frameNumber)
{
	//NOTE: I am removing the AGENT_UPDATE_PROFILE/STRICT_AGENT_ERRORS check, since it was clearly copied from Person.cpp
	//      We will (after merging) be migrating the "frame_init"/"frame_tick" pattern out of Person and into Agent, with
	//      Entity remaining the "minimal" class. To make this easier, it will help if BusController::update() only contains
	//      what will later go into BusController::frame_tick.
	// ~Seth

	unsigned int currTimeMS = frameNumber*ConfigParams::GetInstance().baseGranMS;

	//Has update() been called early?
	//TODO: This should eventually go into its own helper function in the Agent class.
	//      Aim for the following API:
	//      if (updateBeforeStartTime() { return UpdateStatus::Continue; }
	//      ...and have the parent function take care of throwing the error. ~Seth
	if (currTimeMS < getStartTime()) {
		//This only represents an error if dynamic dispatch is enabled. Else, we silently skip this update.
		if (!ConfigParams::GetInstance().DynamicDispatchDisabled()) {
			std::stringstream msg;
			msg << "Agent(" << getId() << ") specifies a start time of: "
					<< getStartTime() << " but it is currently: " << currTimeMS
					<< "; this indicates an error, and should be handled automatically.";
			throw std::runtime_error(msg.str().c_str());
		}
		return UpdateStatus::Continue;
	}

	//TEMPORARY: will exist at the Agent level later.
	if (firstFrameTick) {
		frame_init(frameNumber);
		firstFrameTick = false;
	}
	//END TEMPORARY

	//TEMPORARY: This will become our frame_tick method.
	dispatchFrameTick(frameNumber);
	//END TEMPORARY

	//TEMPORARY: This will become frame_output
	frame_tick_output(frameNumber);
	//END TEMPORARY

	//The variable UpdateStatus::Continue is a convenient way of returning just the simple status.
	return UpdateStatus::Continue;
}

void sim_mob::BusController::frame_init(frame_t frameNumber)
{
	frameNumberCheck = 0;
}

void sim_mob::BusController::frame_tick_output(frame_t frameNumber)
{
	//if no buscontroller in the loadorder, no output
	if (!getToBeInList()) {
		return;
	}

#ifndef SIMMOB_DISABLE_OUTPUT
	LogOut("(\"BusController\""
			<<","<<frameNumber
			<<","<<getId()
			<<",{"
			<<"\"managedBuses size\":\""<<static_cast<int>(managedBuses.size())
			<<"\",\"Bus_xPos\":\""<<static_cast<int>(posBus.x)
			<<"\",\"Bus_yPos\":\""<<static_cast<int>(posBus.y)
			<<"\"})"<<std::endl);
#endif
}

void sim_mob::BusController::buildSubscriptionList(std::vector<BufferedBase*>& subsList)
{
	Agent::buildSubscriptionList(subsList);
}
