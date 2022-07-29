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
	 * @brief Construct a new dining Philosopher object
	 * 
	 * @param id The id for the component, this is passed in by SST. Usually 
	 * just need to pass it to the base SST::Component constructor
	 * @param params The params set by the project driver
	 */
	websiteUser( SST::ComponentId_t id, SST::Params& params );
	
	/**
	 * @brief Destroy the dining Philosopher object
	 * 
	 */
	~websiteUser();

	/**
	 * @brief This function randomizes which chopstick each philosopher grabs 
	 * in the beginning of the simulation in order to randomize it.
	 * 
	 * Our simulation defines a random seed for each philosopher, and this seed 
	 * dictates whether or not the first request to the dining table is sent 
	 * for the chopstick on the left, or for the one on the right
	 * 
	 * @return void
	 */
	void setup();

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
	 * @brief This second clock function induces livelock by emptying the 
	 * philosophers' hands if they only hold one chopstick.
	 * 
	 * This function may or may not run on a different cycle from the previous 
	 * clock in order to induce livelock.  We avoid deadlock in this simulation 
	 * by making our philosophers "polite", and not allowing them to only hold 
	 * one chopstick for an extended period of time.  If a philosopher is only 
	 * holding one chopstick, we initiatie an event to send that chopstick back 
	 * to the dining table for someone else to grab.
	 * 
	 * @param currentCycle This tells us what cycle of the simulation we're on, 
	 * where each philosopher's cycle may or may not have the same frequency.
	 * @return This returns whether or not the simulation should continue 
	 */
	bool waitingTick( SST::Cycle_t currentCycle );

	/** 
	 * @brief handles messages sent to the dining philosophers 
	 * back from dining table
	 * 
	 * This function unwraps the information that was sent back from the 
	 * request to the dining table, and checks to see whether or not the
	 * chopstick that was asked for is actually available.  
 	 *
 	 * @param ev An event object that holds the contents of the message
	 * sent back from the dining table
 	 * @return void
 	*/
    void handleEvent(SST::Event *ev);


	/** @brief function to output current status of simulation
	 * 
	 * This function places a majority of the status outputs into one function
	 * that can be called anywhere to see where chopsticks are being held
	 *  @return void
 	*/
	void outputTickInfo();

	/**
	 * \cond
	 */
	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		websiteUser, // class
		"thunderingHerd", // element library
		"websiteUser", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"philosopher to model the dining philosophers problem",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "thinkingDuration", "How long to wait between thinking and trying to eat", "15s" },
		{ "waitingClock", "How long to wait between checking if I have both chopsticks", "13s" },
		{ "randomseed", "Random Seed for car type generation", "151515" },
		{ "eatingduration", "length of time to eat", "2000" },
		{ "id", "id for dining table", "1" },
		{ "livelockCheck", "number of cycles to wait before livelock check", "10000" },
		{ "windowSize", "length of window for livelock detection", "100" },
	)

	// Statistic name, description, unit, enable level
	SST_ELI_DOCUMENT_STATISTICS(
        { "eatingCounterStats", "Number of times a philosopher ate", "counter", 1 },  
        { "thinkingCounterStats", "Number of times a philosopher thought", "counter", 1 },
        { "hungryCounterStats", "Number of times a philosopher was hungry", "counter", 1 }
    )

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "websiteCache", "Connecting port to the website cache", {"sst.Interfaces.StringEvent", "leftPort"}},
	)
	/**
	 * \endcond
	 */

private:
	SST::Output output;				/* Standard output to the terminal */
	SST::RNG::MarsagliaRNG* rng; 	/* Random number generator for our first website request */
	std::string clock;				/* Clock that checks and updates the state of the philosopher every cycle */
	std::string waitingClock;		/* Clock that checks whether or not we need to return chopsticks every cycle */
	int64_t RandomSeed;				/* Seed pased into the MarsagliaRNG */
	SST::Link *websiteCache;		/* Link to connect philosopher with dining table */


	int userID;
	std::string websiteBrowsingLength;
	std::string websiteRefreshLength;
	SST::Cycle_t requestTimeoutLength;
	std::string currentWebsite;
	std::vector<std::string> listOfPages;
	userStatus currentStatus;
	SST::Cycle_t startWaitingCycle;
	int currentWebsiteRequest;
};

#endif