#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "websiteServer.h"

websiteServer::websiteServer( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {

    // initalizes the name of the server for our output
    output.init("websiteServer-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // This grabs the parameters that were defined in the python test file in 
    // order to initalize our component
	websiteBrowsingLength = params.find<std::string>("websiteBrowsingLength", "5s");

    /*
     * The register clock functions take in a duration of time that was defined 
     * by the parameters above, and ties a function to it that is called 
     * every time this cycle passes.  
     */
	registerClock(websiteBrowsingLength, new SST::Clock::Handler<websiteServer>(this, &websiteServer::clockTick));
	
	// initalize the server to have all the available websites, and their urls
    websites["login"] = "login.com";
    websites["profile1"] = "profile1.com";
    websites["profile2"] = "profile2.com";
    websites["profile3"] = "profile3.com";
    websites["profile4"] = "profile4.com";
    websites["settings"] = "settings.com";
    websites["about"] = "about.com";

	// Configure our port to the cache
	websiteCache = configureLink("websiteCache", "1ns", new SST::Event::Handler<websiteServer>(this, &websiteServer::handleEvent));
	if ( !websiteCache ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'websiteCache'\n");
	}
}

websiteServer::~websiteServer() {

}

bool websiteServer::clockTick( SST::Cycle_t currentCycle ) {
    // output.output(CALL_INFO, "Server Sim-Time: %ld\n", getCurrentSimTimeNano());
    // check that there's an event in the queue to process
    if (memoryRequests.size() > 0) {
        ServerRequestEvent *serverev = memoryRequests.front();
        memoryRequests.pop();
        if ( serverev != NULL ) {
        std::string pageRequested = serverev->serverreq.pageRequested;
        int userID = serverev->serverreq.id;

        // temporarily just always return true
        // future step: randomize bad requests from server
        output.output(CALL_INFO, "is now sending over to cache: %s \n", websites[pageRequested].c_str());
        struct CacheRequest cachereq = { SERVER, userID, pageRequested, websites[pageRequested], 1 };
        websiteCache->send(new CacheRequestEvent(cachereq));
        }
    }
    return false;
}

void websiteServer::handleEvent(SST::Event *ev) {
    // push all requests to server to a queue
    ServerRequestEvent *serverev = dynamic_cast<ServerRequestEvent*>(ev);
    memoryRequests.push(serverev);
    output.output(CALL_INFO, "number of server requests: %ld \n", memoryRequests.size());
}
