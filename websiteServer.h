#ifndef _websiteServer_H
#define _websiteServer_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include <map>
#include <queue>
#include "requests.h"

/**
 * @file websiteServer.h
 * @brief This creates a server that holds all the urls for the websites that 
 * users can access
 * 
 */

class websiteServer : public SST::Component {

public:
	/**
	 * @brief Construct a new website Server object
	 * 
	 * @param id The id for the component, this is passed in by SST. Usually 
	 * just need to pass it to the base SST::Component constructor
	 * @param params The params set by the project driver
	 */
	websiteServer( SST::ComponentId_t id, SST::Params& params );

	/**
	 * @brief Destroy the website Server object
	 * 
	 */
	~websiteServer();

	/**
	 * @brief This clock function checks the status of the philosopher, 
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
	 * @param currentCycle This tells us what cycle of the simulation we're on
	 * @return This returns whether or not the simulation should continue 
	 */
	bool clockTick( SST::Cycle_t currentCycle );

	/**
	 * @brief This function recieves messages fromn the cache, and adds them to 
	 * a queue to be processed in the clock function
	 * 
	 * @param ev An event object that contains the details of the request
	 * @return void
	 */
    void handleEvent(SST::Event *ev);

	/**
	 * \cond
	 */
	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		websiteServer, // class
		"thunderingHerd", // element library
		"websiteServer", // component
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
		{ "websiteCache", "Communication to website cache", {"sst.Interfaces.StringEvent", "websiteCache"}},
	)
	/**
	 * \endcond
	 */

private:
	/* Output about the simulation to display in the terminal */
	SST::Output output;

	/* A link to the cache */
	SST::Link *websiteCache;

	/* Clock that checks and updates the state of the philosopher every cycle */
	std::string clock;				
	std::string websiteBrowsingLength; // defines clock frequency


    std::map<std::string, std::string> websites;	// map to model cache
    std::queue<ServerRequestEvent*> memoryRequests; // queue to hold requests
    int maxQueueSize;
};

#endif