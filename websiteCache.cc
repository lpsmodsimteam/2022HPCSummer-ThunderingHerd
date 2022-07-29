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
    requestLengthTime = params.find<int64_t>("websiteRefreshLength", 5);

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

bool websiteCache::clockTick( SST::Cycle_t currentCycle ) {
    // output.output(CALL_INFO, "Cache Sim-Time: %ld\n", getCurrentSimTimeNano());
    if (memoryRequests.size() > 0) {
        CacheRequestEvent *cacheev = memoryRequests.front();
        memoryRequests.pop();
    
    if ( cacheev != NULL ) {
        requester requester = cacheev->cachereq.request;
        int userID = cacheev->cachereq.id;
        std::string pageRequested = cacheev->cachereq.pageRequested;
        std::string urlRequested = cacheev->cachereq.urlRequested;
        bool successfulReturn = cacheev->cachereq.successfulReturn;

        // temporarily just always return true
        // double checking that connections work properly so far
        if (requester == USER) {
            output.output(CALL_INFO, "user request \n");
            if (websitesInCache.count(pageRequested)) {
                websitesInCache[pageRequested].websiteAge = getCurrentSimTimeNano();
                output.output(CALL_INFO, "returning page %s \n", websitesInCache[pageRequested].websiteUrl.c_str());
                struct UserRequest userreq = { websitesInCache[pageRequested].websiteUrl, true };
                returnUserLink(userID)->send(new UserRequestEvent(userreq));
                // update lrutracker 
            } else {
                // send request to server
                // send back invalid bit to user
                // they have to wait until server processes request
                struct ServerRequest serverreq = { pageRequested, userID };
                returnUserLink(0)->send(new ServerRequestEvent(serverreq));
            }
            // necessary functions:
            // randomize error for sites to have to access server
            // check cache, if not send to server
        } else if ( requester == SERVER ) {
            // cache replacement policy
            output.output(CALL_INFO, "server request \n");
            if (successfulReturn) {
                if (websitesInCache.size() < maxCacheSize) {
                    struct cacheObject newsite = {urlRequested, getCurrentSimTimeNano()};
                    // instead of age tracker, use queue to keep track of LRU
                    // need to make sure queue only stores unique values
                    // check this when we delete something from the cache
                    websitesInCache[pageRequested] = newsite;
                } else {
                    long int oldestTime = 990000000000;
                    std::string lruSite = "";
                    std::map<std::string, cacheObject>::iterator it = websitesInCache.begin();


                    while (it != websitesInCache.end()) {
                        // Accessing the key
                        std::string word = it->first;
                        // output.output(CALL_INFO, "iterating on %s \n",word.c_str());
                        // Accessing the value
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
