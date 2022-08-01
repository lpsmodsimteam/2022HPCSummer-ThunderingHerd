#ifndef _websiteUser_H
#define _websiteUser_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <sst/core/event.h>
#include <vector>
#include "requests.h"

/**
 * @file websiteUser.h
 * @brief This creates a dining Philosopher component that cycles between eating, 
 * thinking, and hungry states, and communicates with the dining table component
 * when it needs a chopstick to eat
 * 
 */
class websiteUser : public SST::Component {

public:
	/**
	 * @brief Construct a new website User object
	 * 
	 * @param id The id for the component, this is passed in by SST. Usually 
	 * just need to pass it to the base SST::Component constructor
	 * @param params The params set by the project driver
	 */
	websiteUser( SST::ComponentId_t id, SST::Params& params );
	
	/**
	 * @brief Destroy the website User object
	 * 
	 */
	~websiteUser();

	/**
	 * @brief This function randomizes which website each user requests 
	 * in the beginning of the simulation in order to randomize it.
	 * 
	 * Our simulation defines a random seed for each user, and this seed 
	 * dictates which site they'll choose to try to access first
	 * @return void
	 */
	void setup();

	/**
	 * @brief This first clock function defines the general behavior of a 
	 * user trying to access a website
	 * 
	 * This function contains the main functionality of the entire simulation.  
	 * It checks what state the user is in, keeping track of the browsing length
	 * if they have a url, or requesting a website if they're ready to switch
	 * pages.  
	 * 
	 * @param currentCycle This tells us what cycle of the simulation we're on, 
	 * where each user's cycle may or may not have the same frequency.
	 * @return This returns whether or not the simulation should continue 
	 */
	bool clockTick( SST::Cycle_t currentCycle );
	/**
	 * @brief This second clock function defines the behavior if the user 
	 * hasn't recieved a response from the cache after a certain period of time
	 * 
	 * This function runs on a shorter frequency than the first clock function,
	 * since it defines the "impatient" behavior of the user.  If the user 
	 * hasn't heard back from the cache after a certain amount of time, this
	 * function will trigger, and they'll begin to spam the cache with requests 
	 * until they recieve the site they're looking for.
	 * 
	 * @param currentCycle This tells us what cycle of the simulation we're on, 
	 * where each user's cycle may or may not have the same frequency.
	 * @return This returns whether or not the simulation should continue 
	 */
	bool waitingTick( SST::Cycle_t currentCycle );

	/** 
	 * @brief handles messages sent to the users back from the cache
	 * 
	 * This function unwraps the information that was sent back from the 
	 * request to the cache, and checks to see whether or not the
	 * website that was asked for is actually available.  
 	 *
 	 * @param ev An event object that holds the contents of the message
	 * sent back from the cache
 	 * @return void
 	*/
    void handleEvent(SST::Event *ev);

	/**
	 * \cond
	 */
	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		websiteUser, // class
		"thunderingHerd", // element library
		"websiteUser", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"users who browse various pages on a website",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "websiteBrowsingLength", "How long to wait between checking user status", "10s" },
		{ "websiteRefreshLength", "How long to wait between impatiently waiting for a website", "2s" },
		{ "requestTimeoutLength", "How many cycles to wait for a cache response", "5" },
		{ "id", "id for the user", "1" },
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "websiteCache", "Connecting port to the website cache", {"sst.Interfaces.StringEvent", "leftPort"}},
	)
	/**
	 * \endcond
	 */

private:
	SST::Output output;						/* Standard output to the terminal */
	SST::RNG::MarsagliaRNG* rng; 			/* Random number generator for our first website request */
	std::string clock;						/* Clock that checks and updates the state of the philosopher every cycle */
	std::string waitingClock;				/* Clock that checks whether or not we need to return chopsticks every cycle */
	int64_t RandomSeed;						/* Seed pased into the MarsagliaRNG */
	SST::Link *websiteCache;				/* Link to connect philosopher with dining table */


	int userID;								/* id for cache to identify users */
	std::string websiteBrowsingLength;		/* defines cycle length for clock */
	std::string websiteRefreshLength;		/* defines cycle length for waitingClock */
	SST::Cycle_t requestTimeoutLength;		/* how many cycles a user will wait for a response until becoming impatient */
	std::string currentWebsite;				/* url of current website being browsed */
	std::vector<std::string> listOfPages;	/* names of websites a user can request */
	userStatus currentStatus;				/* status of the user */
	SST::Cycle_t startWaitingCycle;			/* when a user started waiting for a cache response */
	int currentWebsiteRequest;				/* spot in vector that holds the name of the website request */
};

#endif