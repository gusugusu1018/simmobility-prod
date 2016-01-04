//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//license.txt   (http://opensource.org/licenses/MIT)

#include "Agent_LT.hpp"

#include <cstdlib>
#include <cmath>
#include <boost/lexical_cast.hpp>

#include "conf/ConfigManager.hpp"
#include "conf/ConfigParams.hpp"
#include "conf/settings/ProfileOptions.h"
#include "conf/settings/DisableMPI.h"
#include "conf/settings/StrictAgentErrors.h"
#include "entities/profile/ProfileBuilder.hpp"
#include "event/SystemEvents.hpp"

#include "logging/Log.hpp"
#include "message/MessageBus.hpp"
#include "partitions/PartitionManager.hpp"
#include "partitions/PackageUtils.hpp"
#include "partitions/UnPackageUtils.hpp"
#include "workers/Worker.hpp"
#include "util/LangHelpers.hpp"
#include "util/DebugFlags.hpp"


using namespace sim_mob;
using namespace sim_mob::event;
using namespace sim_mob::messaging;

typedef Entity::UpdateStatus UpdateStatus;

using std::vector;
using std::priority_queue;

StartTimePriorityQueue_lt sim_mob::Agent_LT::pending_agents;
std::set<Entity*> sim_mob::Agent_LT::all_agents;
//EventTimePriorityQueue sim_mob::Agent::agents_with_pending_event;
//vector<Entity*> sim_mob::Agent::agents_on_event;

//Implementation of our comparison function for Agents by start time.
bool sim_mob::cmp_agent_lt_start::operator()(const Agent_LT* x, const Agent_LT* y) const
{

	//TODO: Not sure what to do in this case...
	if ((!x) || (!y))
	{
		return 0;
	}

	//We want a lower start time to translate into a higher priority.
	return x->getStartTime() > y->getStartTime();
}

//Implementation of our comparison function for events by start time.
bool sim_mob::cmp_event_lt_start::operator()(const PendingEvent& x,		const PendingEvent& y) const
{
	//We want a lower start time to translate into a higher priority.
	return x.start > y.start;
}

unsigned int sim_mob::Agent_LT::next_agent_id = 0;
unsigned int sim_mob::Agent_LT::GetAndIncrementID(int preferredID)
{
	//If the ID is valid, modify next_agent_id;
	if (preferredID > static_cast<int>(next_agent_id))
	{
		next_agent_id = static_cast<unsigned int>(preferredID);
	}

#ifndef SIMMOB_DISABLE_MPI
	if (ConfigManager::GetInstance().FullConfig().using_MPI)
	{
		PartitionManager& partitionImpl = PartitionManager::instance();
		int mpi_id = partitionImpl.partition_config->partition_id;
		int cycle = partitionImpl.partition_config->maximum_agent_id;
		return (next_agent_id++) + cycle * mpi_id;
	}
#endif

	//Assign either the value asked for (assume it will not conflict) or
	//  the value of next_agent_id (if it's <0)
	unsigned int res = (preferredID >= 0) ? static_cast<unsigned int>(preferredID) : next_agent_id++;

	//std::cout <<"  assigned: " <<res <<std::endl;

	return res;
}

void sim_mob::Agent_LT::SetIncrementIDStartValue(int startID,bool failIfAlreadyUsed)
{
	//Check fail condition
	if (failIfAlreadyUsed && Agent_LT::next_agent_id != 0)
	{
		throw std::runtime_error("Can't call SetIncrementIDStartValue(); Agent ID has already been used.");
	}

	//Fail if we've already passed this ID.
	if (Agent_LT::next_agent_id > startID)
	{
		throw std::runtime_error("Can't call SetIncrementIDStartValue(); Agent ID has already been assigned.");
	}

	//Set
	Agent_LT::next_agent_id = startID;
}

sim_mob::Agent_LT::Agent_LT(	const MutexStrategy& mtxStrat, int id) : Entity(GetAndIncrementID(id)), mutexStrat(mtxStrat), initialized(false),
								lastUpdatedFrame(-1),toRemoved(false), dynamic_seed(id), currTick(0,0)
{}

sim_mob::Agent_LT::~Agent_LT(){}

void sim_mob::Agent_LT::resetFrameInit()
{
	initialized = false;
}

long sim_mob::Agent_LT::getLastUpdatedFrame()
{
	return lastUpdatedFrame;
}

void sim_mob::Agent_LT::setLastUpdatedFrame(long lastUpdatedFrame)
{
	this->lastUpdatedFrame = lastUpdatedFrame;
}

void sim_mob::Agent_LT::buildSubscriptionList(std::vector<sim_mob::BufferedBase*>& subsList){}

void sim_mob::Agent_LT::CheckFrameTimes(unsigned int agentId, uint32_t now, unsigned int startTime, bool wasFirstFrame, bool wasRemoved)
{
	//Has update() been called early?
	if (now<startTime)
	{
		std::stringstream msg;
		msg << "Agent(" <<agentId << ") specifies a start time of: " <<startTime
			<< " but it is currently: " << now
			<< "; this indicates an error, and should be handled automatically.";
		throw std::runtime_error(msg.str().c_str());
	}

	//Has update() been called too late?
	if (wasRemoved)
	{
		std::stringstream msg;

		msg << "Agent(" <<agentId << ") should have already been removed, but was instead updated at: " <<now
			<< "; this indicates an error, and should be handled automatically.";
		throw std::runtime_error(msg.str().c_str());
	}
	//Was frame_init() called at the wrong point in time?
	if (wasFirstFrame)
	{
		if (abs(now-startTime)>=ConfigManager::GetInstance().FullConfig().baseGranMS())
		{
			std::stringstream msg;
			msg <<"Agent was not started within one timespan of its requested start time.";
			msg <<"\nStart was: " <<startTime <<",  Curr time is: " <<now <<"\n";
			msg <<"Agent ID: " <<agentId <<"\n";
			throw std::runtime_error(msg.str().c_str());
		}
	}
}



UpdateStatus sim_mob::Agent_LT::perform_update(timeslice now)
{
	//We give the Agent the benefit of the doubt here and simply call frame_init().
	//This allows them to override the start_time if it seems appropriate (e.g., if they
	// are swapping trip chains). If frame_init() returns false, immediately exit.
	bool calledFrameInit = false;

	if (!initialized)
	{
		//Call frame_init() and exit early if requested to.
		if (!frame_init(now))
		{
			return UpdateStatus::Done;
		}

		//Set call_frame_init to false here; you can only reset frame_init() in frame_tick()
		initialized = true; //Only initialize once.
		calledFrameInit = true;
	}

	//Now that frame_init has been called, ensure that it was done so for the correct time tick.
	CheckFrameTimes(getId(), now.ms(), getStartTime(), calledFrameInit, isToBeRemoved());

	//Perform the main update tick
	UpdateStatus retVal = frame_tick(now);

	//Save the output
	if (retVal.status != UpdateStatus::RS_DONE)
	{
		frame_output(now);
	}

	//Output if removal requested.
	if (Debug::WorkGroupSemantics && isToBeRemoved())
	{
		LogOut("Person requested removal: " <<"(Role Hidden)" <<std::endl);
	}

	return retVal;
}
/*
Entity::UpdateStatus sim_mob::Agent_LT::update(timeslice now)
{
	PROFILE_LOG_AGENT_UPDATE_BEGIN(currWorkerProvider, this, now);

	//Update within an optional try/catch block.
	UpdateStatus retVal(UpdateStatus::RS_CONTINUE);

#ifndef SIMMOB_STRICT_AGENT_ERRORS
	try
	{
#endif
		//Update functionality
		retVal = perform_update(now);

//Respond to errors only if STRICT is off; otherwise, throw it (so we can catch it in the debugger).
#ifndef SIMMOB_STRICT_AGENT_ERRORS
	}
	catch (std::exception& ex)
	{
		//TODO: We can't handle this right now.
		//PROFILE_LOG_AGENT_EXCEPTION(currWorkerProvider->getProfileBuilder(), *this, now, ex);

		//Add a line to the output file.
		if (ConfigManager::GetInstance().CMakeConfig().OutputEnabled())
		{
			std::stringstream msg;
			msg <<"Error updating Agent[" <<getId() <<"], will be removed from the simulation.";

			msg <<"\n  " <<ex.what();
			LogOut(msg.str() <<std::endl);
		}
		setToBeRemoved();
	}
#endif

	//Ensure that isToBeRemoved() and UpdateStatus::status are in sync
	if (isToBeRemoved() || retVal.status == UpdateStatus::RS_DONE)
	{
		retVal.status = UpdateStatus::RS_DONE;
		setToBeRemoved();

		//notify subscribers that this agent is done
		MessageBus::PublishEvent(event::EVT_CORE_AGENT_DIED, this, MessageBus::EventArgsPtr(new AgentLifeCycleEventArgsLT(getId(), this)));
                
        //unsubscribes all listeners of this agent to this event.
        //(it is safe to do this here because the priority between events)
        MessageBus::UnSubscribeAll(event::EVT_CORE_AGENT_DIED, this);
	}

	PROFILE_LOG_AGENT_UPDATE_END(currWorkerProvider, this, now);
	return retVal;
}
*/

bool sim_mob::Agent_LT::isToBeRemoved()
{
	return toRemoved;
}

void sim_mob::Agent_LT::setToBeRemoved()
{
	toRemoved = true;
}

void sim_mob::Agent_LT::clearToBeRemoved()
{
	toRemoved = false;
}

NullableOutputStream sim_mob::Agent_LT::Log()
{
	return NullableOutputStream(currWorkerProvider->getLogFile());
}

void sim_mob::Agent_LT::HandleMessage(messaging::Message::MessageType type, const messaging::Message& message){}

#ifndef SIMMOB_DISABLE_MPI
int sim_mob::Agent_LT::getOwnRandomNumber()
{
	int one_try = -1;
	int second_try = -2;
	int third_try = -3;
	//		int forth_try = -4;

	while (one_try != second_try || third_try != second_try)
	{
		srand(dynamic_seed);
		one_try = rand();

		srand(dynamic_seed);
		second_try = rand();

		srand(dynamic_seed);
		third_try = rand();
	}

	dynamic_seed = one_try;
	return one_try;
}
#endif



bool Agent_LT::frame_init(timeslice now)
{
    return onFrameInit(now);
}

Entity::UpdateStatus Agent_LT::frame_tick(timeslice now)
{
    return onFrameTick(now);
}

void Agent_LT::frame_output(timeslice now)
{
    onFrameOutput(now);
}



bool Agent_LT::isNonspatial()
{
    return false;
}

Entity::UpdateStatus Agent_LT::update(timeslice now)
{
	return onFrameTick(now);
}




