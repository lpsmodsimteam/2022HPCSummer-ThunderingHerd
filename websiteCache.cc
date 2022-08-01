#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/stopAction.h>
#include <sst/core/simulation.h>
#include "websiteCache.h"

websiteCache::websiteCache( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {

    // initalizes the name of the cache for our output
    output.init("websiteCache-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);
    
    // This grabs the parameters that were defined in the python test file in 
    // order to initalize our component
	websiteBrowsingLength = params.find<std::string>("websiteBrowsingLength", "10ms");
    maxCacheSize = params.find<int64_t>("maxCacheSize", 6);

    /*
     * The register clock functions take in a duration of time that was defined 
     * by the parameters above, and ties a function to it that is called 
     * every time this cycle passes.  
     */
	registerClock(websiteBrowsingLength, new SST::Clock::Handler<websiteCache>(this, &websiteCache::clockTick));
	
	// Configure our ports
	userOne = configureLink("userOne", "1ns", new SST::Event::Handler<websiteCache>(this, &websiteCache::handleEvent));
	if ( !userOne ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'userOne'\n");
	}
    userTwo = configureLink("userTwo", "1ns", new SST::Event::Handler<websiteCache>(this, &websiteCache::handleEvent));
	if ( !userTwo ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'userTwo'\n");
	}
    userThree = configureLink("userThree", "1ns", new SST::Event::Handler<websiteCache>(this, &websiteCache::handleEvent));
	if ( !userThree ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'userThree'\n");
	}
    userFour = configureLink("userFour", "1ns", new SST::Event::Handler<websiteCache>(this, &websiteCache::handleEvent));
	if ( !userFour ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'userFour'\n");
	}
    userFive = configureLink("userFive", "1ns", new SST::Event::Handler<websiteCache>(this, &websiteCache::handleEvent));
	if ( !userFive ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'userFive'\n");
	}
    websiteServer = configureLink("websiteServer", "1ns", new SST::Event::Handler<websiteCache>(this, &websiteCache::handleEvent));
	if ( !websiteServer ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'websiteServer'\n");
	}
}

websiteCache::~websiteCache() {

}

// TODO functions:
// randomize error for sites to have to access server
bool websiteCache::clockTick( SST::Cycle_t currentCycle ) {
    // output.output(CALL_INFO, "Cache Sim-Time: %ld\n", getCurrentSimTimeNano());
    // check if there's a request in the queue to process
    if (memoryRequests.size() > 0) {
        CacheRequestEvent *cacheev = memoryRequests.front();
        memoryRequests.pop();
    
    if ( cacheev != NULL ) {
        // unwrap CacheRequestEvent
        requester requester = cacheev->cachereq.request;
        int userID = cacheev->cachereq.id;
        std::string pageRequested = cacheev->cachereq.pageRequested;
        std::string urlRequested = cacheev->cachereq.urlRequested;
        bool successfulReturn = cacheev->cachereq.successfulReturn;

        // cache recieves requests from both server and users,
        // so we need to differentiate the two
        if (requester == USER) {
            output.output(CALL_INFO, "recieved a user request \n");
            // check if we have url saved in cache
            if (websitesInCache.count(pageRequested)) {
                // access the url the user requested, and send it to them
                // wrap the message in the UserRequestEvent
                // update websiteAge to indicate least recently used item
                websitesInCache[pageRequested].websiteAge = getCurrentSimTimeNano();
                output.output(CALL_INFO, "returning page %s \n", websitesInCache[pageRequested].websiteUrl.c_str());
                struct UserRequest userreq = { websitesInCache[pageRequested].websiteUrl, true };
                returnUserLink(userID)->send(new UserRequestEvent(userreq));
            } else {
                // send request to server for url
                struct ServerRequest serverreq = { pageRequested, userID };
                returnUserLink(0)->send(new ServerRequestEvent(serverreq));
            }

        // server is sending back a requested url, implement cache replacement
        } else if ( requester == SERVER ) {
            output.output(CALL_INFO, "recieved a server request \n");
            if (successfulReturn) {
                // if there's room in the cache, just insert it in the map
                if (websitesInCache.size() < maxCacheSize) {
                    struct cacheObject newsite = {urlRequested, getCurrentSimTimeNano()};
                    websitesInCache[pageRequested] = newsite;
                } else {
                    // map traversal from https://www.geeksforgeeks.org/traversing-a-map-or-unordered_map-in-cpp-stl/
                    // looping through to find the object that was inserted earliest
                    
                    // initalize time comparison to be the latest time in the simulation
                    long int oldestTime = 990000000000;
                    std::string lruSite = "";
                    std::map<std::string, cacheObject>::iterator it = websitesInCache.begin();

                    // iterate through map, compare time stamps
                    while (it != websitesInCache.end()) {
                        std::string word = it->first;
                        cacheObject cacheObject = it->second;
                        int cacheObjectTime = cacheObject.websiteAge;
                        if (cacheObjectTime < oldestTime) {
                            oldestTime = cacheObjectTime;
                            lruSite = word;
                        }
                        // iterator incremented to point next item
                        it++;
                    }
                    // found oldest item to be replaced, delete from cache and replace
                    output.output(CALL_INFO, "oldest item is %s \n",lruSite.c_str());
                    websitesInCache.erase(lruSite);
                    output.output(CALL_INFO, "replacing item with %s \n",urlRequested.c_str());
                    struct cacheObject newsite = {urlRequested, getCurrentSimTimeNano()};
                    websitesInCache[pageRequested] = newsite;
                }
            }
        }
    }
    }
    return false;
}

void websiteCache::handleEvent(SST::Event *ev) {
    // push incoming requests to a queue
    output.output(CALL_INFO, "Sim-Time in cache: %ld\n", getCurrentSimTimeNano());
    CacheRequestEvent *cacheev = dynamic_cast<CacheRequestEvent*>(ev);
    memoryRequests.push(cacheev);
    output.output(CALL_INFO, "number of cache requests: %ld \n", memoryRequests.size());
}


SST::Link * websiteCache::returnUserLink(int userid) {
    switch (userid) {
    case 0:
        return websiteServer;
        break;
    case 1:
        return userOne;
        break;
    case 2:
        return userTwo;
        break;
    case 3:
        return userThree;
        break;
    case 4:
        return userFour;
        break;
    case 5:
        return userFive;
        break;
    default:
        return 0; // unexpected id, return null
        break;
    }
}
