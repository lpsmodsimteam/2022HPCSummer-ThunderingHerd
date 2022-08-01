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
 * @brief This creates a website cache component that holds some of the most 
 * recently used urls, connecting to both users and the server
 * 
 */

class websiteCache : public SST::Component {

public:
	/**
	 * @brief Construct a new website Cache object
	 * 
	 * @param id The id for the component, this is passed in by SST. Usually 
	 * just need to pass it to the base SST::Component constructor
	 * @param params The params set by the project driver
	 */
	websiteCache( SST::ComponentId_t id, SST::Params& params );

	/**
	 * @brief Destroy the website Cache object
	 * 
	 */
	~websiteCache();

	/**
	 * @brief This clock function checks the queue, and processes 
	 * a defined amount of requests every cycle.
	 * 
	 * @param currentCycle This tells us what cycle of the simulation we're on.
	 * @return This returns whether or not the simulation should continue 
	 */
	bool clockTick( SST::Cycle_t currentCycle );

	/**
	 * @brief Takes in the numerical id passed to the cache, and returns 
	 * the correct port for the corresponding user, or server
	 * 
	 * @param userid internal ID for each user
	 * @return SST::Link* returns the port to connect to the correct port
	 */
	SST::Link * returnUserLink(int userid);

	/**
	 * @brief This function recieves messages from both the server and the 
	 * users, and queues them up to be processed in the clock function
	 * 
	 * @param ev An event object that contains the details of the request
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
		"cache to handle website requests from users, and connect to a server",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "randomseed", "Random Seed for errors within simulation", "151515" },
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "userOne", "Communication to user one", {"sst.Interfaces.StringEvent"}},
        { "userTwo", "Communication to user two", {"sst.Interfaces.StringEvent"}},
        { "userThree", "Communication to user three", {"sst.Interfaces.StringEvent"}},
        { "userFour", "Communication to user four", {"sst.Interfaces.StringEvent"}},
        { "userFive", "Communication to user five", {"sst.Interfaces.StringEvent"}},
		{ "websiteServer", "Communication to website server", {"sst.Interfaces.StringEvent"}},
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

    std::map<std::string, cacheObject> websitesInCache;	/* internal map to model cache */
	std::queue<CacheRequestEvent*> memoryRequests; 		/* holds requests to cache */
	int maxCacheSize;									/* size limit to cache */
};

#endif