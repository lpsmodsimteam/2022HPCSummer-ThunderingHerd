#ifndef _websiteCache_H
#define _websiteCache_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include "requests.h"
#include <queue>

/**
 * @file websiteCache.h
 * @brief This creates a dining table component that stores all of the shared 
 * chopsticks for the simulation, as well as who is allowed to hold each one
 * 
 */

class websiteCache : public SST::Component {

public:
	/**
	 * @brief Construct a new dining Table object
	 * 
	 * @param id The id for the component, this is passed in by SST. Usually 
	 * just need to pass it to the base SST::Component constructor
	 * @param params The params set by the project driver
	 */
	websiteCache( SST::ComponentId_t id, SST::Params& params );

	/**
	 * @brief Destroy the dining Table object
	 * 
	 */
	~websiteCache();

	/**
	 * @brief This first clock function checks the status of the philosopher, 
	 * and updates their needs accordingly.  It also performs livelock 
	 * detection
	 * 
	 * This function contains the main functionality of the entire simulation.  
	 * It starts out by checking if we need to update our livelock window, which
	 * checks the status of the system and notes down the current livelock 
	 * metrics.  After this, we check the status of the philosopher, sending a 
	 * request to the dining table for a chopstick if we're hungry, or seeing if 
	 * we've elapsed out eating or thinking durations to switch states.
	 * 
	 * @param currentCycle This tells us what cycle of the simulation we're on, 
	 * where each philosopher's cycle may or may not have the same frequency.
	 * @return This returns whether or not the simulation should continue 
	 */
	bool clockTick( SST::Cycle_t currentCycle );

	/**
	 * @brief Takes in the numerical id passed to the dining table, and returns 
	 * the correct port for the corresponding philosopher
	 * 
	 * @param philid internal ID for each dining philosopher
	 * @return SST::Link* returns the port to connect to that philosopher
	 */
	SST::Link * returnUserLink(int philid);

	/**
	 * @brief This function recieves chopstick requests from the philosophers, 
	 * and checks whether or not these chopsticks are available to be passed 
	 * back onto them
	 * 
	 * @param ev An event object that contains the details of the chopstick 
	 * request
	 */
    void handleEvent(SST::Event *ev);

	/**
	 * \cond
	 */
	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		websiteCache, // class
		"thunderingHerd", // element library
		"websiteCache", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"table to hold set of chopsticks",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "randomseed", "Random Seed for car type generation", "151515" },
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "userOne", "Communication to user one", {"sst.Interfaces.StringEvent", "one"}},
        { "userTwo", "Communication to user two", {"sst.Interfaces.StringEvent", "two"}},
        { "userThree", "Communication to user three", {"sst.Interfaces.StringEvent", "three"}},
        { "userFour", "Communication to user four", {"sst.Interfaces.StringEvent", "four"}},
        { "userFive", "Communication to user five", {"sst.Interfaces.StringEvent", "five"}},
		{ "websiteServer", "Communication to website server", {"sst.Interfaces.StringEvent", "server"}},
	)
	/**
	 * \endcond
	 */

private:
	/* Output about the simulation to display in the terminal */
	SST::Output output;
	/* Clock that checks and updates the state of the philosopher every cycle */
	std::string clock;				
	std::string websiteBrowsingLength; // defines frequency of clock

	/* A collection of links to each of the users and the server */
	SST::Link *userOne;
    SST::Link *userTwo;
    SST::Link *userThree;
    SST::Link *userFour;
    SST::Link *userFive;
	SST::Link *websiteServer;

    std::map<std::string, cacheObject> websitesInCache;
	std::queue<CacheRequestEvent*> memoryRequests; 
    SST::Cycle_t requestLengthTime;
	int maxCacheSize;
};

#endif