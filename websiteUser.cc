/**
 * Model of users trying to access a website to illustrate the thundering 
 * herd problem
 *
 */

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "websiteUser.h"

websiteUser::websiteUser( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
	
    // initalizes the name of each user for our output
	output.init("websiteUser-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// This grabs the parameters that were defined in the python test file in 
    // order to initalize our component
	websiteBrowsingLength = params.find<std::string>("websiteBrowsingLength", "10s");
    websiteRefreshLength = params.find<std::string>("websiteRefreshLength", "2s");
	requestTimeoutLength = params.find<int64_t>("requestTimeoutLength", 5);
    userID = params.find<int8_t>("id", 1);

    // initialization of internal variables
    // user knows the names of the websites they can potentially visit
    // they just need to request the proper urls for access
    listOfPages = {"home", "login", "profile1", "profile2", "profile3", "profile4", "settings", "about"};
    currentWebsite = "none";
    currentStatus = REQUESTING;

    /*
     * The register clock functions take in a duration of time that was defined 
     * by the parameters above, and ties a function to it that is called 
     * every time this cycle passes.  In this simulation, we have two 
     * different clocks that run on different cycles.  The reasoning behind this 
     * is defined in more detail in the report file.
     */
	registerClock(websiteBrowsingLength, new SST::Clock::Handler<websiteUser>(this, &websiteUser::clockTick));
	registerClock(websiteRefreshLength, new SST::Clock::Handler<websiteUser>(this, &websiteUser::waitingTick));
	
	// Initialize private variables
	rng = new SST::RNG::MarsagliaRNG(15, RandomSeed);

    // register users as a primary component
    // this function means that in order for our simulation to exit, every
    // component that was registered as "primary" has to okay an exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
	
	// Configure our port, which links us to other components in the simulation
	websiteCache = configureLink("websiteCache", "1ns", new SST::Event::Handler<websiteUser>(this, &websiteUser::handleEvent));
	if ( !websiteCache ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'websiteCache'\n");
	}
}

websiteUser::~websiteUser() {

}

void websiteUser::setup() {
    // randomize grab of first website
    int temp = (int)(rng->generateNextInt32());          
    currentWebsiteRequest = abs((int)(temp % 8));   
    std::string pageRequest = listOfPages.at(currentWebsiteRequest);
    output.output(CALL_INFO, "is now requesting %s \n", pageRequest.c_str());

    // request the url of this website from the cache
    struct CacheRequest cachereq = { USER, userID, pageRequest, pageRequest, 0 };
    websiteCache->send(new CacheRequestEvent(cachereq));
}

bool websiteUser::clockTick( SST::Cycle_t currentCycle ) {
    // clock based on websiteBrowsingLength
    output.output(CALL_INFO, "Sim-Time: %ld\n", getCurrentSimTimeNano());
    if (currentStatus == BROWSING) {
        // browse for one cycle, then request on the next
        output.output(CALL_INFO, "is now browsing\n");
        currentStatus = REQUESTING;
    } else if (currentStatus == REFRESHING) {
        // do nothing, let waitingClock take care of this
    } else if (currentStatus == REQUESTING) {
        // done browsing, needs a new site to look at
        int temp = (int)(rng->generateNextInt32());          
        currentWebsiteRequest = abs((int)(temp % 8));  
        std::string pageRequest = listOfPages.at(currentWebsiteRequest);
        output.output(CALL_INFO, "is now requesting %s \n", pageRequest.c_str());
        struct CacheRequest cachereq = { USER, userID, pageRequest, pageRequest, 0 };
        websiteCache->send(new CacheRequestEvent(cachereq));
        currentStatus = WAITING;
        startWaitingCycle = currentCycle;
    }
	return false;
}

bool websiteUser::waitingTick( SST::Cycle_t currentCycle ) {
    // clock based on websiteRefreshLength
    if (currentStatus == WAITING && (currentCycle > (startWaitingCycle + requestTimeoutLength))) {
        // being impatient, send another request for same website
        // essentially refreshing the page after it times out
        std::string pageRequest = listOfPages.at(currentWebsiteRequest);
        output.output(CALL_INFO, "is now re-requesting %s \n", pageRequest.c_str());
        struct CacheRequest cachereq = { USER, userID, pageRequest, pageRequest, 0 };
        websiteCache->send(new CacheRequestEvent(cachereq));
    }
    return false;
}

void websiteUser::handleEvent(SST::Event *ev) {
    output.output(CALL_INFO, "event is being handled in user \n");
    UserRequestEvent *userev = dynamic_cast<UserRequestEvent*>(ev);
	if ( userev != NULL ) {
        std::string websiteUrl = userev->userreq.websiteUrl;
        bool validSite = userev->userreq.validSite;
        if (validSite) {
            currentStatus = BROWSING;
        } else {
            // did not get a response from cache
            // impatient, so we will refresh after waiting for refresh time
            currentStatus = WAITING;
        }
    } 
}

void websiteUser::outputTickInfo() {
    output.verbose(CALL_INFO, 4, 0, "Sim-Time: %ld\n", getCurrentSimTimeNano());
}